#include <QtGui/QApplication>
#include <QTextCodec>
#include "widgetFTP.h"

int main(int argc, char *argv[])
{
    QTextCodec *codec=QTextCodec::codecForName("utf-8");
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);

    QApplication a(argc, argv);
    WidgetFTP w;
    w.show();
    
    return a.exec();
}
