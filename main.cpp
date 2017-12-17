#include "mainwindow.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QSettings>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;

    w.move((a.desktop()->width() - w.width()) / 2,
           (a.desktop()->height() - w.height()) / 2);
    w.show();

    return a.exec();
}
