#ifndef VIDEOFILE_H
#define VIDEOFILE_H

#include "mediafile.h"

class Video;

class VideoFile : public MediaFile
{
public:
    enum Resolution
    {
        UnknownResolution,
        LD360pResolution,
        SD480pResolution,
        HD720pResolution,
        HD1080pResolution
    };

public:
    VideoFile(Video *video, Resolution resolution);

    bool operator<(const VideoFile &file) const;

    Video * video() const;
    Resolution resolution() const;

    QString size() const;
    void setSize(const QString &size);

private:
    Video *_video;
    QString _size;
    Resolution _resolution;
};

#endif // VIDEOFILE_H

