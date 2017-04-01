#ifndef STRUCTURE_H
#define STRUCTURE_H

#include <iostream>
#include <QApplication>
#include <QtNetwork>
#include <QtWidgets>
#include <string>
#include <QtGui>
#include <QWidget>
#include <QMovie>
#include <vector>
#include "mythread.h"

struct Partie {
    int tourJoueur;
    bool partieLancee;
    MyThread morpion;
    QList<QTcpSocket *> joueurs;
};

#endif // STRUCTURE_H
