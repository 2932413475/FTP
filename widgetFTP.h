#ifndef WIDGETFTP_H
#define WIDGETFTP_H

#include <QWidget>
#include "ftpsocket.h"

namespace Ui {
class WidgetFTP;
}

class WidgetFTP : public QWidget
{
    Q_OBJECT
    
public:
    explicit WidgetFTP(QWidget *parent = 0);
    ~WidgetFTP();
    
private slots:
    void on_pushButton_connect_clicked();
    void connected_slot();
    void disconnected_slot();

    void on_tableWidget_server_doubleClicked(const QModelIndex &index);

    void on_pushButton_down_clicked();

private:
    Ui::WidgetFTP *ui;
    FtpSocket* ftpSocket;
};

#endif // WIDGETFTP_H
