#ifndef FTPSOCKET_H
#define FTPSOCKET_H

#include <QTcpSocket>
#include <QTextEdit>
#include <QLineEdit>
#include <QTableWidget>

class FtpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit FtpSocket(QTextEdit* recode = 0):recode(recode) {}
    void send_user(const QString& user);
    void send_pass(const QString& pass);
    void send_cwd(const QString& path);
    void send_pwd(QLineEdit* lineEdit);
    void send_down(const QString& file);
    void send_list(QTableWidget *tableWidget);
    FtpSocket* send_pasv(void);

    bool send(const QString& cmd);
    bool recv(const char* check_status);
    
signals:
    
public slots:

private:
    QTextEdit* recode;
    QByteArray bufArray;
};

#endif // FTPSOCKET_H
