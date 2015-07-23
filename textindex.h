#ifndef TEXTINDEX_H
#define TEXTINDEX_H

#include <QString>
#include <QHash>
#include <QList>

class Video;
class CategoryNode;

class TextIndex
{
public:
    typedef QHash<Video*, int> ResultsHash;

public:
    TextIndex();

    void build(const QList<Video*> videos);
    void search(const QString &searchString, ResultsHash &results) const;

private:
    enum Field
    {
        TitleField,
        SummaryField,
        CategoryField
    };

    QStringList words(const QString &str) const;
    QStringList words(const QList<CategoryNode*> &categories) const;
    void addWord(const QString &word, Video* video, Field field);

    typedef QHash<QString, ResultsHash> Index;
    Index _index;
    QStringList _stopWords;
};

#endif // TEXTINDEX_H
