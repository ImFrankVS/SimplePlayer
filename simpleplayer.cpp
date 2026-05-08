#include "simpleplayer.h"
#include "./ui_simpleplayer.h"

#include <QStandardPaths>

#include <QDir>
#include <QFileInfoList>

SimplePlayer::SimplePlayer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SimplePlayer)
{

    ui->setupUi(this);

    Player = new QMediaPlayer(this);
    AudioOutput = new QAudioOutput(this);
    Player->setAudioOutput(AudioOutput);

    ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay)); // play
    ui->nextButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward)); // next
    ui->prevButton->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward)); // prev
    ui->stopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop)); // stop

    // Controls
    connect(ui->actionOpenFile, &QAction::triggered, this, &SimplePlayer::actionOpenFile);
    connect(ui->playButton, &QPushButton::clicked, this, &SimplePlayer::playButtonClicked);
    connect(ui->stopButton, &QPushButton::clicked, this, &SimplePlayer::stopButtonClicked);
    connect(ui->nextButton, &QPushButton::clicked, this, &SimplePlayer::nextButtonClicked);
    connect(ui->prevButton, &QPushButton::clicked, this, &SimplePlayer::prevButtonClicked);
    connect(ui->volumeSlider, &QSlider::valueChanged, this, &SimplePlayer::volumeValueChanged);

    // List Widget
    connect(ui->listSongs, &QListWidget::itemDoubleClicked, this, &SimplePlayer::playSelectedSong);

    // Player state
    connect(Player, &QMediaPlayer::mediaStatusChanged, this, &SimplePlayer::playNextSong);

    QString musicFolderPath = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
    listSongs(musicFolderPath);
}


SimplePlayer::~SimplePlayer()
{
    delete ui;
}


void SimplePlayer::actionOpenFile()
{
    QString musicLocation = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Select Audio File"), musicLocation, tr("MP3 Files (*.mp3)") );
    Player->setSource(QUrl::fromLocalFile(fileName));

    QFileInfo fileInfo(fileName);
    ui->lblTitleValue->setText(fileInfo.fileName());

    volumeValueChanged(ui->volumeSlider->value());
    Player->play();

    ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));

    printMediaMetadata(Player);
}


void SimplePlayer::playButtonClicked()
{
    if (ui->lblTitleValue->text() == QString("FILE_NAME")) return;

    if (Player->isPlaying()) {
        Player->pause();
        ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    } else {
        Player->play();
        ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    }
}


void SimplePlayer::stopButtonClicked()
{
    Player->stop();
    ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
}


void SimplePlayer::nextButtonClicked()
{
    currentSongIndex++;

    if (currentSongIndex >= ui->listSongs->count()) {
        currentSongIndex = 0;
    }

    playSong(ui->listSongs->item(currentSongIndex)->data(Qt::UserRole).toString());
    ui->lblTitleValue->setText(Player->source().fileName());
}


void SimplePlayer::prevButtonClicked()
{
    currentSongIndex--;

    if (currentSongIndex < 0) {
        currentSongIndex = ui->listSongs->count() - 1;
    }

    playSong(ui->listSongs->item(currentSongIndex)->data(Qt::UserRole).toString());
    ui->lblTitleValue->setText(Player->source().fileName());
}


void SimplePlayer::volumeValueChanged(int value)
{
    ui->lblVolume->setText(QString::number(value));

    qreal linearVolume =
        QAudio::convertVolume(value / qreal(100),
                              QAudio::LogarithmicVolumeScale, QAudio::LinearVolumeScale);

    AudioOutput->setVolume(linearVolume);
}


void SimplePlayer::playSelectedSong(QListWidgetItem *item)
{
    currentSongIndex = ui->listSongs->row(item);
    playSong(item->data(Qt::UserRole).toString());
    ui->playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    ui->lblTitleValue->setText(Player->source().fileName());
}


void SimplePlayer::playNextSong(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia) {
        currentSongIndex++;
        if (currentSongIndex < ui->listSongs->count()) {
            playSong(ui->listSongs->item(currentSongIndex)->data(Qt::UserRole).toString());
            ui->lblTitleValue->setText(Player->source().fileName());
        }
    }
}


void SimplePlayer::listSongs(const QString &folderPath) {
    QDir dir(folderPath);
    QFileInfoList files = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot, QDir::Name);

    for (const QFileInfo &fileInfo : files) {
        if (fileInfo.isDir()) {
            listSongs(fileInfo.absoluteFilePath());  // Llamada recursiva para subcarpetas
        } else if (fileInfo.isFile() && fileInfo.suffix().compare("mp3", Qt::CaseInsensitive) == 0) {
            // qDebug() << fileInfo.fileName();
            QListWidgetItem *item = new QListWidgetItem(fileInfo.fileName(), ui->listSongs);
            item->setData(Qt::UserRole, fileInfo.absoluteFilePath());
        }
    }
}


void SimplePlayer::playSong(const QString &filePath)
{
    Player->setSource(QUrl::fromLocalFile(filePath));
    Player->play();
    volumeValueChanged(ui->volumeSlider->value());
    printMediaMetadata(Player);
}


void SimplePlayer::printMediaMetadata(QMediaPlayer *player)
{
    // QMediaMetaData metadata = player->metaData();

    // qDebug() << metadata.value(QMediaMetaData::Orientation);
    // qDebug() << metadata.value(QMediaMetaData::Resolution);
    // qDebug() << metadata.value(QMediaMetaData::CoverArtImage);
}
