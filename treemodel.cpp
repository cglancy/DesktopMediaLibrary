#include "treemodel.h"
#include "categorynode.h"
#include "videofile.h"
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
#include <QDebug>

TreeModel::TreeModel(QObject *parent)
    : QAbstractItemModel(parent), _rootCategory(0)
{
    _folderPixmap = QPixmap(":/pvm/images/video_folder_16.png");

    _smallCategoryPixmap = QPixmap(":/pvm/images/video_folder_48.png");
    _mediumCategoryPixmap = QPixmap(":/pvm/images/video_folder_128.png");
    _largeCategoryPixmap = QPixmap(":/pvm/images/video_folder_256.png");

    _smallVideoPixmap = QPixmap(":/pvm/images/video_48.png");
    _mediumVideoPixmap = QPixmap(":/pvm/images/video_128.png");
    _largeVideoPixmap = QPixmap(":/pvm/images/video_256.png");

    _smallAudioPixmap = QPixmap(":/pvm/images/audio_48.png");
    _mediumAudioPixmap = QPixmap(":/pvm/images/audio_128.png");
    _largeAudioPixmap = QPixmap(":/pvm/images/audio_256.png");
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
}

CategoryNode * TreeModel::rootCategory() const
{
    return _rootCategory;
}

QList<Video*> TreeModel::videos() const
{
    return _videoHash.values();
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

	_videoDirectory = Utility::currentVideoDirectory();
	_mediaDirectory = _videoDirectory;
	_thumbnailDirectory = _mediaDirectory + "/thumbnails";

    QXmlStreamReader xml(file);
	QStack<CategoryNode*> nodeStack;
	Video *currentVideo = 0;

    _rootCategory = new CategoryNode(0);
    nodeStack.push(_rootCategory);

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
                CategoryNode *category = parseCategory(xml, nodeStack.top());
                nodeStack.push(category);
			}
			else if (xml.name() == "videoref")
			{
				parseVideoRef(xml, nodeStack.top());
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
				nodeStack.pop();
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

bool TreeModel::parseLibrary(QXmlStreamReader& xml)
{
	QXmlStreamAttributes attributes = xml.attributes();

	if (attributes.hasAttribute("name"))
	{
		QString name = Utility::validFileSystemString(attributes.value("name").toString());
		_mediaDirectory = _videoDirectory + "/" + name;
		_thumbnailDirectory = _mediaDirectory + "/thumbnails";

		QDir mediaDir(_mediaDirectory);
		if (!mediaDir.exists())
        {
			if (!createDirectory(_mediaDirectory))
                return false;
        }

		QDir thumbDir(_thumbnailDirectory);
		if (!thumbDir.exists())
        {
            if (!createDirectory(_thumbnailDirectory))
                return false;
        }
	}
	else
	{
		QDir thumbDir(_thumbnailDirectory);
		if (!thumbDir.exists())
        {
            if (!createDirectory(_thumbnailDirectory))
                return false;
        }
	}

    return true;
}

bool TreeModel::createDirectory(const QString &dirPath)
{
    QDir dir;
    bool success = dir.mkpath(dirPath);
    if (!success)
        QMessageBox::critical(QApplication::activeWindow(), Utility::applicationName(), tr("Unable to create folder %1.").arg(dirPath), QMessageBox::Ok);

    return success;
}

CategoryNode* TreeModel::parseCategory(QXmlStreamReader& xml, CategoryNode *parent)
{
    CategoryNode *node = new CategoryNode(parent);

	QXmlStreamAttributes attributes = xml.attributes();

	if (attributes.hasAttribute("name")) 
		node->setName(attributes.value("name").toString());

	return node;
}

void TreeModel::parseVideoRef(QXmlStreamReader& xml, CategoryNode *node)
{
	QXmlStreamAttributes attributes = xml.attributes();

    if (attributes.hasAttribute("ref"))
	{	
        QString ref = attributes.value("ref").toString();
		_refList.append(QPair<CategoryNode*, QString>(node, ref));
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
        video->setThumbnailUrl(attributes.value("thumbnailUrl").toString());

    if (attributes.hasAttribute("videoUrl"))
        video->setVideoUrl(attributes.value("videoUrl").toString());

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
    }
}

QModelIndex TreeModel::index(CategoryNode *node) const
{
    return createIndex(node->row(), 0, node);
}

CategoryNode * TreeModel::category(const QModelIndex &index) const
{
	return static_cast<CategoryNode*>(index.internalPointer());
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	CategoryNode *parentNode;

	if (!parent.isValid())
        parentNode = _rootCategory;
	else
		parentNode = static_cast<CategoryNode*>(parent.internalPointer());

    CategoryNode *childNode = parentNode->categories().at(row);
	if (childNode)
		return createIndex(row, column, childNode);
	else
		return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index ) const
{
	if (!index.isValid())
		return QModelIndex();

	CategoryNode *childNode = static_cast<CategoryNode*>(index.internalPointer());
	CategoryNode *parentNode = childNode->parent();

    if (parentNode == _rootCategory || parentNode == 0)
		return QModelIndex();

	return createIndex(parentNode->row(), 0, parentNode);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
	CategoryNode *parentNode;
	if (parent.column() > 0)
		return 0;

	if (!parent.isValid())
        parentNode = _rootCategory;
	else
		parentNode = static_cast<CategoryNode*>(parent.internalPointer());

	if (parentNode)
        return parentNode->categories().size();

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

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    QVariant v;

    if (!index.isValid())
        return v;

    CategoryNode *node = static_cast<CategoryNode*>(index.internalPointer());
    if (!node)
        return v;

    if (role == Qt::DisplayRole || role == Qt::ToolTipRole)
    {
        if (index.column() == 0)
            v = QVariant(node->name());
    }
    else if (index.column() == 0 && role == Qt::DecorationRole)
    {
		v = QVariant(_folderPixmap);
    }

    return v;
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
	Q_UNUSED(index);

    Qt::ItemFlags f = Qt::ItemIsSelectable |  Qt::ItemIsEnabled; 
    return f;
}

QString TreeModel::stripUrlQuery(const QString &urlStr)
{
    QString stripStr(urlStr);
    if (stripStr.contains('?'))
        stripStr.truncate(stripStr.indexOf('?'));
    return stripStr;
}

