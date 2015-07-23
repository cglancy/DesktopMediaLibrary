#ifndef VIDEOFILE_H
#define VIDEOFILE_H

#include <QString>

class Video;

class VideoFile
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

    QString url() const;
    void setUrl(const QString &url);

    QString size() const;
    void setSize(const QString &size);

private:
    Video *_video;
    QString _url;
    QString _size;
    Resolution _resolution;
};

#endif // VIDEOFILE_H

