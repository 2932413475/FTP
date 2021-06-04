#include "widgetFTP.h"
#include "ui_widgetFTP.h"
#include <QHostAddress>

WidgetFTP::WidgetFTP(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetFTP)
{
    ui->setupUi(this);
    ftpSocket = new FtpSocket(ui->textEdit_record);
}

WidgetFTP::~WidgetFTP()
{
    delete ui;
}

void WidgetFTP::on_pushButton_connect_clicked()
{
    ftpSocket->connectToHost(QHostAddress(ui->lineEdit_ip->text()),ui->lineEdit_port->text().toShort());
    connect(ftpSocket,SIGNAL(connected()),this,SLOT(connected_slot()));
    connect(ftpSocket,SIGNAL(disconnected()),this,SLOT(disconnected_slot()));
}

void WidgetFTP::connected_slot()
{
    ui->pushButton_connect->setText("已连接");
    ui->pushButton_connect->setEnabled(false);

    if(ftpSocket->recv("220"))
    {

        ftpSocket->send_user(ui->lineEdit_user->text());
        ftpSocket->send_pass(ui->lineEdit_pass->text());
        ftpSocket->send_pwd(ui->lineEdit_server_path);
        ftpSocket->send_list(ui->tableWidget_server);
    }
}

void WidgetFTP::disconnected_slot()
{
    ui->pushButton_connect->setText("连接");
    ui->pushButton_connect->setEnabled(true);
}


void WidgetFTP::on_tableWidget_server_doubleClicked(const QModelIndex &index)
{
    if('d'==ui->tableWidget_server->item(index.row(),5)->text().at(0))
    {
        ftpSocket->send_cwd(ui->tableWidget_server->item(index.row(),0)->text());
        ftpSocket->send_pwd(ui->lineEdit_server_path);
        ftpSocket->send_list(ui->tableWidget_server);
    }

}

void WidgetFTP::on_pushButton_down_clicked()
{
    int row = ui->tableWidget_server->currentRow();
    if('d' != ui->tableWidget_server->item(row,5)->text().at(0))
    {
       ftpSocket->send_down(ui->tableWidget_server->item(row,0)->text());
       // 刷新 tableWidget_local 的内容
    }
}
