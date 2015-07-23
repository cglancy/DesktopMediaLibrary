#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <QHash>
#include <QList>
#include <QPixmap>

class CategoryNode;
class Video;
class QXmlStreamReader;

class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    TreeModel(QObject *parent);
    ~TreeModel();

    void initWithFile(const QString &xmlFilename);
	void clear();

    CategoryNode * rootCategory() const;
	QList<Video*> videos() const;

    QModelIndex index(CategoryNode *category) const;
    CategoryNode * category(const QModelIndex &index) const;

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

private:
    CategoryNode *_rootCategory;
    QHash<QString, Video*> _videoHash;
    QList<QPair<CategoryNode*, QString> > _refList;
	QString _videoDirectory, _mediaDirectory, _thumbnailDirectory;
    QPixmap _folderPixmap;
    QPixmap _smallCategoryPixmap, _mediumCategoryPixmap, _largeCategoryPixmap;
    QPixmap _smallAudioPixmap, _mediumAudioPixmap, _largeAudioPixmap;
    QPixmap _smallVideoPixmap, _mediumVideoPixmap, _largeVideoPixmap;
};

#endif // TREEMODEL_H
