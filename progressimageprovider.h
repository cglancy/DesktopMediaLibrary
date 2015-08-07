#ifndef PROGRESSIMAGEPROVIDER_H
#define PROGRESSIMAGEPROVIDER_H

#include <QQuickImageProvider>
#include <QHash>
#include <QMap>
#include "mediafile.h"

class ProgressImageProvider : public QQuickImageProvider
{
public:
    ProgressImageProvider();
    ~ProgressImageProvider();

    static void initMaps();
    static QString imageName(MediaFile::State state, int progress = 0);
    static QString progressString(int progress);
    static void progressAndState(const QString &imageName, MediaFile::State &state, int &progress);

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);

private:
    QPixmap createPixmap(const QSize &size, MediaFile::State state, int progress);
    static const QString NotDownloadedName;
    static const QString DownloadedName;
    static const QString DownloadingName;
    static const QString DownloadingPausedName;
    static const QString DownloadErrorName;

    static QMap<MediaFile::State, QString> _nameMap;
    static QMap<QString, MediaFile::State> _stateMap;
};

#endif // PROGRESSIMAGEPROVIDER_H
