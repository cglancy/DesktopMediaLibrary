#ifndef VIDEO_H
#define VIDEO_H

#include <QString>
#include <QList>
#include <QMap>

class VideoFile;
class ImageFile;
class CategoryNode;

class Video
{
public:
    enum Quality
    {
        LowQuality,
        MediumQuality,
        HighQuality
    };

public:
    Video(const QString &id);
    ~Video();

    bool operator==(const Video &v) const;
    bool operator<(const Video &v) const;

    QString id() const;

    bool isOnlineOnly() const;

    QString title() const;
    void setTitle(const QString &title);

    QString summary() const;
    void setSummary(const QString &summary);

    ImageFile * thumbnailFile() const;
    void setThumbnailFile(ImageFile *file);

    QString videoUrl() const;
    void setVideoUrl(const QString &url);

    QString length() const;
    void setLength(const QString &length);

    void addFile(VideoFile *file);
    const QList<VideoFile*> & files() const;

    VideoFile * file(Quality quality) const;

    void addCategory(CategoryNode *category);
    const QList<CategoryNode*> & categories() const;

private:
    QString _id;
    QString _title;
    QString _summary;
    QString _videoUrl;
    QString _length;
    ImageFile *_thumbnailFile;
    QList<VideoFile*> _fileList;
    QList<CategoryNode*> _categoryList;
    QMap<Quality, VideoFile*> _fileMap;
};

#endif // VIDEO_H
