#include "categorynode.h"

CategoryNode::CategoryNode(CategoryNode *parent)
    : _parent(parent)
{
    if (parent)
        parent->addCategory(this);
}

CategoryNode::~CategoryNode()
{
    qDeleteAll(_categoryList);
}

CategoryNode * CategoryNode::parent() const
{
    return _parent;
}

QString CategoryNode::name() const
{
    return _name;
}

void CategoryNode::setName(const QString &name)
{
    _name = name;
}

int CategoryNode::row() const
{
    int row = -1;

    if (_parent)
        row = _parent->_categoryList.indexOf(const_cast<CategoryNode *const>(this));

    return row;
}

void CategoryNode::addCategory(CategoryNode *category)
{
    _categoryList.append(category);
}

void CategoryNode::addVideo(Video *video)
{
    _videoList.append(video);
}

const QList<CategoryNode*> & CategoryNode::categories() const
{
    return _categoryList;
}

const QList<Video*> & CategoryNode::videos() const
{
    return _videoList;
}

QSet<Video*> CategoryNode::allVideos() const
{
    QSet<Video*> videoSet;

    foreach (Video *video, _videoList)
        videoSet.insert(video);

    foreach (CategoryNode *category, _categoryList)
    {
        QSet<Video*> categorySet = category->allVideos();
        videoSet.unite(categorySet);
    }

    return videoSet;
}


