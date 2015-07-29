#include "categorynode.h"

CategoryNode::CategoryNode(CategoryNode *parent)
    : _parent(parent),
      _exportState(NoState)
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

CategoryNode::ExportState CategoryNode::exportState() const
{
    return _exportState;
}

void CategoryNode::setExport(bool exportState)
{
    // 1. Set all child states equal to parent export state.
    propogateExportDown(exportState);

    // 2. Update states for all ancestors.
    if (_parent)
        _parent->propogateExportUp(_exportState);
}

void CategoryNode::propogateExportDown(bool exportState)
{
    _exportState = exportState ? YesState: NoState;

    foreach (CategoryNode *category, _categoryList)
        category->propogateExportDown(exportState);
}

void CategoryNode::propogateExportUp(ExportState childState)
{
    if (childState == MixedState)
    {
        _exportState = MixedState;
    }
    else
    {
        int yesCount = 0;
        int noCount = 0;

        foreach (CategoryNode *category, _categoryList)
        {
            ExportState childState = category->exportState();

            if (childState == YesState)
                yesCount++;
            else if (childState == NoState)
                noCount++;
        }

        if (yesCount == _categoryList.count())
            _exportState = YesState;
        else if (noCount == _categoryList.count())
            _exportState = NoState;
        else
            _exportState = MixedState;
    }

    if (_parent)
        _parent->propogateExportUp(_exportState);
}

