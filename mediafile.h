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

    int progress() const;
    void setProgress(int percent);

    QString downloadError() const;
    void setDownloadError(const QString &error);

    QString localFilePath() const;
    QString localDownloadPath() const;

private:
    Type _type;
    QString _url;
    State _state;
    int _progress;
    QString _downloadError;
};

#endif // MEDIAFILE_H
