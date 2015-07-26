#include "videofile.h"
#include "video.h"

VideoFile::VideoFile(Video *video, Resolution resolution)
    : MediaFile(MediaFile::VideoType),
      _video(video),
      _resolution(resolution)
{
    if (video)
        video->addFile(this);
}

bool VideoFile::operator<(const VideoFile &file) const
{
    return _resolution < file._resolution;
}

Video * VideoFile::video() const
{
    return _video;
}

QString VideoFile::size() const
{
    return _size;
}

void VideoFile::setSize(const QString &size)
{
    _size = size;
}

VideoFile::Resolution VideoFile::resolution() const
{
    return _resolution;
}

