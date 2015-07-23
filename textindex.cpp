#include "textindex.h"
#include "video.h"
#include "categorynode.h"

#include <QRegExp>
#include <QDebug>
#include <QDateTime>

TextIndex::TextIndex()
{

}

void TextIndex::build(const QList<Video*> videos)
{
#ifdef QT_DEBUG
    QDateTime start = QDateTime::currentDateTime();
#endif

    _index.clear();

    foreach (Video *video, videos)
    {
        QStringList titleWords = words(video->title());
        foreach (QString word, titleWords)
            addWord(word, video, TitleField);

        QStringList summaryWords = words(video->summary());
        foreach (QString word, summaryWords)
            addWord(word, video, SummaryField);

        QStringList categoryWords = words(video->categories());
        foreach (QString word, categoryWords)
            addWord(word, video, CategoryField);
    }

#ifdef QT_DEBUG
    QDateTime end = QDateTime::currentDateTime();
    qDebug() << "TextIndex::build() took " << start.time().msecsTo(end.time()) << " milliseconds.";
    qDebug() << "Index has " << _index.size() << " entries.";
#endif
}

QStringList TextIndex::words(const QString &str) const
{
    return str.split(QRegExp("[\\s\\:,;\"\\(\\)#\\?!“”]"), QString::SkipEmptyParts);
}

QStringList TextIndex::words(const QList<CategoryNode*> &categories) const
{
    QStringList list;

    foreach (CategoryNode *category, categories)
    {
        list += words(category->name());

        CategoryNode *parent = category->parent();
        while (parent)
        {
            list += words(parent->name());
            parent = parent->parent();
        }
    }

    return list;
}

void TextIndex::addWord(const QString &originalWord, Video* video, Field field)
{
    if (originalWord.length() < 2)
        return;

    QString word = originalWord.toLower();

    if (_stopWords.contains(word))
        return;

    // replace apostrophe (right single quotation mark U+2019) with single quote
    // since user won't be typing this char on US keyboard
    static QChar rightSingleQuote(8217);
    if (word.contains(rightSingleQuote))
        word.replace(rightSingleQuote, '\'');

    int score = 1;
    if (field == TitleField)
        score = 10;
    else if (field == SummaryField)
        score = 2;

    if (_index.contains(word))
    {
        if (_index.value(word).contains(video))
            _index[word][video] += score;
        else
            _index[word].insert(video, score);
    }
    else
    {
        QHash<Video*, int> hash;
        hash.insert(video, score);
        _index.insert(word, hash);
    }
}

void TextIndex::search(const QString &searchString, ResultsHash &results) const
{
    results.clear();

    // clean up search string
    QString text = searchString.simplified();

    QStringList wordList = words(text);

    if (wordList.size() >= 1)
    {
        QString word = wordList.at(0).toLower();

        if (_index.contains(word))
        {
            ResultsHash hash = _index.value(word);
            for (auto i = hash.constBegin(); i != hash.constEnd(); ++i)
            {
                Video *video = i.key();
                int score = i.value();

                if (results.contains(video))
                    results[video] += score;
                else
                    results.insert(video, score);
            }
        }
    }

    if (wordList.size() > 1)
    {
        QSet<Video*> intersectSet = results.keys().toSet();

        for (int i = 1; i < wordList.count(); i++)
        {
            QString word = wordList.at(i).toLower();

            if (_index.contains(word))
            {
                QSet<Video*> videoSet;

                ResultsHash hash = _index.value(word);
                for (auto i = hash.constBegin(); i != hash.constEnd(); ++i)
                {
                    Video *video = i.key();
                    int score = i.value();

                    if (results.contains(video))
                        results[video] += score;
                    else
                        results.insert(video, score);

                    if (!videoSet.contains(video))
                        videoSet.insert(video);
                }

                intersectSet.intersect(videoSet);
            }
        }

        // bonus score for videos that match all words
        foreach (Video *video, intersectSet)
            results[video] *= 10;
    }
}



