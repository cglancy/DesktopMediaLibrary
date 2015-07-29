#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <QObject>
#include "textindex.h"

class TreeModel;
class ListModel;
class DownloadManager;
class MediaFile;
class QTimer;

class MainController : public QObject
{
    Q_OBJECT
public:
    explicit MainController(QObject *parent = 0);

    void loadData();

    TreeModel * treeModel() const;
    ListModel * listModel() const;

    Q_INVOKABLE void search(const QString &text);
    Q_INVOKABLE void setCategory(const QModelIndex &index);
    Q_INVOKABLE void setCategoryExport(const QModelIndex &index, int value);
    Q_INVOKABLE void clickVideoButton(const QString &url);

private slots:
    void downloadFinished(MediaFile *file);
    void downloadProgress(MediaFile *file, qint64 bytesReceived, qint64 bytesTotal);
    void updateTree();

private:
    TreeModel *_treeModel;
    ListModel *_listModel;
    TextIndex _textIndex;
    DownloadManager *_fileManager;
    QTimer *_timer;
};

#endif // MAINCONTROLLER_H
