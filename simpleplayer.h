#ifndef SIMPLEPLAYER_H
#define SIMPLEPLAYER_H

#include <QMainWindow>

#include <QMediaPlayer>
#include <QMediaMetaData>
#include <QAudioOutput>
#include <QFileDialog>
#include <QListWidget>
#include <QStyle>

QT_BEGIN_NAMESPACE
namespace Ui { class SimplePlayer; }
QT_END_NAMESPACE

class SimplePlayer : public QMainWindow
{
    Q_OBJECT

public:
    SimplePlayer(QWidget *parent = nullptr);
    ~SimplePlayer();

private slots:
    void actionOpenFile();
    void playButtonClicked();
    void stopButtonClicked();
    void volumeValueChanged(int value);
    void playSelectedSong(QListWidgetItem *item);
    void playNextSong(QMediaPlayer::MediaStatus status);

private:
    Ui::SimplePlayer *ui;

    QMediaPlayer *Player;
    QAudioOutput *AudioOutput;

    int currentSongIndex = 0;

    void listSongs(const QString &folderPath);
    void playSong(const QString &filePath);
    void printMediaMetadata(QMediaPlayer *player);

    // Controls
    void nextButtonClicked();
    void prevButtonClicked();
};
#endif // SIMPLEPLAYER_H
