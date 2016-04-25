#ifndef SONG_H
#define SONG_H

#include <QString>
#include <QHash>


class song
{
    public:
        song(): titre(""), artiste(""), album("") {}
        song(QString titre, QString artiste, QString album);

        QString getTitre() const;
        QString getArtiste() const;
        QString getAlbum() const;

        int getMatch();

        QHash<long,double> getTable();

        void setTitre (QString& titre);
        void setArtiste (QString& artiste);
        void setAlbum(QString& album);
        void setMatch(int m);

        void setTableValues(const QString& h, const QString& t);

private:
        QHash<long,double> table;
        QString titre;
        QString artiste;
        QString album;
        int nbMatch;
};

#endif // SONG_H
