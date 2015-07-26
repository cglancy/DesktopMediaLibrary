#ifndef MEDIAFILE_H
#define MEDIAFILE_H

#include <QString>

class MediaFile
{
public:
    enum Type
    {
        VideoType,
        AudioType
    };

    enum State
    {
        NotDownloadedState,
        DownloadErrorState,
        DownloadingState,
        DownloadingPausedState,
        DownloadedState
    };

public:
    MediaFile(Type type);
    virtual ~MediaFile();

    Type type() const;

    QString url() const;
    void setUrl(const QString &url);

    State state() const;
    void setState(State state);

    QString downloadError() const;
    void setDownloadError(const QString &error);

    QString localFilePath() const;
    QString localDownloadPath() const;

    qint64 fileSize() const;
    void setFileSize(qint64 sizeBytes);

    qint64 bytesReceived() const;
    void setBytesReceived(qint64 bytesReceived);

    qint64 bytesResumed() const;
    void setBytesResumed(qint64 bytesResumed);

    int percentComplete() const;

private:
    Type _type;
    QString _url;
    State _state;
    QString _downloadError;
    qint64 _size, _bytesReceived, _bytesResumed;
};

#endif // MEDIAFILE_H
