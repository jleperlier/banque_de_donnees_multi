#include "mainwindow.h"
#include "complex.h"
#include "fft.h"
#include "math.h"
#include "song.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QVector>
#include <QString>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QInputDialog>
#include <QFileDialog>
#include <QFileInfo>
#include <QFileInfoList>

using namespace std;

#define CHUNK_SIZE 4096
#define FUZ_FACTOR 2

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_input(0),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initializeAudio();
    loadDB();
}

MainWindow::~MainWindow()
{
    delete ui;
}

const int MainWindow::RANGE[] = {40,80,120,180};

void MainWindow::initializeAudio()
{

    m_format.setSampleRate(44100); //set frequency to 8000
    m_format.setChannelCount(2); //set channels to mono
    m_format.setSampleSize(16); //set sample sze to 16 bit
    m_format.setSampleType(QAudioFormat::SignedInt ); //Sample type as usigned integer sample
    m_format.setByteOrder(QAudioFormat::BigEndian); //Byte order
    m_format.setCodec("audio/pcm"); //set codec as simple audio/pcm

    QAudioDeviceInfo infoIn(QAudioDeviceInfo::defaultInputDevice());
    if (!infoIn.isFormatSupported(m_format))
    {
        //Default format not supported - trying to use nearest
        m_format = infoIn.nearestFormat(m_format);
    }

}

void MainWindow::loadDB()
{
    QString namedb("db");
    QDir db(namedb);

    db.setFilter(QDir::Files);
    db.setSorting(QDir::Time);

    QFileInfoList list = db.entryInfoList();

    QVector<song> songList;

        for (int i = 0; i < list.size(); i++) {
            QFileInfo fileInfo = list.at(i);

            QFile file(fileInfo.filePath());

            song piece;

            if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
                    QTextStream in(&file);

                    QString line1 = in.readLine();
                    piece.setTitre(line1);

                    QString line2 = in.readLine();
                    piece.setArtiste(line2);

                    QString line3 = in.readLine();
                    piece.setAlbum(line3);

                    while(!in.atEnd()){
                        QString line = in.readLine();
                        QStringList lineList = line.split(':');

                        piece.setTableValues(lineList.at(0),lineList.at(1));
                    }
            }
            else{
                cout << "erreur ouverture fichier en mode lecture" << endl;
            }

            songList.insert(i,piece);

        }

        songs = songList;

        cout << "chargement de la base réussi" << endl;

}

void MainWindow::createAudioInput()
{
    if (m_input != 0) {
        disconnect(m_input, 0, this, 0);
        m_input = 0;
    }

    m_audioInput = new QAudioInput(QAudioDeviceInfo::defaultInputDevice(), m_format, this);
    m_buffer.clear();
    m_buffer.resize(2*m_audioInput->bufferSize());
}

void MainWindow::readMore()
{
    //Return if audio input is null
    if(!m_audioInput)
        return;

    //Check the number of samples in input buffer
    qint64 len = m_audioInput->bytesReady();

    /*Limit sample size
    if(len > CHUNK_SIZE)
        len = CHUNK_SIZE;*/

    //Read sound samples from input device to buffer
    qint64 l = m_input->read(m_buffer.data(), len);
    if(l == len)
    {
        //Assign sound samples to short array
        short* resultingData = (short*)m_buffer.data();

        for (int i=0; i < len/2; i++ )
             {
                 list.append(resultingData[i]);
             }
    }

}

void MainWindow::fourier(){
    int totalSize(list.length());

    amountPossible = totalSize/CHUNK_SIZE;

    for(int times = 0; times < amountPossible; times++){

        QVector<complex> chunk;

        for(int i=0; i<CHUNK_SIZE;i++){
            complex num(list.value(times*CHUNK_SIZE+i),0);
            chunk.insert(i,num);
        }

        if(FFT::Forward(chunk.data(),CHUNK_SIZE))
        {
            fourierRes.insert(times,chunk);
        }
        else
        {
            cout << "erreur FFT" << endl;
        }
    }
}

long MainWindow::hash(long p1, long p2, long p3, long p4)
{
    return  (p4 - (p4 % FUZ_FACTOR)) * 100000000 +
            (p3 - (p3 % FUZ_FACTOR)) * 100000 +
            (p2 - (p2 % FUZ_FACTOR)) * 100 +
            (p1 - (p1 % FUZ_FACTOR));
}

void MainWindow::keyPointsRecord(){
    recordedHash.clear();
    QVector<short> recordPoints(4);
    QVector<short> highScores(4);

    for(int times=0; times<amountPossible; times++){
        recordPoints.clear();
        highScores.clear();

        for(int freq=30; freq<400; freq++){
            short mag = log(fourierRes.at(times).at(freq).norm()+1);

                int index = getIndex(mag);

                if(mag > highScores.value(index)){
                    highScores.insert(index,mag);
                    recordPoints.insert(index,freq);
                }

        }

        long h = hash(recordPoints.value(0), recordPoints.value(1), recordPoints.value(2), recordPoints.value(3));
        double t = (double)times / 10;

        recordedHash.insert(h,t);
    }

    /*QHash<long,double>::iterator it;
    for(it=recordedHash.begin(); it != recordedHash.end(); it++)
        cout << it.key() << " : " << it.value() << endl;*/

}

void MainWindow::keyPointsAdd(){
    addHash.clear();
    QVector<short> recordPoints(4);
    QVector<short> highScores(4);

    for(int times=0; times<amountPossible; times++)
    {
        recordPoints.clear();
        highScores.clear();

        for(int freq=30; freq<400; freq++){
            short mag = log(fourierRes.at(times).at(freq).norm()+1);

                int index = getIndex(mag);

                if(mag > highScores.value(index)){
                    highScores.insert(index,mag);
                    recordPoints.insert(index,freq);
                }

        }

        long h = hash(recordPoints.value(0), recordPoints.value(1), recordPoints.value(2), recordPoints.value(3));
        double t = (double)times / 10;

        addHash.insert(h,t);
    }
}

void MainWindow::matching(){
    for(int song=0; song<songs.size(); song++){

        QVector<double> timeRec;
        QVector<double> timeSong;
        int match(0);

        QHash<long,double>::iterator it;

        for(it=recordedHash.begin(); it != recordedHash.end(); it++){
            if(songs.value(song).getTable().contains(it.key())){
                    timeRec.append(it.value());
                    timeSong.append(songs.value(song).getTable().value(it.key()));
            }
        }

        for(int i=0; i<timeRec.size()-1; i++){
            int dRec = abs((int)timeRec.value(i+1) - (int)timeRec.value(i));
            int dSong = abs((int)timeSong.value(i+1) - (int)timeSong.value(i));

            if(abs(dRec-dSong)<=5) match++;

        }

        matchTab.insert(song,match);
        //songs.value(song).setMatch(match);

    }
}

void MainWindow::on_stoprecord_clicked()
{
    m_audioInput->stop();
    //m_input->close();

    fourier();

    keyPointsRecord();

    matching();

    //Printing results :

    QMap<int,song> results;

    for(int res=0; res<songs.size(); res++){
        results.insert(matchTab.value(res),songs.value(res));
    }

    QMap<int,song>::const_iterator iter = results.constBegin();
    while(iter != results.constEnd()){
        cout << iter.value().getTitre().toStdString() << " de " << iter.value().getArtiste().toStdString() << ". matches = " << iter.key() << endl;
        iter++;
    }

}

int MainWindow::getIndex(short freq){
    int i = 0;

    while(RANGE[i] < freq) {
        i++;
    }
            return i;
}

void MainWindow::on_record_clicked()
{
    createAudioInput();
    //Audio input device
    m_input = m_audioInput->start();
    //connect readyRead signal to readMre slot.
    //Call readmore when audio samples fill in inputbuffer
    connect(m_input, SIGNAL(readyRead()), SLOT(readMore()));

}

void MainWindow::on_add_clicked()
{
    m_buffer.clear();
    list.clear();
    bool ok;

    QString title = QInputDialog::getText(this, tr("Titre"),
                                         tr("Entrez le titre :"), QLineEdit::Normal,
                                             QDir::home().dirName(), &ok);
    QString artist = QInputDialog::getText(this, tr("Artiste"),
                                         tr("Entrez l'artiste :"), QLineEdit::Normal,
                                             QDir::home().dirName(), &ok);
    QString album = QInputDialog::getText(this, tr("Album"),
                                         tr("Entrez l'album :"), QLineEdit::Normal,
                                             QDir::home().dirName(), &ok);

    QString path = QFileDialog::getOpenFileName(this, "Ouvrir un fichier .mp3 ...");
    QFile song(path);

    if(song.open(QIODevice::ReadOnly)){
        m_buffer = song.readAll();
    }
    else{
        cout << "erreur à l'ouverture du fichier mp3" << endl;
    }


    short* resultingData = (short*)m_buffer.data();

    for (int i=0; i < m_buffer.size(); i++ )
         {
             list.append(resultingData[i]);
         }

    fourier();

    keyPointsAdd();

    QFile file("db/" + title + ".txt");
    file.open(QIODevice::Append | QIODevice::Text);
    QTextStream out(&file);


        out << title << endl;
        out << artist << endl;
        out << album << endl;

        QHash<long,double>::iterator it;
        for(it=addHash.begin(); it != addHash.end(); it++)
                out << it.key() << " : " << it.value() << endl;

}
