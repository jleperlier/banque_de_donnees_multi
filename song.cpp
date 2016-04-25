#include "song.h"

song::song(QString titre, QString artiste, QString album)
{
    this->titre = titre;
    this->artiste = artiste;
    this->album = album;
}

QString song::getTitre() const{
    return this->titre;
}

QString song::getArtiste() const{
    return this->artiste;
}

QString song::getAlbum() const{
    return this->album;
}

int song::getMatch(){
    return this->nbMatch;
}

QHash<long,double> song::getTable(){
    return this->table;
}

void song::setTitre(QString& titre){
    this->titre = titre;
}

void song::setArtiste(QString& artiste){
     this->artiste = artiste;
}

void song::setAlbum(QString& album){
    this->album = album;
}

void song::setMatch(int m){
    nbMatch = m;
}

void song::setTableValues(const QString& h, const QString& t){
    table.insert(h.toLong(),t.toDouble());
}

