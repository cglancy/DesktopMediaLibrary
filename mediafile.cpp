#include "mediafile.h"
#include "utility.h"

#include <QUrl>

MediaFile::MediaFile(MediaFile::Type type)
    : _type(type),
      _state(NotDownloadedState),
      _size(0),
      _bytesReceived(0),
      _bytesResumed(0)
{

}

MediaFile::~MediaFile()
{

}

MediaFile::Type MediaFile::type() const
{
    return _type;
}

MediaFile::State MediaFile::state() const
{
    return _state;
}

void MediaFile::setState(MediaFile::State state)
{
    _state = state;
}

QString MediaFile::url() const
{
    return _url;
}

void MediaFile::setUrl(const QString &url)
{
    _url = url;
}

QString MediaFile::downloadError() const
{
    return _downloadError;
}

void MediaFile::setDownloadError(const QString &error)
{
    _downloadError = error;
}

QString MediaFile::localFilePath() const
{
    QUrl fileUrl(_url);
    return Utility::currentVideoDirectory() + fileUrl.path();
}

QString MediaFile::localDownloadPath() const
{
    return localFilePath() + ".download";
}

qint64 MediaFile::fileSize() const
{
    return _size;
}

void MediaFile::setFileSize(qint64 sizeBytes)
{
    _size = sizeBytes;
}

qint64 MediaFile::bytesReceived() const
{
    return _bytesReceived;
}

void MediaFile::setBytesReceived(qint64 bytesReceived)
{
    _bytesReceived = bytesReceived;
}

qint64 MediaFile::bytesResumed() const
{
    return _bytesResumed;
}

void MediaFile::setBytesResumed(qint64 bytesResumed)
{
    _bytesResumed = bytesResumed;
}

int MediaFile::percentComplete() const
{
    int percent = 0;
    if (_size > 0)
        percent = (int) (_bytesReceived * 100. / _size);
    return percent;
}
