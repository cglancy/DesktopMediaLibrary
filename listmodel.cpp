#include "listmodel.h"
#include "categorynode.h"
#include "video.h"
#include "videofile.h"

#include <QLocale>
#include <QSet>
#include <QUrl>

class SearchRankCompare
{
public:
    SearchRankCompare() {}

    void setSearchResults(const TextIndex::ResultsHash &results)
    {
        _searchResults = results;
    }

    bool operator()(Video *a, Video *b)
    {
        int aScore = _searchResults.value(a, 0);
        int bScore = _searchResults.value(b, 0);
        return aScore > bScore;
    }

private:
    TextIndex::ResultsHash _searchResults;
};

static SearchRankCompare s_searchRankCompare;

class VideoTitleCompare
{
public:
    VideoTitleCompare() {}

    bool operator()(Video *a, Video *b)
    {
        return a->title() < b->title();
    }
};

static VideoTitleCompare s_videoTitleCompare;



ListModel::ListModel(QObject *parent)
	: QAbstractListModel(parent), _filterCategory(0)
{
}

ListModel::~ListModel()
{

}

Video * ListModel::video(const QModelIndex &index) const
{
    if (index.isValid() && index.row() < _videoList.count())
        return _videoList[index.row()];
    return 0;
}

const QList<Video*>  ListModel::videos() const
{
    return _videoList;
}

void ListModel::filterByCategory(CategoryNode *node)
{
    _filterCategory = node;

    if (_videoList.count() > 0)
    {
        beginRemoveRows(QModelIndex(), 0, _videoList.count() - 1);
        _videoList.clear();
        endRemoveRows();
    }

    if (!node)
        return;

    QSet<Video*> videoSet = node->allVideos();

    if (videoSet.count() > 0)
    {
        beginInsertRows(QModelIndex(), 0, videoSet.count() - 1);
        _videoList = QList<Video*>::fromSet(videoSet);
        std::sort(_videoList.begin(), _videoList.end(), s_videoTitleCompare);
        endInsertRows();
    }
}

void ListModel::setSearchResults(const TextIndex::ResultsHash &results)
{
    if (_videoList.count() > 0)
    {
        beginRemoveRows(QModelIndex(), 0, _videoList.count() - 1);
        _videoList.clear();
        endRemoveRows();
    }

    if (results.size() > 0)
    {
        beginInsertRows(QModelIndex(), 0, results.size() - 1);
        _videoList = results.keys();
        s_searchRankCompare.setSearchResults(results);
        std::sort(_videoList.begin(), _videoList.end(), s_searchRankCompare);
        _searchResultsHash = results;
        endInsertRows();
    }
}

void ListModel::clearSearchResults()
{
    filterByCategory(_filterCategory);
}

int ListModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
    return _videoList.count();
}

int ListModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
    return 1;
}

QHash<int, QByteArray> ListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TitleRole] = "title";
    roles[SummaryRole] = "summary";
    roles[LengthRole] = "length";
    roles[ThumbnailUrlRole] = "thumbnailUrl";
    roles[OnlineOnlyRole] = "onlineOnly";
    roles[OnlineUrlRole] = "onlineUrl";
    roles[HighQualityFileResolutionRole] = "highQualityFileResolution";
    roles[MediumQualityFileResolutionRole] = "mediumQualityFileResolution";
    roles[LowQualityFileResolutionRole] = "lowQualityFileResolution";
    roles[HighQualityFileUrlRole] = "highQualityFileUrl";
    roles[MediumQualityFileUrlRole] = "mediumQualityFileUrl";
    roles[LowQualityFileUrlRole] = "lowQualityFileUrl";

    return roles;
}

QVariant ListModel::data(const QModelIndex &index, int role) const
{
	QVariant v;
    Video *video = _videoList.at(index.row());
	if (!video)
		return v;

    if (role == TitleRole)
        v = QVariant(video->title());
    else if (role == SummaryRole)
        v = QVariant(video->summary());
    else if (role == LengthRole)
        v = QVariant(video->length());
    else if (role == ThumbnailUrlRole)
        v = QVariant(QUrl(video->thumbnailUrl()));
    else if (role == OnlineOnlyRole)
        v = QVariant(video->isOnlineOnly());
    else if (role == OnlineUrlRole)
        v = QVariant(video->videoUrl());
    else if (role == HighQualityFileResolutionRole)
        v = QVariant(fileResolutionString(video, Video::HighQuality));
    else if (role == MediumQualityFileResolutionRole)
        v = QVariant(fileResolutionString(video, Video::MediumQuality));
    else if (role == LowQualityFileResolutionRole)
        v = QVariant(fileResolutionString(video, Video::LowQuality));
    else if (role == HighQualityFileUrlRole)
        v = QVariant(fileUrlString(video, Video::HighQuality));
    else if (role == MediumQualityFileUrlRole)
        v = QVariant(fileUrlString(video, Video::MediumQuality));
    else if (role == LowQualityFileUrlRole)
        v = QVariant(fileUrlString(video, Video::LowQuality));

	return v;
}

QString ListModel::fileResolutionString(Video *video, Video::Quality quality) const
{
    QString str;

    VideoFile *file = video->file(quality);
    if (!file)
        return str;

    switch (file->resolution())
    {
    case VideoFile::LD360pResolution:
        str = "360p";
        break;
    case VideoFile::SD480pResolution:
        str = "480p";
        break;
    case VideoFile::HD720pResolution:
        str = "720p";
        break;
    case VideoFile::HD1080pResolution:
        str = "1080p";
        break;
    }

    return str;
}

QString ListModel::fileUrlString(Video *video, Video::Quality quality) const
{
    QString str;

    VideoFile *file = video->file(quality);
    if (!file)
        return str;

    str = file->url();
    return str;
}
