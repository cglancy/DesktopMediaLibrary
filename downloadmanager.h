#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QHash>
#include <QNetworkReply>

class QNetworkAccessManager;
class QFile;
class MediaFile;
class Video;

class DownloadManager : public QObject
{
	Q_OBJECT
public:
    DownloadManager(QNetworkAccessManager *nam, QObject *parent = 0);
    ~DownloadManager();

    bool isDownloading() const;

    void getFileSize(MediaFile *file);
    void downloadFile(MediaFile *file);
    void cancelFileDownload(MediaFile *file);
    
signals:
    void fileSizeFinished(MediaFile *file);
    void fileFinished(MediaFile *file);
    void fileProgress(MediaFile *file, qint64 bytesReceived, qint64 bytesTotal);

public slots:
    void cancelAllDownloads();

private slots:
    void readyRead();
    void progress(qint64 bytesReceived, qint64 bytesTotal);
    void finished();
    void sizeFinished();
    bool proceedWithSizeRequest(MediaFile *file);
	void error(QNetworkReply::NetworkError code);

private:
    struct FileData
    {
        FileData() : mediaFile(0), file(0) {}
        FileData(MediaFile *mf, QFile *f) : mediaFile(mf), file(f) {}
        MediaFile *mediaFile;
        QFile *file;
    };

private:
    QNetworkAccessManager *_networkAccessManager;
    QHash<QNetworkReply*, FileData> _fileHash;
    QHash<QNetworkReply*, MediaFile*> _sizeHash;
};

#endif // DOWNLOADMANAGER_H
