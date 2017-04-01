#include "mainwindow.h"
#include <iostream>
#include <QtNetwork>
#include <QtWidgets>
#include <QApplication>
#include <iostream>
#include <QtGui>

#include "serv.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    serv AuRe;
    AuRe.show();

    return a.exec();
}
