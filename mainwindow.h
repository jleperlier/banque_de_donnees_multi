#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "song.h"
#include "complex.h"
#include <QMainWindow>
#include <QByteArray>
#include <QAudioInput>
#include <QAudioOutput>
#include <QMediaPlayer>
#include <QBuffer>
#include <QList>
#include <QHash>

using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    static int getIndex(short freq);
    const static int RANGE[4];

private:
    void initializeAudio();
    void loadDB();
    void matching();
    void createAudioInput();
    void fourier();
    void keyPointsRecord();
    void keyPointsAdd();
    long hash(long p1, long p2, long p3, long p4);

private slots:
    void readMore();
    void on_record_clicked();
    void on_stoprecord_clicked();
    void on_add_clicked();

private:
    Ui::MainWindow *ui;
    QMediaPlayer player;
    QAudioDeviceInfo m_Inputdevice;
    QAudioFormat m_format;
    QAudioInput *m_audioInput;
    QIODevice *m_input;
    QByteArray m_buffer;
    QList<short> list;
    int amountPossible;
    QVector<int> matchTab;
    QVector< QVector<complex> > fourierRes;
    QHash<long,double> recordedHash;
    QHash<long,double> addHash;
    QVector<song> songs;
};

#endif // MAINWINDOW_H
