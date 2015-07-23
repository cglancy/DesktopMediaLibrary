#include "video.h"
#include "videofile.h"

Video::Video(const QString &id)
    : _id(id)
{
}

Video::~Video()
{
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

QString Video::thumbnailUrl() const
{
    return _thumbUrl;
}

void Video::setThumbnailUrl(const QString &url)
{
    _thumbUrl = url;
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

