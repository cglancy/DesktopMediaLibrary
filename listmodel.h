#ifndef LISTMODEL_H
#define LISTMODEL_H

#include <QAbstractListModel>
#include <QPixmap>
#include <QList>
#include "textindex.h"
#include "video.h"

class CategoryNode;

class ListModel : public QAbstractListModel
{
	Q_OBJECT

public:
    enum Role
    {
        TitleRole   = Qt::UserRole + 1,
        SummaryRole,
        ThumbnailUrlRole,
        LengthRole,
        CategoryPathRole,
        HighQualityFileUrlRole,
        HighQualityFileResolutionRole,
        HighQualityFileStateRole,
        MediumQualityFileUrlRole,
        MediumQualityFileResolutionRole,
        MediumQualityFileStateRole,
        LowQualityFileUrlRole,
        LowQualityFileResolutionRole,
        LowQualityFileStateRole
    };

public:
    ListModel(QObject *parent);
    ~ListModel();

    void filterByCategory(CategoryNode *node);
    void setSearchResults(const TextIndex::ResultsHash &results);
    void clearSearchResults();

    Video * video(const QModelIndex &index) const;
    const QList<Video*> videos() const;

    QHash<int, QByteArray> roleNames() const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;

private:
    QString fileResolutionString(Video *video, Video::Quality quality) const;
    QString fileUrlString(Video *video, Video::Quality quality) const;

private:
    CategoryNode *_filterCategory;
    QList<Video*> _videoList;
    TextIndex::ResultsHash _searchResultsHash;
};

#endif // LISTMODEL_H
