#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include <QObject>
#include <QVariant>
#include "textindex.h"

class TreeModel;
class ListModel;
class DownloadManager;
class MediaFile;
class QTimer;
class ProgressImageProvider;
class ThumbnailImageProvider;
class QNetworkAccessManager;

class MainController : public QObject
{
    Q_OBJECT
public:
    explicit MainController(QObject *parent = 0);
    ~MainController();

    void loadData();

    TreeModel * treeModel() const;
    ListModel * listModel() const;
    ProgressImageProvider * progressImageProvider() const;
    ThumbnailImageProvider * thumbnailImageProvider() const;

    Q_INVOKABLE void search(const QString &text);
    Q_INVOKABLE void setCategory(const QModelIndex &index);
    Q_INVOKABLE void setCategoryExport(const QModelIndex &index, int value);
    Q_INVOKABLE void clickVideoButton(const QString &url);
    Q_INVOKABLE QVariant firstIndex();

private slots:
    void downloadFinished(MediaFile *file);
    void downloadProgress(MediaFile *file, qint64 bytesReceived, qint64 bytesTotal);

private:
    QNetworkAccessManager *_networkAccessManager;
    TreeModel *_treeModel;
    ListModel *_listModel;
    TextIndex _textIndex;
    DownloadManager *_downloadManager;
    ProgressImageProvider *_progressImageProvider;
    ThumbnailImageProvider *_thumbnailImageProvider;
};

#endif // MAINCONTROLLER_H
