#ifndef CATEGORY_H
#define CATEGORY_H

#include <QString>
#include <QList>
#include <QSet>

class Video;

class CategoryNode
{
public:
    CategoryNode(CategoryNode *parent);
    ~CategoryNode();

    CategoryNode * parent() const;
    QString name() const;
    void setName(const QString &name);
    int row() const;
    const QList<CategoryNode*> & categories() const;
    const QList<Video*> & videos() const;
    QSet<Video*> allVideos() const;

    void addCategory(CategoryNode *category);
    void addVideo(Video *video);

private:
    CategoryNode *_parent;
    QString _name;
    QList<CategoryNode*> _categoryList;
    QList<Video*> _videoList;
};

#endif // CATEGORY_H
