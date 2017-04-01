#include "mythread.h"
#include "serv.h"

serv::serv()
{
    setupUi(this);
    serveur = new QTcpServer;

    QMovie *movie = new QMovie("chargement3.gif");
    label->setMovie(movie);
    movie->start();

    QString key = "***"; // maniere d authentification temporaire
    QString keyStuxnet = "!!!"; // maniere d auth temporaire
    stuxnetMdp.push_back(keyStuxnet);
    mdp.push_back(key);

    texte->append("test!!!");


    if(!serveur->listen(QHostAddress::Any,8836)){
        cout << "Le server ne peut pas démarrer " << endl;
    }else{
        texte->append("J'ai bien démarré");
        QObject::connect(serveur,SIGNAL(newConnection()),this,SLOT(connectionClient()));
    }
    texte->append("WTF!!!");
    taille = 0;

}

void serv::deconnnectionClient() //a revoir
{
    removePlaying(qobject_cast<QTcpSocket *>(sender()));
    //envoyerAtous("un client vient de se deconnecter");
    // envoyerCRC("Un client vient de se déconnecter",0);
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if(socket == 0)
        return;
    /*
    if(verifySocketRoot(socket)==true){
        tabCRC.removeOne(socket);
    }
    if(verifySocketRoot(socket)==true){
        tabStuxnet.removeOne(socket);
    }
    if(verifySocketRoot(socket)==true){
        tabUnknow.removeOne(socket);
    }
*/
    socket->deleteLater();
}

void serv::connectionClient() // a revoir
{
    QTcpSocket *socket = serveur->nextPendingConnection();
    texte->append("un client INCONNU vient de se connecter");
    tabUnknow<<socket;

    QObject::connect(socket,SIGNAL(disconnected()),this,SLOT(deconnnectionClient()));
    QObject::connect(socket,SIGNAL(readyRead()),this,SLOT(recevoirDonnee()));
}

/**
void serv::envoyerAtous(const QString &message) //CRC + Stuxnet mais pas les unknow
{
    QByteArray paquet;
    QDataStream out(&paquet,QIODevice::WriteOnly);

    out<<(quint16)0;
    out<<message;
    out.device()->seek(0);
    out<<(quint16)(paquet.size()-sizeof(quint16));

//    for(int i=0;i<tabCRC.size();i++){
//        tabCRC[i]->write(paquet);
//    }
//    for(int i=0;i<tabStuxnet.size();i++){
//        tabStuxnet[i]->write(paquet);
//    }
    texte->append("envoyé -TOUS- "+message);
}
**/

bool serv::ajouterAPartie(int a, QTcpSocket *soqet){
    if(games[a]->joueurs.length() > 1) {
        envoyerAuJoueur("\nTrop de joueur dans cette partie pour quelle puisse être rejoint", soqet);
        return false;
    }

    removeUnknow(soqet);
    removePlaying(soqet);
    games[a]->joueurs.push_back(soqet);

    return true;

}

bool serv::verifyPlayingGameNum(int gameNumber, QTcpSocket* soqet) {
    for(int i=0;i<games.size();i++){
        for (int var = 0; var < games[i]->joueurs.size(); ++var) {
            if(games[i]->joueurs[var]==soqet && var == gameNumber) {
                return true;
            }
        }
    }
    return false;
}

bool serv::verifyPlaying( QTcpSocket *soqet ) {
    for(int i=0;i<games.size();i++) {
        for (int var = 0; var < games[i]->joueurs.size(); ++var) {
            if(games[i]->joueurs[var]==soqet) {
                return true;
            }
        }
    }
    return false;
}

bool serv::verifyTourJoueur(int gameNum, QTcpSocket *soqet) {
    for (int var = 1; var <= games[gameNum]->joueurs.length(); ++var) {
        if (games[gameNum]->joueurs[var - 1] == soqet) {
            if (var == games[gameNum]->tourJoueur) {
                cout << "C'est bien au tour de ce joueur de jouer" << endl;
                return true;
            } else {
                envoyerAuJoueur("\nCe n'est pas à votre tour de jouer" ,soqet);
                return false;
            }
        }
    }
    return false;
}

bool serv::removeUnknow(QTcpSocket *soqet) {
    if(verifyPlaying(soqet)==true) {
        tabUnknow.removeOne(soqet);
        return true;
    }
    return false;
}

bool serv::removePlaying(QTcpSocket *soqet) {
    for(int i=0;i<games.length();i++){
        for (int var = 0; var < games[i]->joueurs.length(); ++var) {
            if(games[i]->joueurs[var] == soqet) {
                games[i]->joueurs.removeOne(soqet);
                return true;
            }
        }
    }
    return false;
}

// même fonction que removePlaying mais on précise la partie pour gagner en performance
bool serv::removePlaying(int numPartie, QTcpSocket *soqet) {

}

void serv::creerNouvellePartie(QTcpSocket *soqet){
    games.push_back(new Partie);
    games.last()->morpion.initialiseGrille();
    games.last()->joueurs.push_back(soqet);
    // on enleve le joueur de la table Unknow
    if(removeUnknow(soqet) == true) {
        cout << "Supprimé de la table Unknow" << endl;
    } else {
        cout << "Fail de suppr de la table Unknow" << endl;
    }

    envoyerAuJoueur("\nVous venez de vous connecter à la partie numero " + QString::number(games.length() - 1), soqet);
    // QThread::sleep(1);
    if(games.last()->joueurs.length() > 1) {
        envoyerAuJoueur("\nVous êtes actuellement " + QString::number(games.last()->joueurs.length()) + " joueurs dans la partie", soqet);
    } else {
        envoyerAuJoueur("\nVous êtes actuellement le seul joueur dans la partie :'(",soqet);
        // QThread::sleep(1);
        envoyerAuJoueur("\nQuand quelqu'un rejoindra vous pourez lui proposer de jouer avec la commande /start",soqet);
    }
    // QThread::sleep(1);
    envoyerAuJoueur("\n\nVoici les rêgles du jeu : " + rules, soqet);
}

void serv::envoyerAuJoueur(const QString &message, QTcpSocket *soqet){
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);

    out<<(quint16)0;
    out<<message;
    out.device()->seek(0);
    out<<(quint16)(data.size() - sizeof(quint16));

    soqet->write(data);

    texte->append("envoye à un client : " + message);
}

void serv::envoyerAuxJoueurs(const QString &message, int gameNum) {
    QByteArray paquet;
    QDataStream out(&paquet,QIODevice::WriteOnly);

    out<<(quint16)0;
    out<<message;
    out.device()->seek(0);
    out<<(quint16)(paquet.size()-sizeof(quint16));

    //    for(int i=0;i<tabCRC.size();i++){
    //        tabCRC[i]->write(paquet);
    //    }
    //    for(int i=0;i<tabStuxnet.size();i++){
    //        tabStuxnet[i]->write(paquet);
    //    }

    for (int var = 0; var < games[gameNum]->joueurs.length(); ++var) {
        games[gameNum]->joueurs[var]->write(paquet);
    }

    texte->append("envoyé -TOUS- "+message);
}

/*
bool serv::verifyTourJoueur(QTcpSocket *soqet) {
    for(int i=0;i<games.length();i++){
        for (int var = 0; var < games[i]->joueurs.length(); ++var) {
            if(games[i]->joueurs[var] == soqet && var == games[i]->tourJoueur) {
                return true;
            }
        }
    }
    return false;
}
*/

int serv::getPartieJoueur (QTcpSocket *soqet) {
    if(verifyPlaying(soqet) == false) {
        envoyerAuJoueur("\nVous n'êtes pas dans une partie.",soqet);
        return -1;
    }
    for(int i=0;i<games.size();i++){
        for (int var = 0; var < games[i]->joueurs.size(); ++var) {
            if(games[i]->joueurs[var] == soqet) {
                return i;
            }
        }
    }
    return -2;
}

// fonction permettant le lancement d'une partie, j'ai appellé la fonction lancer Thread plutot que lancerPartie pour leur
// donner un nom unique et identique à toutes les fonction du même genre
void serv::lancerThread(int numPartie, QTcpSocket *soqet) {
    // la grille à déjà été initialisée lors de la création de la partie
    // games[numPartie]->morpion.initialiseGrille();

    // on détermine au hazard qui va commencer à jouer en premier
    /*srand (time(NULL));
    int random = rand()%games[numPartie]->joueurs.length();
    games[numPartie]->tourJoueur = random;
    */
    games[numPartie]->tourJoueur = 1;
    games[numPartie]->partieLancee = true;
}

void serv::finirThread(int numPartie, QTcpSocket *soqet) {
    games[numPartie]->partieLancee = false;
    games[numPartie]->tourJoueur = NULL;
    games[numPartie]->morpion.initialiseGrille();
    envoyerAuJoueur("\nLa partie s'est terminée, vous pouvez demander une nouvelle partie à l'adversaire avec /play" ,soqet);
}

void serv::tourThread(int x, int y, int numPartie, QTcpSocket *soqet) {
    // A FAIRE vérifier à chaques fois que les variables impactant des boucles ou des fonctions soient bien définies
    // A FAIRE vérifier dans toutes les fonctions de jeu de morpion que le joueur n'est pas seul
    // (possibilité qu'un adversaire quitte en plein milieu de partie)

    // on vérifie en premier que la partie est bien lancée
    if(games[numPartie]->partieLancee == false) {
        cout << "Erreur, la partie n'est pas lancée -> tourThread" << endl;
        return;
    }

    // on vérifie que c'est bien au tour du joueur qui lance la commande de jouer
    if(verifyTourJoueur(numPartie, soqet) == false) {
        return;
    }

    if(checkWinThread(numPartie,soqet) == true){
        cout << "Erreur, la partie est gagnée par un joueur -> tourThread" << endl;
        return;
    }

    if(games[numPartie]->morpion.metUnPionSurLaGrille(x,y) == false ) {
        envoyerAuJoueur("\nLa fonction de placement n'a pas aboutie, verifiez vos coordonnées" ,soqet);
    } else {
        cout << "tour JOUEUR = " << games[numPartie]->tourJoueur << endl;
        envoyerAuJoueur("tour JOUEUR = " + QString::number(games[numPartie]->tourJoueur), soqet);
        // on passe au tour du joueur suivant
        /*if (games[numPartie]->joueurs.length() > games[numPartie]->tourJoueur) {
            games[numPartie]->tourJoueur++;
        } else if (games[numPartie]->tourJoueur != 1){
            games[numPartie]->tourJoueur = 1;
        } else {
            cout << "Erreur, impossible de trouver à qui c'est le tour de jouer -> tourThread" << endl;
        }*/

        if (games[numPartie]->tourJoueur  == 1) {
            games[numPartie]->tourJoueur = 2;
        } else if (games[numPartie]->tourJoueur == 2){
            games[numPartie]->tourJoueur = 1;
        } else {
            cout << "Erreur, impossible de trouver à qui c'est le tour de jouer -> tourThread" << endl;
        }

    }

    checkWinThread(numPartie,soqet);

}

bool serv::checkWinThread(int numPartie, QTcpSocket *soqet) {
    if(games[numPartie]->morpion.testeFinJeu() == true) {
        envoyerAuJoueur("\nVictoire du joueur" + QString::number(games[numPartie]->tourJoueur),soqet);
        finirThread(numPartie,soqet);
        return true;
    } else {
        cout << "Pas de gagnant ce round" << endl;
        return false;
    }
    cout << "Erreur fonction checkWinThread" << endl;
    return false;
}

void serv::afficheGrille(int gameNum) {

    int i, j;
    for (i=0; i<games[gameNum]->morpion.NB_LIG; i++) {
        for (j=0; j<games[gameNum]->morpion.NB_COL; j++) {
            switch (games[gameNum]->morpion.grille[i][j]) {
            case VIDE:
                printf("_ ");
                envoyerAuxJoueurs("_ ", gameNum);
                break;
            case ROND:
                printf("O ");
                envoyerAuxJoueurs("O ", gameNum);
                break;
            case CROIX:
                printf("X ");
                envoyerAuxJoueurs("X ", gameNum);
                break;
            }
        }
        printf("\n"); /* fin de la ligne */
        envoyerAuxJoueurs("\n", gameNum);
    }
}

void serv::recevoirDonnee() // a revoir
{
    QTcpSocket *sockket = qobject_cast<QTcpSocket *>(sender());
    if(sockket==0)
        return;

    QDataStream in(sockket);

    if(taille==0){
        if(sockket->bytesAvailable()<(int)sizeof(quint16))
            return;

        in>>taille;
    }

    if(sockket->bytesAvailable()<taille)
        return;

    QString dataa;
    in>>dataa;
    texte->append(dataa);

    // LES COMMANDES

    // /play [number],[number]
    if(dataa.contains("/play")){
        int numGame = 0;
        numGame = getPartieJoueur(sockket);

        if(verifyTourJoueur(numGame, sockket) == false) {
            envoyerAuJoueur("\nVous pouvez réclamer un timeout un utilisant la commande /timeout",sockket);
        }

        if(numGame < 0 ){
            cout << "erreur dans /play -> get partie du joueur" << endl;
            if(numGame = -2){
                envoyerAuJoueur("\nPas de partie trouvée",sockket);
            }
            return;
        }

        if(games[numGame]->partieLancee == false){
            envoyerAuJoueur("\nVotre partie n'est pas lancée, vous ne pouvez pas jouer",sockket);
        }

        //        if(games[numGame]->joueurs.length() < 2) {
        //            envoyerAuJoueur("\nIl n'y a pas assez de joueurs dans votre partie pour la lancer ( 2 minimum ).",sockket);
        //        }

        // regarder dans le tableu de thread si il y a une partie à vector[game number]
        texte->append("START DIGIT : " + dataa.mid(6,dataa.size() - 6));

        int diigit = NULL;
        int diigit2 = NULL;
        int cursor = 0;
        for (int var = 0; var < dataa.length(); ++var) {
            // bool ok;
            if (dataa.at(var).isDigit()){
                if (cursor == 0) {
                    diigit = concatenate(diigit,dataa.at(var).digitValue());
                } else if (cursor != 0){
                    diigit2 = concatenate(diigit2,dataa.at(var).digitValue());
                }
            }
            if(dataa.at(var) == ","){
                cursor++;
                cout << "is ponct" << endl;
            }
        }

        cout << "DIGIT : " << diigit << endl;
        cout << "DIGIT2 : " << diigit2 << endl;
        texte->append("Digit : " + QString::number(diigit));
        texte->append("Digit2 : " + QString::number(diigit2));

        tourThread(diigit,diigit2,numGame,sockket);

        afficheGrille(numGame);
    }

    // /start
    if(dataa.contains("/start")){
        int numGame = 0;
        numGame = getPartieJoueur(sockket);
        envoyerAuJoueur("\nVous pouvez réclamer un timeout un utilisant la commande /timeout",sockket);

        if(numGame < 0 ){
            cout << "erreur dans /play -> get partie du joueur" << endl;
            if(numGame = -2){
                envoyerAuJoueur("\nPas de partie trouvée",sockket);
            }
            return;
        }

        if(games[numGame]->joueurs.length() < 2) {
            envoyerAuJoueur("\nIl n'y a pas assez de joueurs dans votre partie pour la lancer ( 2 minimum ).",sockket);
            return;
        }

        lancerThread(numGame,sockket);

        afficheGrille(numGame);

    }

    // /timeout permet au joueur adverse de celui en train de joueur actuellement de réclamer un de son tour en cour
    if(dataa.contains("/timeout")){
        texte->append("CA MARCHE");

    }

    // /status
    if(dataa.contains("/status")){
        for (int var = 0; var < games.length(); ++var) {
            envoyerAuJoueur("\nPartie numéro " + QString::number(var) ,sockket);
            for (int varr = 0; varr < games[var]->joueurs.length(); ++varr) {
                envoyerAuJoueur("\n    Joueur numéro " + QString::number(varr) ,sockket);
                // games[var]->joueurs[varr];
            }
        }
    }

    // /again
    if(dataa.contains("/again")){
        texte->append("CA MARCHE");

    }


    // /accept
    if(dataa.contains("/accept")){
        texte->append("CA MARCHE");

    }

    // /refuse
    if(dataa.contains("/refuse")){
        texte->append("CA MARCHE");

    }

    // /join [game number]
    if(dataa.contains("/join")){
        // regarder dans le tableu de thread si il y a une partie à vector[game number]
        texte->append("MID : " + dataa.mid(6,dataa.size() - 6));

        int diigit = NULL;

        for (int var = 0; var < dataa.length(); ++var) {
            // bool ok;
            if (dataa.at(var).isDigit()){
                diigit = concatenate(diigit,dataa.at(var).digitValue());
            }
        }

        cout << "DIGIT : " << diigit << endl;
        texte->append("Digit : " + QString::number(diigit));


        if(games.length() >= diigit && diigit > -1) {
            cout << "coucou" << endl;
            if(verifyPlayingGameNum(diigit,sockket) == true) {
                envoyerAuJoueur("\nVous êtes déjà dans cette partie, choisissez en une autre", sockket);
                return;
            }
            cout << "coucou2" << endl;
            if(games[diigit]->joueurs.size() < 2) {
                // envoyer au client qu'il a bien été connecté
                texte->append("Un client s'est connecté à une partie");
                envoyerAuJoueur("\nVous vous êtes connecté à la partie numéro " + QString::number(diigit),sockket);
                if(ajouterAPartie(diigit,sockket) == false) {

                }
                cout << "coucou3" << endl;
            } else {
                // envoyer au client que la partie est pleine ou n'existe pas
                cout << "xD" << endl;
                texte->append("Un client n'a pas pu se connecter à une partie");
                envoyerAuJoueur("\nLa connection à la partie numéro " + QString::number(diigit) + " est impossible.",sockket);
                envoyerAuJoueur("\nPeut être est elle déjà pleine ou n'a pas pu être trouvée.",sockket);
            }
        }
        cout << "pas de if" << endl;

    }

    // /create
    if(dataa.contains("/create")){
        // creer un nouveau thread dans le vector
        if(verifyPlaying(sockket) == true) {
            envoyerAuJoueur("\nVous êtes actuellement dans une partie, veuillez en sortir -> /leave",sockket);
            return;
        }
        creerNouvellePartie(sockket);
    }

    // /create
    if(dataa.contains("/leave")){
        if(verifyPlaying(sockket)==false) {
            envoyerAuJoueur("\nVous n'êtes pas dans une partie, vous ne pouvez effectuer cette action.",sockket);
            return;
        }
        if(removePlaying(sockket) == true ) {
            envoyerAuJoueur("\nVous venez de quitter la partie en cour.",sockket);
        } else {
            envoyerAuJoueur("\nLa commande /leave n'a pas fonctionné.",sockket);
            return;
        }
    }

    // /display
    if(dataa.contains("/display")){
        // appeler la fonction affiche du morpion pour la partie à laquelle le joueur est relier
        // faire un tableau de liaison joueur / partie
        int numGame = getPartieJoueur(sockket);
        afficheGrille(numGame);

    }

    // /connect [server ip]:[server port]
    if(dataa.contains("/connect")){
        // fonction inutile à prioris

    }

    // /disconnect    /exit    /quit
    if(dataa.contains("/disconnect") || dataa.contains("/exit") || dataa.contains("/quit")){
        // à faire en dernier si vraiment ca en vaut la peine
    }

    // /score [player number]   /score
    if(dataa.contains("/score") || dataa.contains("/score")){
        // affiche dans le tableau général qui a le plus de scores
        // peut etre faire une BDD ou un fichier de score si vraiment ca en vaut la peine
    }

    // /game
    if(dataa.contains("/game")){
        texte->append("CA MARCHE");
        // envoyer la fonction affiche du morpion au client
    }

    // /rules
    if(dataa.contains("/rules")){
        // envoie au client la chaine de caractere contenant les rules
        envoyerAuJoueur("\nVoici les rêgles du jeu : \n" + rules, sockket);

    }

    if(dataa==mdp){
        /*
        if(verifySocketRoot(sockket)==false){
            tabCRC.push_back(sockket);
            envoyerCRC("Une demande d'auth pour CRC a été ACCEPTEE",0);
            texte->append("Une demande d'auth pour CRC a été ACCEPTEE");
            if(verifySocketUnknow(sockket)==true)
                tabUnknow.removeOne(sockket);
            if(verifySocketStuxnet(sockket)==true)
                tabStuxnet.removeOne(sockket);
        }else{
            envoyerCRC("Vous etes deja un CRC",0);
            texte->append("Vous etes deja un CRC");
        }
        */
    }

    if(dataa==stuxnetMdp){
        /*
        if(verifySocketStuxnet(sockket)==false){
            tabStuxnet.push_back(sockket);
            envoyerCRC("Une demande d'auth pour STUXNET a été ACCEPTEE",0);
            texte->append("Une demande d'auth pour STUXNET a été ACCEPTEE");
            if(verifySocketUnknow(sockket)==true)
                tabUnknow.removeOne(sockket);
            if(verifySocketRoot(sockket)==true)
                tabCRC.removeOne(sockket);
        }else{
            envoyerAtous("Vous etes deja un STUXNET");
            texte->append("Vous etes deja un STUXNET");
        }
        */
    }

    /*
    if(verifySocketRoot(sockket)==true){
        envoyerAtous(dataa);
    }else if(verifySocketStuxnet(sockket)==true){
        envoyerCRC(dataa,0);
    }else if(verifySocketUnknow(sockket)==true){
        envoyerCRC("Un client UNKNOW a tenter d'envoyer ces données sur le réseau -> ",0);
        envoyerCRC(dataa,0);
    }
    */

    taille=0;
}

bool serv::verifySocketUnknow(QTcpSocket *soqet)
{
    for(int i=0;i<tabUnknow.size();i++){
        if(tabUnknow[i]==soqet)
            return true;
    }
    return false;
}

/*
bool serv::verifySocketStuxnet(QTcpSocket *soqqet)
{
    for(int i=0;i<tabStuxnet.size();i++){
        if(tabStuxnet[i]==soqqet)
            return true;
    }
    return false;
}

bool serv::verifySocketRoot(QTcpSocket *sockket)
{
    for(int i=0;i<tabCRC.size();i++){
        if(tabCRC[i]==sockket)
            return true;
    }
    return false;
}
*/

unsigned serv::concatenate(unsigned x, unsigned y) {
    unsigned pow = 10;
    while(y >= pow)
        pow *= 10;
    return x * pow + y;
}

//void serv::envoyerCRC(const QString &message) //seulement CRC
//{
//    QByteArray data;
//    QDataStream out(&data, QIODevice::WriteOnly);

//    out<<(quint16)0;
//    out<<message;
//    out.device()->seek(0);
//    out<<(quint16)(data.size() - sizeof(quint16));

//    for(int i=0;i<tabCRC.size();i++){
//        tabCRC[i]->write(data);
//    }
//    texte->append("envoyé -CRC- " + message);
//}

/*
void serv::envoyerCRC(const QString &message, int n_args,...) //seulement CRC
{
    QTcpSocket *soquett = new QTcpSocket();
    va_list ap;
    va_start(ap, n_args);
    for(int i = 0; i < n_args; i ++)
        soquett = va_arg(ap,QTcpSocket *);

    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);

    out<<(quint16)0;
    out<<message;
    out.device()->seek(0);
    out<<(quint16)(data.size() - sizeof(quint16));

    for(int i=0;i<tabCRC.size();i++){
        if(tabCRC[i]!=soquett || n_args == 0)
            tabCRC[i]->write(data);
    }
    texte->append("envoyé -CRC- " + message);
}


void serv::envoyerStuxnet(const QString &message) // seulement Stuxnet
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);

    out<<(quint16)0;
    out<<message;
    out.device()->seek(0);
    out<<(quint16)(data.size() - sizeof(quint16));

    for(int i=0;i<tabStuxnet.size();i++){
        tabStuxnet[i]->write(data);
    }
    texte->append("envoyé -Stuxnet- " + message);
}
*/
