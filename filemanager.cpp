#include "filemanager.h"
#include "utility.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QSettings>
#include <QUrl>

#define DOWNLOADING_EXTENSION ".download"

FileManager::FileManager(QObject *parent)
	: QObject(parent)
{
	_networkAccessManager = new QNetworkAccessManager(this);
    _fileDirectory = Utility::currentVideoDirectory();
}

FileManager::~FileManager()
{

}

QString FileManager::fileDirectory() const
{
    return _fileDirectory;
}

void FileManager::setFileDirectory(const QString &dir)
{
    _fileDirectory = dir;
}

bool FileManager::isDownloading() const
{
    return _downloadHash.size() > 0;
}

FileManager::FileState FileManager::fileState(const QString &url) const
{
    if (_downloadHash.contains(url))
        return DownloadingState;

    QString path = localFilePath(url);

    if (QFileInfo::exists(path))
        return DownloadedState;

    path = localDownloadPath(url);

    if (QFileInfo::exists(path))
        return DownloadingPausedState;

    return NotDownloadedState;
}

QString FileManager::localFilePath(const QString &url) const
{
    QUrl fileUrl(url);
    return _fileDirectory + fileUrl.path();
}

QString FileManager::localDownloadPath(const QString &url) const
{
    return localFilePath(url) + DOWNLOADING_EXTENSION;
}

void FileManager::downloadFile(const QString &url)
{
    QString path = localDownloadPath(url);

    if (!createPath(path))
    {
        qDebug() << "FileManager::downloadFile() Error: Unable to make directory path." << path;
        return;
    }

    QFile *downloadFile = new QFile(path);
    if (!downloadFile->open(QIODevice::WriteOnly | QIODevice::Append))
    {
        qDebug() << "FileManager::downloadFile() Error: Unable to open file" << path;
        delete downloadFile;
        return;
    }

    QNetworkRequest request;
    request.setUrl(QUrl(url));

    QNetworkReply *reply = _networkAccessManager->get(request);
    connect(reply, &QNetworkReply::finished, this, &FileManager::downloadFinished);
    connect(reply, &QNetworkReply::downloadProgress, this, &FileManager::downloadProgress);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));

    DownloadData data(reply, downloadFile);
    _downloadHash.insert(url, data);

    qDebug() << "FileManager::downloadFile(" << url << ")";
}

bool FileManager::createPath(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    QString dirPath = fileInfo.absolutePath();
    QDir dir(dirPath);
    return dir.mkpath(dirPath);
}

void FileManager::cancelDownload(const QString &url)
{

}

void FileManager::cancelAllDownloads()
{

}

void FileManager::readyRead()
{
    QNetworkReply *reply = static_cast<QNetworkReply*>(sender());

    QFile *downloadFile = _downloadHash.value(reply->url().toString()).file;
    if (downloadFile)
        downloadFile->write(reply->readAll());
}

void FileManager::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    qDebug() << "FileManager::downloadProgress() received " << bytesReceived << " of " << bytesTotal;
}

void FileManager::downloadFinished()
{
    QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
    QString url = reply->url().toString();
    DownloadData data = _downloadHash.value(url);

    if (reply->error() == QNetworkReply::NoError)
    {
        QFile *downloadFile = data.file;
        if (downloadFile)
        {
            downloadFile->write(reply->readAll());
            downloadFile->flush();
            downloadFile->close();
            QFile::rename(localDownloadPath(url), localFilePath(url));
            delete downloadFile;
        }

        emit fileDownloaded(url);
        qDebug() << "FileManager::downloadFinished() for URL = " << url;
    }

    _downloadHash.remove(url);
    reply->deleteLater();
}

void FileManager::error(QNetworkReply::NetworkError code)
{
    Q_UNUSED(code);

    QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
    qDebug() << "Network error: " << reply->errorString();
}

#if 0
void FileManager::downloadFile(RemoteFile *file)
{
	Q_ASSERT(file);
	if (!file)
		return;

    if (!proceedWithDownload(file))
        return;

    QNetworkRequest request;
    request.setUrl(QUrl(file->url()));
    request.setRawHeader("User-Agent", _userAgentStr);

	QNetworkReply *reply = _networkAccessManager->get(request);
	connect(reply, SIGNAL(finished()), this, SLOT(fileFinished()));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));

	file->setFileSize(0);
	file->setState(RemoteFile::DownloadingState);

	_videoThumbReplyMap.insert(reply, file);

	qDebug("Start download %s", file->url().toLocal8Bit().data());
}

bool FileManager::proceedWithDownload(RemoteFile *file)
{
    if (file->url().isEmpty())
    {
        file->setState(RemoteFile::DownloadErrorState);
        file->setErrorMessage(tr("Error: No URL"));
        emit fileDownloaded(file);
        return false;
    }

    QUrl url(file->url());
    QString schemeStr = url.scheme();

    if (schemeStr == "file")
    {
        if (QFile::exists(file->localPath()))
        {
            file->setState(RemoteFile::DownloadedState);
            emit fileDownloaded(file);
            return false;
        }
        else
        {
            file->setState(RemoteFile::DownloadErrorState);
            file->setErrorMessage(tr("Error: Local file %1 not found.").arg(file->localPath()));
            emit fileDownloaded(file);
            return false;
        }
    }
    else if (schemeStr == "http")
    {
        if (QFile::exists(file->localPath()) && !file->alwaysDownload())
        {
            file->setState(RemoteFile::DownloadedState);
            emit fileDownloaded(file);
            return false;
        }
    }
    else
    {
        if (QFile::exists(file->localPath()))
        {
            file->setState(RemoteFile::DownloadedState);
            emit fileDownloaded(file);
            return false;
        }
        else
        {
            file->setState(RemoteFile::DownloadErrorState);
            file->setErrorMessage(tr("Error: Invalid URL"));
            emit fileDownloaded(file);
            return false;
        }
    }

    return true;
}

void FileManager::fileFinished()
{
	QNetworkReply *reply = static_cast<QNetworkReply*>(sender());

	if (_videoThumbReplyMap.contains(reply))
	{
		RemoteFile *file = _videoThumbReplyMap.value(reply);
		_videoThumbReplyMap.remove(reply);

		if (!file)
			return;

		if (reply->error() == QNetworkReply::NoError)
		{
			writeFile(file->localPath(), reply->readAll());
			file->setState(RemoteFile::DownloadedState);
			emit fileDownloaded(file);

			qDebug("Downloaded %s", file->fileName().toLocal8Bit().data());
		}
		else
		{
			file->setState(RemoteFile::DownloadErrorState);
			file->setErrorMessage(reply->errorString());
			emit fileDownloaded(file);
		}
	}

	reply->deleteLater();
}

void FileManager::downloadVideo(RemoteFile *file)
{
	Q_ASSERT(file);
	if (!file)
		return;

	if (file->state() == RemoteFile::DownloadingState)
		return;

    if (!proceedWithDownload(file))
        return;

	QNetworkRequest request;
	request.setUrl(QUrl(file->url()));
	request.setRawHeader("User-Agent", _userAgentStr);

	QFileInfo fi(file->downloadPath());
	if (fi.exists())
	{
		QString rangeStr = QString("bytes=%1-").arg(QString::number(fi.size()));
		request.setRawHeader("Range", rangeStr.toUtf8());
		file->setBytesResumed(fi.size());
	}
	else
	{
        file->setBytesResumed(0);
	}

	QFile *downloadFile = new QFile(file->downloadPath());
	if (!downloadFile->open(QIODevice::WriteOnly | QIODevice::Append))
	{
		file->setState(RemoteFile::DownloadErrorState);
		file->setErrorMessage(tr("Error: Unable to open file %1.").arg(file->downloadPath()));
		delete downloadFile;
		return;
	}

	file->setDownloadFile(downloadFile);
	file->setState(RemoteFile::DownloadingState);
    file->setBytesReceived(file->bytesResumed());

	QNetworkReply *reply = _networkAccessManager->get(request);
	connect(reply, SIGNAL(finished()), this, SLOT(videoDownloadFinished()));
	connect(reply, SIGNAL(readyRead()), this, SLOT(videoReadyRead()));
	connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(downloadProgress(qint64, qint64)));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));

	_videoReplyMap.insert(reply, file);

	emitAllDownloadsProgress();
	emit downloadStatus(QString(tr("Downloading %1...")).arg(file->fileName()));

	qDebug("Start download %s", file->url().toLocal8Bit().data());
}

void FileManager::videoReadyRead()
{
	QNetworkReply *reply = static_cast<QNetworkReply*>(sender());

	if (_videoReplyMap.contains(reply))
	{
		RemoteFile *file = _videoReplyMap.value(reply);
		file->downloadFile()->write(reply->readAll());
	}
}

void FileManager::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    Q_UNUSED(bytesTotal);

	QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
	if (_videoReplyMap.contains(reply))
	{
		RemoteFile *file = _videoReplyMap.value(reply);
		file->setBytesReceived(file->bytesResumed() + bytesReceived);
        emitAllDownloadsProgress();
	}
}

void FileManager::error(QNetworkReply::NetworkError code)
{
    Q_UNUSED(code);

	QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
    qDebug("Network error: %s", reply->errorString().toLocal8Bit().data());
 }

void FileManager::videoDownloadFinished()
{
	QNetworkReply *reply = static_cast<QNetworkReply*>(sender());

	if (_videoReplyMap.contains(reply))
	{
		RemoteFile *file = _videoReplyMap.value(reply);
        _videoReplyMap.remove(reply);

		if (reply->error() == QNetworkReply::NoError)
		{
            file->downloadFile()->write(reply->readAll());
            file->downloadFile()->flush();
            file->downloadFile()->close();
            QFile::rename(file->downloadPath(), file->localPath());
            delete file->downloadFile();
			file->setDownloadFile(0);

            file->setState(RemoteFile::DownloadedState);
            emit videoDownloaded(file);
            emit downloadStatus(QString(tr("Downloaded %1")).arg(file->fileName()));

            qDebug("Downloaded %s", file->fileName().toLocal8Bit().data());
		}
		else if (reply->error() == QNetworkReply::RemoteHostClosedError && file->retryCount() < 3)
		{
			file->setRetryCount(file->retryCount() + 1);
			file->downloadFile()->close();
			delete file->downloadFile();
			file->setDownloadFile(0);
			file->setState(RemoteFile::DownloadPausedState);

			// try again
			qDebug("Retrying %s", file->fileName().toLocal8Bit().data());
			downloadVideo(file);
		}
		else if (reply->error() == QNetworkReply::OperationCanceledError)
		{
			file->downloadFile()->write(reply->readAll());
			file->downloadFile()->flush();
			if (file->downloadFile()->size() > 0)
			{
				file->setState(RemoteFile::DownloadPausedState);
				file->setBytesReceived(file->downloadFile()->size());
			}
			else
			{
				file->setState(RemoteFile::NotDownloadedState);
				file->setBytesReceived(0);
			}
			file->downloadFile()->close();
			delete file->downloadFile();
			file->setDownloadFile(0);

			emitAllDownloadsProgress();
		}
		else
		{
            file->downloadFile()->close();
            delete file->downloadFile();
			file->setDownloadFile(0);
			file->setState(RemoteFile::DownloadErrorState);
			file->setErrorMessage(reply->errorString());
			file->setBytesReceived(0);

			emitAllDownloadsProgress();
		}

        if (_videoReplyMap.count() == 0)
		{
            emit allDownloadsFinished();
		}
	}

    reply->deleteLater();
}

void FileManager::emitAllDownloadsProgress()
{
    if (_videoReplyMap.count() > 0)
    {
        qint64 bytesReceived = 0;
        qint64 bytesTotal = 0;

        foreach (RemoteFile *file, _videoReplyMap)
        {
            bytesReceived += file->bytesReceived();
            bytesTotal += file->fileSize();
        }

        emit allDownloadsProgress(bytesReceived, bytesTotal);
    }
    else
    {
        emit allDownloadsProgress(1, 1);
    }
}

void FileManager::writeFile(const QString &fileName, const QByteArray &data)
{
	// force overwrite??
	if (QFile::exists(fileName))
		QFile::remove(fileName);

	QFile localFile(fileName);
	if (!localFile.open(QIODevice::WriteOnly))
		return;

	localFile.write(data);
	localFile.close();

	return;
}

bool FileManager::isDownloading() const
{
	return _videoReplyMap.count() > 0;
}

void FileManager::cancelVideoDownload(RemoteFile *videoFile)
{
	if (!videoFile)
		return;

	QMap<QNetworkReply*, RemoteFile*>::iterator i = _videoReplyMap.begin();
	while (i != _videoReplyMap.end()) 
	{
		if (i.value() == videoFile)
		{
			i.key()->abort();
			return;
		}

		i++;
	}

	// not found, so at least change the state
	videoFile->determineFileState();
}

void FileManager::cancelAllVideoDownloads()
{

}
#endif
