#include "ftpsocket.h"
#include <QHostAddress>

bool FtpSocket::send(const QString& cmd)
{
    int ret = write(cmd.toStdString().c_str(),cmd.size());
    if(0 >= ret)
    {
        emit disconnected();
        return false;
    }
    // 设置要显示的文字颜色
    recode->setTextColor(QColor(0,255,0));
    // 显示发送到的内容
    recode->append("send："+cmd);
    qDebug("---send:%s",cmd.toStdString().c_str());
    return waitForBytesWritten();
}

bool FtpSocket::recv(const char* check_status)
{
    // 等待数据来临
    waitForReadyRead();
    // 接收数据
    bufArray = readAll();
    // 设置要显示的文字颜色
    recode->setTextColor(QColor(0,0,255));
    // 显示接收到的内容
    recode->append(bufArray);
    qDebug("---recv:%s",bufArray.data());
    // 检查接收到的状态码
    return !strncmp(bufArray.data(),check_status,3);

}

FtpSocket* FtpSocket::send_pasv(void)
{
    if(send("PASV\n"))
    {
        if(recv("227"))
        {
            char ip[16];
            unsigned char ip1,ip2,ip3,ip4;
            unsigned short port1,port2;
            sscanf(bufArray.data()+27,"%hhu,%hhu,%hhu,%hhu,%hu,%hu",
                  &ip1,&ip2,&ip3,&ip4,&port1,&port2);
            sprintf(ip,"%hhu.%hhu.%hhu.%hhu",ip1,ip2,ip3,ip4);
            FtpSocket* pasvSocket = new FtpSocket(recode);
            pasvSocket->connectToHost(QHostAddress(ip),port1*256+port2);
            pasvSocket->waitForConnected();
            return pasvSocket;
        }
    }
    return NULL;
}

void FtpSocket::send_user(const QString& user)
{
    QString cmd = "USER " + user + "\n";
    if(send(cmd))
        recv("331");
}

void FtpSocket::send_pass(const QString& pass)
{
    QString cmd = "PASS " + pass + "\n";
    if(send(cmd))
        recv("230");
}

void FtpSocket::send_pwd(QLineEdit* lineEdit)
{
    QString cmd = "PWD\n";
    if(send(cmd))
    {
        if(recv("257"))
        {
            bufArray[bufArray.size()-3] = '/';
            bufArray[bufArray.size()-2] = '\0';
            lineEdit->setText(bufArray.data()+5);
        }
    }
}

void FtpSocket::send_cwd(const QString& path)
{
    QString cmd = "CWD "+path+"\n";
    if(send(cmd))
        recv("250");
}

void FtpSocket::send_list(QTableWidget *tableWidget)
{
    // 开启PASV模式
    FtpSocket* pasvSocket = send_pasv();
    if(NULL == pasvSocket)
    {
        qDebug("开启PASV模式失败！");
        return;
    }

    send("LIST -al\n");
    recv("150");

    // 等待数据发送完毕一次性接收
    pasvSocket->waitForReadyRead();
    bufArray = pasvSocket->readAll();

    // 定义文件的图标
    QIcon dpng("./image/dir.png");
    QIcon fpng("./image/file.png");

    // 显示前清空上一次的数据
    tableWidget->setRowCount(0);

    char* fileInfo = bufArray.data();
    char name[256],size[10],user[32],group[32],month[4],day[3],time[6],mode[11],date[14];
    while(fileInfo = strchr(fileInfo+1,'\n'))
    {
        // 解析出各项数据
        sscanf(fileInfo+1,"%s%*d%s%s%s%s%s%s%s",mode,user,group,size,month,day,time,name);
        // 忽略点开头的文件
        if('.' == name[0] && '.' != name[1])
            continue;

        // 拼凑日期数据
        sprintf(date,"%s %s %s",month,day,time);
        // 获取表格的行数
        int rowCount = tableWidget->rowCount();
        // 增加一行数据
        tableWidget->setRowCount(rowCount+1);
        // 往新健的行中添加数据
        QTableWidgetItem* item0 = new QTableWidgetItem('d'==mode[0]?dpng:fpng,name);
        QTableWidgetItem* item1 = new QTableWidgetItem(size);
        QTableWidgetItem* item2 = new QTableWidgetItem(user);
        QTableWidgetItem* item3 = new QTableWidgetItem(group);
        QTableWidgetItem* item4 = new QTableWidgetItem(date);
        QTableWidgetItem* item5 = new QTableWidgetItem(mode);
        tableWidget->setItem(rowCount,0,item0);
        tableWidget->setItem(rowCount,1,item1);
        tableWidget->setItem(rowCount,2,item2);
        tableWidget->setItem(rowCount,3,item3);
        tableWidget->setItem(rowCount,4,item4);
        tableWidget->setItem(rowCount,5,item5);
    }

    // 释放PASV连接
    delete pasvSocket;
    recv("226");

}

void FtpSocket::send_down(const QString& file)
{
    // 开启二进制模式下载文件
    send("TYPE I\n");
    recv("200");

    // 开启PASV模式
    FtpSocket* pasvSocket = send_pasv();
    if(NULL == pasvSocket)
    {
        qDebug("开启PASV模式失败！");
        return;
    }

    // 告诉服务器要下载的文件
    QString cmd = "RETR " + file + "\n";
    send(cmd);
    recv("150");


    // 创建新文件
    FILE* fw = fopen(file.toStdString().c_str(),"w");
    // 等待数据发送完毕一次性接收
    while(pasvSocket->waitForReadyRead())
    {
        bufArray = pasvSocket->readAll();
        fwrite(bufArray.data(),1,bufArray.size(),fw);
    }

    // 关闭文件并释义PASV连接
    fclose(fw);
    delete pasvSocket;
    recv("226");
}

