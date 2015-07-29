#include "maincontroller.h"
#include "treemodel.h"
#include "listmodel.h"
#include "downloadmanager.h"
#include "mediafile.h"
#include "videofile.h"
#include "categorynode.h"

#include <QRegExp>
#include <QDesktopServices>
#include <QTimer>
#include <QDebug>

MainController::MainController(QObject *parent)
    : QObject(parent),
      _treeModel(0),
      _listModel(0),
      _fileManager(0)
{
    _fileManager = new DownloadManager(this);
    _treeModel = new TreeModel(this);
    _listModel = new ListModel(this);
    _timer = new QTimer(this);

    connect(_fileManager, &DownloadManager::downloadProgress, this, &MainController::downloadProgress);
    connect(_fileManager, &DownloadManager::downloadFinished, this, &MainController::downloadFinished);
    connect(_timer, &QTimer::timeout, this, &MainController::updateTree);
}

void MainController::loadData()
{
    _treeModel->initWithFile("C:\\Test\\lds-media-library.xml");
    _treeModel->initFileStates();
    _listModel->filterByCategory(_treeModel->rootCategory());
    _textIndex.build(_treeModel->videos());
}

void MainController::search(const QString &text)
{
    if (text.size() > 0)
    {
        TextIndex::ResultsHash results;
        _textIndex.search(text, results);
        _listModel->setSearchResults(results);
    }
    else
    {
        _listModel->clearSearchResults();
    }
}

TreeModel * MainController::treeModel() const
{
    return _treeModel;
}

ListModel * MainController::listModel() const
{
    return _listModel;
}

void MainController::setCategory(const QModelIndex &index)
{
    CategoryNode *category = _treeModel->category(index);
    if (category)
        _listModel->filterByCategory(category);
}

void MainController::clickVideoButton(const QString &url)
{
    qDebug() << "MainController::clickVideoButton(" << url << ")";

    MediaFile *file = _treeModel->file(url);
    if (file)
    {
        MediaFile::State state = file->state();

        if (state == MediaFile::NotDownloadedState ||
            state == MediaFile::DownloadingPausedState ||
            state == MediaFile::DownloadErrorState)
        {
            _fileManager->downloadFile(file);
        }
        else if (state == MediaFile::DownloadingState)
        {
            _fileManager->cancelDownload(file);
        }
        else if (state == MediaFile::DownloadedState)
        {
            QString fileUrl = "file:///" + file->localFilePath();
            QDesktopServices::openUrl(fileUrl);
        }
    }
}

void MainController::downloadFinished(MediaFile *file)
{
    VideoFile *videoFile = dynamic_cast<VideoFile*>(file);
    if (videoFile)
        _listModel->updateVideo(videoFile->video());
}

void MainController::downloadProgress(MediaFile *file, qint64 bytesReceived, qint64 bytesTotal)
{
    Q_UNUSED(bytesReceived);
    Q_UNUSED(bytesTotal);

    VideoFile *videoFile = dynamic_cast<VideoFile*>(file);
    if (videoFile)
    {
        _listModel->updateVideo(videoFile->video());
    }
}

void MainController::setCategoryExport(const QModelIndex &index, int value)
{
    qDebug() << "MainController::setCategoryExport, value = " << value;
    CategoryNode *category = _treeModel->category(index);
    if (category)
    {
        category->setExport(value == Qt::Checked ? true : false);
        //_treeModel->updateCategory(category);
        _timer->start(500);
    }
}

void MainController::updateTree()
{
    _treeModel->updateAll();
}
