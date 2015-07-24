#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QObject>
#include <QHash>
#include <QNetworkReply>

class QNetworkAccessManager;
class QFile;

class FileManager : public QObject
{
	Q_OBJECT
public:
    enum FileState
    {
        NotDownloadedState,
        DownloadingState,
        DownloadingPausedState,
        DownloadedState
    };

public:
    FileManager(QObject *parent);
    ~FileManager();

    QString fileDirectory() const;
    void setFileDirectory(const QString &dir);

    bool isDownloading() const;

    FileState fileState(const QString &url) const;
    QString localFilePath(const QString &url) const;

    void downloadFile(const QString &url);
    void cancelDownload(const QString &url);
    
signals:
    void fileDownloaded(const QString &url);
    void fileDownloadProgress(const QString &url, int progress);

public slots:
    void cancelAllDownloads();

private slots:
    void readyRead();
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void downloadFinished();
	void error(QNetworkReply::NetworkError code);

private:
    QString localDownloadPath(const QString &url) const;
    bool createPath(const QString &path);

private:
    struct DownloadData
    {
        DownloadData() : reply(0), file(0) {}
        DownloadData(QNetworkReply *r, QFile *f) : reply(r), file(f) {}
        QNetworkReply *reply;
        QFile *file;
    };

private:
    QString _fileDirectory;
    QNetworkAccessManager *_networkAccessManager;
    QHash<QString, DownloadData> _downloadHash;
};

#endif // FILEMANAGER_H
