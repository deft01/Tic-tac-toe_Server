#ifndef SERV_H
#define SERV_H

#include <iostream>
#include <QApplication>
#include <QtNetwork>
#include <QtWidgets>
#include <string>
#include <QtGui>
#include <QWidget>
#include <QMovie>
#include <vector>
#include <stdarg.h>
#include <QString>
#include <QThread>
#include <ctime>

#define calculs "76 69 63 74 6f 72 20 70 6c 6f 75 68 69 6e 65 63"

#include "math.h"
#include "mythread.h"
#include "structure.h"
#include "ui_mainwindow.h"

using namespace std;

class serv : public QMainWindow, private Ui::MainWindow {
    Q_OBJECT
public:
    serv();
    QList<Partie *> games;
    const QString rules = "ATTENTION CECI EST UNE REGLE A RESPECTER";


private slots:
    void recevoirDonnee();
    void deconnnectionClient();
    void connectionClient();

private:
    // void envoyerAtous(const QString &message);
    // void envoyerCRC(const QString &message, int n_args, ...);
    // void envoyerStuxnet(const QString &message);
    void envoyerAuxJoueurs(const QString &message, int);
    void envoyerAuJoueur(const QString &message, int);
    void envoyerAuJoueur(const QString &message, QTcpSocket *);

    void creerNouvellePartie(QTcpSocket *);
    bool ajouterAPartie(int a, QTcpSocket *);
    void ajouterAUnknow(int a, QTcpSocket *);

    bool removeUnknow(QTcpSocket *soqet);
    bool removePlaying(QTcpSocket *soqet);
    bool removePlaying(int numPartie, QTcpSocket *soqet); // A FAIRE

    int getPartieJoueur (QTcpSocket *soqet);
    void afficheGrille(int gameNum);

    // bool verifySocketStuxnet(QTcpSocket *);
    // bool verifySocketRoot(QTcpSocket *);
    bool verifySocketUnknow(QTcpSocket *);
    bool verifyPlayingGameNum(int gameNUmber, QTcpSocket* soqet);
    bool verifyPlaying(QTcpSocket *);    
    bool verifyTourJoueur(int gameNum, QTcpSocket *);

    void lancerThread(int numPartie, QTcpSocket *soqet);
    void finirThread(int numPartie, QTcpSocket *soqet);
    void tourThread(int x, int y, int numPartie, QTcpSocket *soqet);
    int checkWinThread(int numPartie, QTcpSocket *soqet);

    unsigned concatenate(unsigned x, unsigned y) ;

    QString mdp;
    QString stuxnetMdp;
    QTcpServer *serveur;


    QList<QTcpSocket *> tabUnknow;
    // QList<QTcpSocket *> tabCRC;
    // QList<QTcpSocket *> tabStuxnet;
    quint16 taille;
};

#endif // SERV_H
