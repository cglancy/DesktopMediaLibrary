#include "downloadmanager.h"
#include "mediafile.h"
#include "utility.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QSettings>
#include <QUrl>


DownloadManager::DownloadManager(QObject *parent)
	: QObject(parent)
{
	_networkAccessManager = new QNetworkAccessManager(this);
}

DownloadManager::~DownloadManager()
{

}

bool DownloadManager::isDownloading() const
{
    return _downloadHash.size() > 0;
}

void DownloadManager::downloadFile(MediaFile *file)
{
    if (file->state() == MediaFile::DownloadedState ||
        file->state() == MediaFile::DownloadingState)
        return;

    qDebug() << "FileManager::downloadFile(" << file->url() << ")";

    QString downloadPath = file->localDownloadPath();
    QFileInfo fi(downloadPath);
    qint64 fileSize = 0;

    if (!fi.exists())
    {
        if (!createPath(downloadPath))
        {
            qDebug() << "FileManager::downloadFile() Error: Unable to make directory path." << downloadPath;
            return;
        }
    }
    else
    {
        fileSize = fi.size();
    }

    QFile *downloadFile = new QFile(downloadPath);
    if (!downloadFile->open(QIODevice::WriteOnly | QIODevice::Append))
    {
        qDebug() << "FileManager::downloadFile() Error: Unable to open file" << downloadPath;
        delete downloadFile;
        return;
    }

    QNetworkRequest request;
    request.setUrl(QUrl(file->url()));

    if (fileSize > 0)
    {
        QString rangeStr = QString("bytes=%1-").arg(QString::number(fileSize));
        request.setRawHeader("Range", rangeStr.toUtf8());

        file->setBytesResumed(fileSize);
        file->setBytesReceived(fileSize);

        qDebug() << "Resuming download at " << fileSize << " bytes";
    }
    else
    {
        file->setBytesReceived(0);
        file->setBytesResumed(0);

        qDebug() << "Starting download.";
    }

    QNetworkReply *reply = _networkAccessManager->get(request);
    connect(reply, &QNetworkReply::readyRead, this, &DownloadManager::readyRead);
    connect(reply, &QNetworkReply::finished, this, &DownloadManager::finished);
    connect(reply, &QNetworkReply::downloadProgress, this, &DownloadManager::progress);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));

    DownloadData data(file, downloadFile);
    _downloadHash.insert(reply, data);

    file->setState(MediaFile::DownloadingState);

}

bool DownloadManager::createPath(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    QString dirPath = fileInfo.absolutePath();
    QDir dir(dirPath);
    return dir.mkpath(dirPath);
}

void DownloadManager::cancelDownload(MediaFile *file)
{
    if (!file)
        return;

    auto i = _downloadHash.begin();
    while (i != _downloadHash.end())
    {
        if (i.value().mediaFile == file)
        {
            i.key()->abort();
            return;
        }

        i++;
    }
}

void DownloadManager::cancelAllDownloads()
{
    auto i = _downloadHash.begin();
    while (i != _downloadHash.end())
    {
        i.key()->abort();
        i++;
    }
}

void DownloadManager::readyRead()
{
    QNetworkReply *reply = static_cast<QNetworkReply*>(sender());

    QFile *downloadFile = _downloadHash.value(reply).file;
    if (downloadFile)
        downloadFile->write(reply->readAll());
}

void DownloadManager::progress(qint64 bytesReceived, qint64 bytesTotal)
{
    QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
    MediaFile *file = _downloadHash.value(reply).mediaFile;
    if (file)
    {
        file->setFileSize(bytesTotal);
        file->setBytesReceived(file->bytesResumed() + bytesReceived);
    }

    emit downloadProgress(file, bytesReceived, bytesTotal);
}

void DownloadManager::finished()
{
    QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
    DownloadData data = _downloadHash.value(reply);
    MediaFile *file = data.mediaFile;
    QFile *downloadFile = data.file;

    if (reply->error() == QNetworkReply::NoError)
    {
        if (downloadFile)
        {
            downloadFile->write(reply->readAll());
            downloadFile->flush();
            downloadFile->close();
            QFile::rename(file->localDownloadPath(), file->localFilePath());
            delete downloadFile;

            if (file)
                file->setState(MediaFile::DownloadedState);
        }

        qDebug() << "FileManager::finished() for URL = " << file->url();
    }
    else if (reply->error() == QNetworkReply::OperationCanceledError)
    {
        if (downloadFile)
        {
            downloadFile->write(reply->readAll());
            downloadFile->flush();
            if (downloadFile->size() > 0)
            {
                file->setState(MediaFile::DownloadingPausedState);
                file->setBytesReceived(downloadFile->size());
            }
            else
            {
                file->setState(MediaFile::NotDownloadedState);
                file->setBytesReceived(0);
            }
            downloadFile->close();
            delete downloadFile;
        }

        qDebug() << "Download canceled for URL = " << file->url();
    }
    else
    {
        if (downloadFile)
        {
            downloadFile->close();
            delete downloadFile;
            file->setState(MediaFile::DownloadErrorState);
            file->setDownloadError(reply->errorString());
        }
    }

    emit downloadFinished(file);

    _downloadHash.remove(reply);
    reply->deleteLater();
}

void DownloadManager::error(QNetworkReply::NetworkError code)
{
    Q_UNUSED(code);

    QNetworkReply *reply = static_cast<QNetworkReply*>(sender());

    qDebug() << "Network error: " << reply->errorString();
}


