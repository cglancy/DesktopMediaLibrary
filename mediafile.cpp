#include "mediafile.h"
#include "utility.h"

#include <QUrl>

MediaFile::MediaFile(MediaFile::Type type)
    : _type(type),
      _state(NotDownloadedState)
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

int MediaFile::progress() const
{
    return _progress;
}

void MediaFile::setProgress(int percent)
{
    _progress = percent;
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
