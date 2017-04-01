#ifndef MYTHREAD_H
#define MYTHREAD_H
#include <QThread>
#include <QString>
#include <iostream>
#include <QCoreApplication>
#include <QObject>
#include <QtCore>
#include <stdio.h>
#include <QDebug>
#include <ctime>
#include <stdio.h>
#define TAILLE_MAX 100
#define TAILLE_MINI 3

typedef enum {VIDE, ROND, CROIX} ValeurGrille;
typedef enum {FALSE, TRUE} Boolean;

/* indique quel sera le prochain joueur a mettre un pion dans la grille ie soit ROND soit CROIX */
static int prochainJoueur = ROND;


using namespace std;

class MyThread : public QThread
{
    Q_OBJECT

public:
    // constructor
    // set name using initializer
    MyThread();

    int NB_LIG = 3;
    int NB_COL = 3;
    // overriding the QThread's run() method
    void run();
    bool testeFinJeu();
    bool metUnPionSurLaGrille(int x, int y);
    void afficheGrille();
    void initialiseGrille();

    ValeurGrille grille[3][3]; /* grille du morpion valeurs possibles VIDE, ROND ou CROIX */


public slots:
    void produce();
signals:
    void produced( void );
};

#endif // MYTHREAD_H



