#include "progressimageprovider.h"

#include <QPainter>
#include <QPainterPath>
#include <QDebug>

const QString ProgressImageProvider::NotDownloadedName      = "notDownloaded";
const QString ProgressImageProvider::DownloadedName         = "downloaded";
const QString ProgressImageProvider::DownloadingName        = "downloading";
const QString ProgressImageProvider::DownloadingPausedName  = "downloadingPaused";
const QString ProgressImageProvider::DownloadErrorName      = "downloadError";

QMap<MediaFile::State, QString> ProgressImageProvider::_nameMap;
QMap<QString, MediaFile::State> ProgressImageProvider::_stateMap;

ProgressImageProvider::ProgressImageProvider()
    : QQuickImageProvider(QQmlImageProviderBase::Pixmap)
{

}

ProgressImageProvider::~ProgressImageProvider()
{

}

void ProgressImageProvider::initMaps()
{
    if (_nameMap.size() == 0)
    {
        _nameMap[MediaFile::NotDownloadedState] = NotDownloadedName;
        _nameMap[MediaFile::DownloadedState] = DownloadedName;
        _nameMap[MediaFile::DownloadErrorState] = DownloadErrorName;
        _nameMap[MediaFile::DownloadingState] = DownloadingName;
        _nameMap[MediaFile::DownloadingPausedState] = DownloadingPausedName;

        _stateMap[NotDownloadedName] = MediaFile::NotDownloadedState;
        _stateMap[DownloadedName] = MediaFile::DownloadedState;
        _stateMap[DownloadErrorName] = MediaFile::DownloadErrorState;
        _stateMap[DownloadingName] = MediaFile::DownloadingState;
        _stateMap[DownloadingPausedName] = MediaFile::DownloadingPausedState;
    }
}

QString ProgressImageProvider::imageName(MediaFile::State state, int progress)
{
    initMaps();

    QString name = _nameMap.value(state);

    if (state == MediaFile::DownloadingState ||
        state == MediaFile::DownloadingPausedState)
        name += "-" + progressString(progress);

    return name;
}

QString ProgressImageProvider::progressString(int progress)
{
    QString str = "0";
    if (progress >= 0 && progress <= 100)
        str = QString::number(progress);
    return str;
}

void ProgressImageProvider::progressAndState(const QString &imageName, MediaFile::State &state, int &progress)
{
    initMaps();

    state = MediaFile::NotDownloadedState;
    progress = 0;

    QStringList list = imageName.split('-');
    if (list.size() < 1)
        return;

    state = _stateMap.value(list.at(0));

    if (list.size() == 2)
        progress = list.at(1).toInt();
}

QPixmap ProgressImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(size);

    MediaFile::State state;
    int progress = 0;
    progressAndState(id, state, progress);

    return createPixmap(requestedSize, state, progress);
}

QPixmap ProgressImageProvider::createPixmap(const QSize &size, MediaFile::State state, int progress)
{
    Q_UNUSED(size);

    int width = 24;
    int height = 24;

    QPixmap pixmap(width, height);
    pixmap.fill(Qt::transparent);

    QPainter p(&pixmap);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::black);
    p.drawEllipse(1, 1, 22, 22);

    if (state == MediaFile::NotDownloadedState)
    {
        p.setRenderHint(QPainter::Antialiasing, false);
        // download arrow
        QPainterPath path;
        path.moveTo(8, 7);
        path.lineTo(15, 7);
        path.lineTo(15, 11);
        path.lineTo(18, 11);
        path.lineTo(12, 17);
        path.lineTo(11, 17);
        path.lineTo(5, 11);
        path.lineTo(8, 11);
        path.closeSubpath();
        p.drawPath(path);
    }
    else if (state == MediaFile::DownloadingPausedState)
    {
        p.setRenderHint(QPainter::Antialiasing, false);
        // download arrow
        QPainterPath path;
        path.moveTo(8, 7);
        path.lineTo(15, 7);
        path.lineTo(15, 11);
        path.lineTo(18, 11);
        path.lineTo(12, 17);
        path.lineTo(11, 17);
        path.lineTo(5, 11);
        path.lineTo(8, 11);
        path.closeSubpath();
        p.fillPath(path, Qt::black);

        p.setRenderHint(QPainter::Antialiasing, true);
        int degree = progress * 3.6;

        int startAngle = 90 * 16;
        int spanAngle = -degree * 16;

        QPen arcPen(Qt::black, 3.0);
        p.setPen(arcPen);
        QRect rect(2, 2, 20, 20);
        p.drawArc(rect, startAngle, spanAngle);
    }
    else if (state == MediaFile::DownloadingState)
    {
        p.fillRect(8, 8, 8, 8, Qt::black);

        int degree = progress * 3.6;

        int startAngle = 90 * 16;
        int spanAngle = -degree * 16;

        QPen arcPen(Qt::black, 3.0);
        p.setPen(arcPen);
        QRect rect(2, 2, 20, 20);
        p.drawArc(rect, startAngle, spanAngle);
    }
    else if (state == MediaFile::DownloadedState)
    {
        QPainterPath path;
        path.moveTo(10, 5);
        path.lineTo(16, 11);
        path.lineTo(16, 12);
        path.lineTo(10, 18);
        path.closeSubpath();
        p.fillPath(path, Qt::black);
    }

    return pixmap;
}

