#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;
    w.setWindowTitle(QString::fromUtf8("Qt + OpenCV你好 Demo"));
    w.show();
    return app.exec();
}
