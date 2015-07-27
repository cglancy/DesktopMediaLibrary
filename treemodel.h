#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <QHash>
#include <QList>
#include <QPixmap>

class CategoryNode;
class Video;
class MediaFile;
class QXmlStreamReader;

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum Role
    {
        NameRole = Qt::UserRole + 1,
        HighQualitySizeRole,
        HighQualityExportRole,
        MediumQualitySizeRole,
        MediumQualityExportRole,
        LowQualitySizeRole,
        LowQualityExportRole
    };

public:
    TreeModel(QObject *parent);
    ~TreeModel();

    void initWithFile(const QString &xmlFilename);
    void initFileStates();
	void clear();

    CategoryNode * rootCategory() const;
	QList<Video*> videos() const;

    MediaFile * file(const QString &url) const;
    Video * video(const QString &url) const;

    QModelIndex index(CategoryNode *category) const;
    CategoryNode * category(const QModelIndex &index) const;

    QHash<int, QByteArray> roleNames() const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

private:
    CategoryNode* parseCategory(QXmlStreamReader& xml, CategoryNode *parent);
    void parseVideoRef(QXmlStreamReader& xml, CategoryNode *parent);
    Video* parseVideo(QXmlStreamReader& xml);
	void parseFile(QXmlStreamReader& xml, Video *video);
	bool parseLibrary(QXmlStreamReader& xml);
    bool createDirectory(const QString &path);
    QString stripUrlQuery(const QString &url);
    static void initFileState(MediaFile *file);

private:
    CategoryNode *_rootCategory;
    QHash<QString, Video*> _videoHash;
    QHash<QString, Video*> _videoUrlHash;
    QHash<QString, MediaFile*> _fileUrlHash;
    QList<QPair<CategoryNode*, QString> > _refList;
	QString _videoDirectory, _mediaDirectory, _thumbnailDirectory;
    QPixmap _folderPixmap;
    QPixmap _smallCategoryPixmap, _mediumCategoryPixmap, _largeCategoryPixmap;
    QPixmap _smallAudioPixmap, _mediumAudioPixmap, _largeAudioPixmap;
    QPixmap _smallVideoPixmap, _mediumVideoPixmap, _largeVideoPixmap;
};

#endif // TREEMODEL_H
