#include "video.h"
#include "videofile.h"
#include "imagefile.h"

Video::Video(const QString &id)
    : _id(id),
    _thumbnailFile(0)
{
}

Video::~Video()
{
    if (_thumbnailFile)
        delete _thumbnailFile;

    qDeleteAll(_fileList);
}

bool Video::operator==(const Video &v) const
{
    return _id == v._id;
}

bool Video::operator<(const Video &v) const
{
    return _id < v._id;
}

QString Video::id() const
{
    return _id;
}

bool Video::isOnlineOnly() const
{
    return _fileList.size() == 0;
}

QString Video::title() const
{
    return _title;
}

void Video::setTitle(const QString &title)
{
    _title = title;
}

QString Video::summary() const
{
    return _summary;
}

void Video::setSummary(const QString &summary)
{
    _summary = summary;
}

ImageFile * Video::thumbnailFile() const
{
    return _thumbnailFile;
}

void Video::setThumbnailFile(ImageFile *file)
{
    _thumbnailFile = file;
}

QString Video::videoUrl() const
{
    return _videoUrl;
}

void Video::setVideoUrl(const QString &url)
{
    _videoUrl = url;
}

QString Video::length() const
{
    return _length;
}

void Video::setLength(const QString &length)
{
    _length = length;
}

VideoFile * Video::file(Quality quality) const
{
    return _fileMap.value(quality);
}

void Video::addFile(VideoFile *file)
{
    _fileList.append(file);

    if (_fileList.size() > 1)
        std::sort(_fileList.begin(), _fileList.end());

    _fileMap.clear();

    if (_fileList.size() > 0)
        _fileMap.insert(HighQuality, _fileList.at(0));

    if (_fileList.size() > 1)
        _fileMap.insert(MediumQuality, _fileList.at(1));

    if (_fileList.size() > 2)
        _fileMap.insert(LowQuality, _fileList.at(2));
}

void Video::addCategory(CategoryNode *category)
{
    _categoryList.append(category);
}

const QList<CategoryNode*> & Video::categories() const
{
    return _categoryList;
}

