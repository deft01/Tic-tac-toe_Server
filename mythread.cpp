// mythread.cpp
#include <QDebug>
#include "serv.h"

// We overrides the QThread's run() method here
// run() will be called when a thread starts
// the code will be shared by all threads

MyThread::MyThread(){
}


void MyThread::run()
{

}

void MyThread::produce() {
    // qDebug() << "produce";
    cout << "produce" << endl;
    sleep(1);
    emit produced();
}

void MyThread::initialiseGrille() {
    int i, j;
    for (i=0; i<NB_LIG; i++) {
        for (j=0; j<NB_COL; j++) {
            grille[i][j] = VIDE;
        }
    }
}

void MyThread::afficheGrille() {
    int i, j;
    for (i=0; i<NB_LIG; i++) {
        for (j=0; j<NB_COL; j++) {
            switch (grille[i][j]) {
            case VIDE:
                printf("_ ");

                break;
            case ROND:
                printf("O ");
                break;
            case CROIX:
                printf("X ");
                break;
            }
        }
        printf("\n"); /* fin de la ligne */
    }
}

bool MyThread::metUnPionSurLaGrille(int x, int y) {
    int ligne, col;
    bool saisieCorrecte = FALSE;

        ligne = x; // changer par une fonction qui interroge le client
        col = y;
        printf("\n");

        if ((ligne > 0) && (ligne <= NB_LIG) && (col > 0) && (col <= NB_COL)) {
            ligne--; /* enleve 1 pour etre compatible avec le tableau ayant des
            indices de 0 a NB_LIG-1 */
            col--;
            if (grille[ligne][col] != VIDE) {
                cout  << "Cette case a deja ete remplie. Veuillez recommencer: \n" << endl;
                return false;
            }
            else {
                saisieCorrecte = TRUE;
                grille[ligne][col] = (ValeurGrille)prochainJoueur;
                if (prochainJoueur == ROND) {
                    prochainJoueur = CROIX;
                } else {
                    prochainJoueur = ROND;
                }
            }
        } else {
            cout  << "Indice de ligne ou de colonne incorrect. Veuillez recommencer:\n" << endl;
            return false;
        }
        return true;

}

bool MyThread::testeFinJeu() {
    int i,j;
    int joueurGagnant; /* pour connaitre quel est le gagnant ie soit CROIX soit ROND */
    bool estFini = FALSE;

    /* Teste s'il y a un gagnant */
    /* L'algorithme utilise est le plus facile mais n'est pas le plus efficace
       car on n'utilise pas la position du dernier pion ajoute sur la grille. Cette information
       permettrait de reduire le temps de la recherche.
       De plus, cet algo suppose que la taille de la matrice est de 3 par 3
    */
    /* si la case 1,1 est VIDE, cela signifie que les diagonales, la ligne 1 et la colonne 1 ne sont
       pas gagnantes
    */
    if (grille[1][1] != VIDE) {
        if (/* colonne 1 */ ((grille[0][1] == grille[1][1]) && (grille[1][1] == grille[2][1])) ||
                /* ligne 1 */ ((grille[1][0] == grille[1][1]) && (grille[1][1] == grille[1][2])) ||
                /* diagonale */ ((grille[0][0] == grille[1][1]) && (grille[1][1] == grille[2][2])) ||
                /* autre diag */ ((grille[0][2] == grille[1][1]) && (grille[1][1] == grille[2][0]))) {
            joueurGagnant = grille[1][1]; /* ie ROND ou CROIX */
            estFini = TRUE;
        }
    }

    /* si la case 0,0 est vide, cela signifie que la ligne 0 et le colonne 0 ne sont pas gagnantes */
    if ((!estFini) && (grille[0][0] != VIDE)) {
        if ( /* ligne 0 */ ((grille[0][0] == grille[0][1]) && (grille[0][1] == grille[0][2])) ||
             /* colonne 0*/ ((grille[0][0] == grille[1][0]) && (grille[1][0] == grille[2][0]))) {
            joueurGagnant = grille[0][0];
            estFini = TRUE;
        }
    }

    /* si la case 2,2 est vide, cela signifie que la ligne 2 et la colonne 2 ne sont gagnantes */
    if ((!estFini) && (grille[2][2] != VIDE)) {
        if ( /* ligne 2 */ ((grille[2][0] == grille[2][1]) && (grille[2][1] == grille[2][2])) ||
             /* colonne 2 */ ((grille[0][2] == grille[1][2]) && (grille[1][2] == grille[2][2]))) {
            joueurGagnant = grille[2][2];
            estFini = TRUE;
        }
    }

    if (estFini) {
        cout << "Felicitations au joueur ayant les " << endl;
        if (joueurGagnant == ROND)
            cout << "ronds " << endl;
        else
            printf("croix ");
        cout << "qui a gagne.\n" << endl;
        return TRUE;
    }

    /* teste si la grille n'est pas pleine */
    for (i=0; i<NB_LIG; i++) {
        for (j=0; j<NB_COL; j++) {
            if (grille[i][j] == VIDE) /* Au moins une case est vide donc le jeu n'est pas fini */
                return FALSE;
        }
    }
    return TRUE;

}



