#include "treemodel.h"
#include "categorynode.h"
#include "videofile.h"
#include "imagefile.h"
#include "video.h"
#include "utility.h"

#include <QStack>
#include <QXmlStreamReader>
#include <QFile>
#include <QMessageBox>
#include <QLocale>
#include <QUrl>
#include <QDir>
#include <QApplication>
#include <QtConcurrent>
#include <QDebug>

TreeModel::TreeModel(QObject *parent)
    : QAbstractItemModel(parent), _rootCategory(0)
{
}

TreeModel::~TreeModel()
{
	clear();
}

void TreeModel::clear()
{
    if (_rootCategory)
	{
        beginResetModel();
        delete _rootCategory;
        _rootCategory = 0;
        endResetModel();
	}

    auto i = _videoHash.constBegin();
    while (i != _videoHash.constEnd())
	{
		delete i.value();
		++i;
	}
    _videoHash.clear();

    _videoUrlHash.clear();
    _fileUrlHash.clear();
}

CategoryNode * TreeModel::rootCategory() const
{
    return _rootCategory;
}

QList<Video*> TreeModel::videos() const
{
    return _videoHash.values();
}

MediaFile * TreeModel::fileForUrl(const QString &url) const
{
    return _fileUrlHash.value(url);
}

Video * TreeModel::videoForUrl(const QString &url) const
{
    return _videoUrlHash.value(url);
}

Video *TreeModel::video(const QString &id) const
{
    return _videoHash.value(id);
}

void TreeModel::initWithFile(const QString &xmlFilename)
{
    beginResetModel();

	clear();
	_refList.clear();

    QFile* file = new QFile(xmlFilename);

    if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) 
    {
        QMessageBox::critical(QApplication::activeWindow(), Utility::applicationName(), tr("Unable to open file %1.").arg(xmlFilename), QMessageBox::Ok);
        delete file;
        return;
    }

    QXmlStreamReader xml(file);
    QStack<CategoryNode*> categoryState;
	Video *currentVideo = 0;

    _rootCategory = new CategoryNode(0);
    categoryState.push(_rootCategory);

    while (!xml.atEnd() && !xml.hasError()) 
    {
        QXmlStreamReader::TokenType token = xml.readNext();

        if (token == QXmlStreamReader::StartDocument) 
            continue;

        if (token == QXmlStreamReader::StartElement) 
        {
			if (xml.name() == "library") 
			{
				if (!parseLibrary(xml))
                    break; // abort xml parsing
			}
			else if (xml.name() == "category") 
			{
                parseCategory(xml, categoryState);
			}
			else if (xml.name() == "videoref")
			{
                parseVideoRef(xml, categoryState.top());
			}
            else if (xml.name() == "video")
            {
                currentVideo = parseVideo(xml);
            }
            else if (xml.name() == "file")
            {
                parseFile(xml, currentVideo);
            }
        }
		else if (token == QXmlStreamReader::EndElement)
		{
			if (xml.name() == "category")
                categoryState.pop();
		}
    }

    if (xml.hasError()) 
    {
        QMessageBox::critical(QApplication::activeWindow(), Utility::applicationName(), xml.errorString(), QMessageBox::Ok);
        delete file;
		return;
    }

	xml.clear();
	file->close();
    delete file;

	for (int i = 0; i < _refList.count(); i++)
	{
        CategoryNode *category = _refList[i].first;
		QString id = _refList[i].second;

        if (_videoHash.contains(id))
		{
            Video *video = _videoHash.value(id);
            if (category && video)
            {
                category->addVideo(video);
                video->addCategory(category);
            }
		}
		else
		{
            qDebug() << "Missing video " << id << " for category " << category->name();
		}
	}

	_refList.clear();

    endResetModel();
}

void TreeModel::initFileState(MediaFile *file)
{
    if (QFile::exists(file->localFilePath()))
        file->setState(MediaFile::DownloadedState);
    else if (QFile::exists(file->localDownloadPath()))
        file->setState(MediaFile::DownloadingPausedState);
}

void TreeModel::initFileStates()
{
    QList<MediaFile*> files = _fileUrlHash.values();
#if 0
    QtConcurrent::map(files, &TreeModel::initFileState);
#else
    foreach (MediaFile *file, files)
        initFileState(file);
#endif
}

bool TreeModel::parseLibrary(QXmlStreamReader& xml)
{
    Q_UNUSED(xml);
    //QXmlStreamAttributes attributes = xml.attributes();

    return true;
}

void TreeModel::parseCategory(QXmlStreamReader& xml, QStack<CategoryNode*> &stack)
{
    CategoryNode *category = new CategoryNode(stack.top());
    stack.push(category);

	QXmlStreamAttributes attributes = xml.attributes();

	if (attributes.hasAttribute("name")) 
        category->setName(attributes.value("name").toString());
}

void TreeModel::parseVideoRef(QXmlStreamReader& xml, CategoryNode *category)
{
	QXmlStreamAttributes attributes = xml.attributes();

    if (attributes.hasAttribute("ref"))
	{	
        QString ref = attributes.value("ref").toString();
        _refList.append(QPair<CategoryNode*, QString>(category, ref));
	}
}

Video* TreeModel::parseVideo(QXmlStreamReader& xml)
{
	QXmlStreamAttributes attributes = xml.attributes();

	if (!attributes.hasAttribute("id"))
		return 0;

	QString id = attributes.value("id").toString();
    Video *video = new Video(id);
    _videoHash.insert(id, video);

    if (attributes.hasAttribute("title"))
        video->setTitle(attributes.value("title").toString());

    if (attributes.hasAttribute("summary"))
        video->setSummary(attributes.value("summary").toString());

    if (attributes.hasAttribute("length"))
        video->setLength(attributes.value("length").toString());

    if (attributes.hasAttribute("thumbnailUrl"))
    {
        ImageFile *imageFile = new ImageFile(video);
        video->setThumbnailFile(imageFile);
        imageFile->setUrl(attributes.value("thumbnailUrl").toString());
    }

    if (attributes.hasAttribute("videoUrl"))
        video->setVideoUrl(attributes.value("videoUrl").toString());

    _videoUrlHash.insert(video->videoUrl(), video);

	return video;
}

void TreeModel::parseFile(QXmlStreamReader& xml, Video *video)
{
    if (!video)
        return;

    QXmlStreamAttributes attributes = xml.attributes();

    QString quality;
    if (attributes.hasAttribute("quality"))
        quality = attributes.value("quality").toString();

    VideoFile::Resolution resolution = VideoFile::UnknownResolution;

    if (quality == "360p" || quality == "Small")
        resolution = VideoFile::LD360pResolution;
    else if (quality == "480p")
        resolution = VideoFile::SD480pResolution;
    else if (quality == "720p" || quality == "720P" || quality == "Medium")
        resolution = VideoFile::HD720pResolution;
    else if (quality == "1080p" || quality == "Large")
        resolution = VideoFile::HD1080pResolution;
    else
        qDebug() << "Unknown format = " << quality;

    if (!quality.isEmpty())
    {
        VideoFile *file = new VideoFile(video, resolution);

        if (attributes.hasAttribute("link"))
            file->setUrl(attributes.value("link").toString());

        if (attributes.hasAttribute("size"))
            file->setSize(attributes.value("size").toString());

        _fileUrlHash.insert(file->url(), file);
    }
}

QModelIndex TreeModel::index(CategoryNode *category) const
{
    return createIndex(category->row(), 0, category);
}

CategoryNode * TreeModel::category(const QModelIndex &index) const
{
	return static_cast<CategoryNode*>(index.internalPointer());
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
//	if (!hasIndex(row, column, parent))
//		return QModelIndex();

    CategoryNode *parentCategory;

	if (!parent.isValid())
        parentCategory = _rootCategory;
	else
        parentCategory = static_cast<CategoryNode*>(parent.internalPointer());

    CategoryNode *childCategory = parentCategory->categories().at(row);
    if (childCategory)
        return createIndex(row, column, childCategory);
	else
		return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index ) const
{
	if (!index.isValid())
		return QModelIndex();

    CategoryNode *childCategory = static_cast<CategoryNode*>(index.internalPointer());
    CategoryNode *parentCategory = childCategory->parent();

    if (parentCategory == _rootCategory || parentCategory == 0)
		return QModelIndex();

    return createIndex(parentCategory->row(), 0, parentCategory);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    CategoryNode *parentCategory;
	if (parent.column() > 0)
		return 0;

	if (!parent.isValid())
        parentCategory = _rootCategory;
	else
        parentCategory = static_cast<CategoryNode*>(parent.internalPointer());

    if (parentCategory)
        return parentCategory->categories().size();

	return 0;
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        if (section == 0)
            return QVariant(tr("Categories"));
        //else if (section == 1)
        //    return QVariant("Size");
    }

    return QVariant();
}

QHash<int, QByteArray> TreeModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[ExportRole] = "export";
    return roles;
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    QVariant v;

    if (!index.isValid())
        return v;

    CategoryNode *category = static_cast<CategoryNode*>(index.internalPointer());
    if (!category)
        return v;

    if (role == NameRole)
        v = QVariant(category->name());
    else if (role == ExportRole)
        v = QVariant((int) category->exportState());

    return v;
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
	Q_UNUSED(index);

    Qt::ItemFlags f = Qt::ItemIsSelectable |  Qt::ItemIsEnabled | Qt::ItemIsEditable;
    return f;
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool success = false;

    CategoryNode *category = static_cast<CategoryNode*>(index.internalPointer());
    if (!category)
        return success;

    if (role == ExportRole)
    {
        category->setExport(value.toInt() == 2);
        dataChanged(index, index);
        success = true;
    }

    return success;
}

QString TreeModel::stripUrlQuery(const QString &urlStr)
{
    QString stripStr(urlStr);
    if (stripStr.contains('?'))
        stripStr.truncate(stripStr.indexOf('?'));
    return stripStr;
}

void TreeModel::updateCategory(CategoryNode *category)
{
    Q_UNUSED(category);
    //updateParent(category);
    //updateChildren(category);
    updateAll(_rootCategory);
}

void TreeModel::updateParent(CategoryNode *category)
{
    QVector<int> roles;
    roles.append(ExportRole);

    QModelIndex categoryIndex = index(category);
    emit dataChanged(categoryIndex, categoryIndex, roles);

    CategoryNode *parent = category->parent();
    if (parent)
        updateParent(parent);
}

void TreeModel::updateChildren(CategoryNode *category)
{
    if (category->categories().size() > 0)
    {
        QVector<int> roles;
        roles.append(ExportRole);

        QModelIndex firstIndex = index(category->categories().first());
        QModelIndex lastIndex = index(category->categories().last());
        emit dataChanged(firstIndex, lastIndex, roles);

        foreach (CategoryNode *child, category->categories())
            updateChildren(child);
    }
}

void TreeModel::updateAll(CategoryNode *category)
{
#if 0
    if (!category)
        category = _rootCategory;

    QModelIndex categoryIndex = index(category);
    emit dataChanged(categoryIndex, categoryIndex);

    foreach (CategoryNode *childCategory, category->categories())
        updateAll(childCategory);
#else
    Q_UNUSED(category);
    emit layoutAboutToBeChanged();
    emit layoutChanged();
    emit dataChanged(QModelIndex(), QModelIndex());
#endif
}
