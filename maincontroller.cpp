#include "maincontroller.h"
#include "treemodel.h"
#include "listmodel.h"
#include "downloadmanager.h"
#include "mediafile.h"
#include "videofile.h"
#include "categorynode.h"
#include "progressimageprovider.h"
#include "thumbnailimageprovider.h"

#include <QRegExp>
#include <QDesktopServices>
#include <QTimer>
#include <QModelIndex>
#include <QDebug>

MainController::MainController(QObject *parent)
    : QObject(parent),
      _networkAccessManager(0),
      _treeModel(0),
      _listModel(0),
      _downloadManager(0),
      _progressImageProvider(0),
      _thumbnailImageProvider(0)
{
    _networkAccessManager = new QNetworkAccessManager(this);

    _downloadManager = new DownloadManager(_networkAccessManager, this);
    _treeModel = new TreeModel(this);
    _listModel = new ListModel(this);
    _progressImageProvider = new ProgressImageProvider();
    _thumbnailImageProvider = new ThumbnailImageProvider(_networkAccessManager, _treeModel);

    connect(_downloadManager, &DownloadManager::fileProgress, this, &MainController::downloadProgress);
    connect(_downloadManager, &DownloadManager::fileFinished, this, &MainController::downloadFinished);
}

MainController::~MainController()
{
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

ProgressImageProvider *MainController::progressImageProvider() const
{
    return _progressImageProvider;
}

ThumbnailImageProvider *MainController::thumbnailImageProvider() const
{
    return _thumbnailImageProvider;
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

    MediaFile *file = _treeModel->fileForUrl(url);
    if (file)
    {
        MediaFile::State state = file->state();

        if (state == MediaFile::NotDownloadedState ||
            state == MediaFile::DownloadingPausedState ||
            state == MediaFile::DownloadErrorState)
        {
            _downloadManager->downloadFile(file);
        }
        else if (state == MediaFile::DownloadingState)
        {
            _downloadManager->cancelFileDownload(file);
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
        _treeModel->updateCategory(category);
    }
}

QVariant MainController::firstIndex()
{
    CategoryNode *root = _treeModel->rootCategory();
    CategoryNode *firstIndex = root->categories().first();
    QModelIndex index = _treeModel->index(firstIndex);
    return QVariant(index);
}
