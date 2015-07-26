#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QHash>
#include <QNetworkReply>

class QNetworkAccessManager;
class QFile;
class MediaFile;

class DownloadManager : public QObject
{
	Q_OBJECT
public:
    DownloadManager(QObject *parent);
    ~DownloadManager();

    bool isDownloading() const;

    void downloadFile(MediaFile *file);
    void cancelDownload(MediaFile *file);
    
signals:
    void downloadFinished(MediaFile *file);
    void downloadProgress(MediaFile *file, qint64 bytesReceived, qint64 bytesTotal);

public slots:
    void cancelAllDownloads();

private slots:
    void readyRead();
    void progress(qint64 bytesReceived, qint64 bytesTotal);
    void finished();
	void error(QNetworkReply::NetworkError code);

private:
    bool createPath(const QString &path);

private:
    struct DownloadData
    {
        DownloadData() : mediaFile(0), file(0) {}
        DownloadData(MediaFile *mf, QFile *f) : mediaFile(mf), file(f) {}
        MediaFile *mediaFile;
        QFile *file;
    };

private:
    QNetworkAccessManager *_networkAccessManager;
    QHash<QNetworkReply*, DownloadData> _downloadHash;
};

#endif // DOWNLOADMANAGER_H
