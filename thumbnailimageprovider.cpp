#include "thumbnailimageprovider.h"
#include "treemodel.h"
#include "video.h"
#include "imagefile.h"
#include "utility.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QEventLoop>
#include <QtConcurrent>
#include <QDebug>


ThumbnailImageProvider::ThumbnailImageProvider(QNetworkAccessManager *nam, TreeModel *treeModel)
    : QQuickImageProvider(QQmlImageProviderBase::Image),
      _networkAccessManager(nam),
      _treeModel(treeModel)
{
}

ThumbnailImageProvider::~ThumbnailImageProvider()
{
}


QImage ThumbnailImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    QImage image;

    Q_UNUSED(size);
    Q_UNUSED(requestedSize);

    if (!_treeModel)
        return image;

    qDebug() << "ThumbnailImageProvider::requestImage(" << id << ")";

    Video *video = _treeModel->video(id);
    if (!video)
        return image;

    ImageFile *file = video->thumbnailFile();
    if (!file)
        return image;

    if (QFile::exists(file->localFilePath()))
        image = QImage(file->localFilePath());
    else
        image = downloadFile(file);

    return image;
}

QImage ThumbnailImageProvider::downloadFile(ImageFile *file)
{
    QImage image;
    QNetworkRequest request;
    request.setUrl(QUrl(file->url()));

    qDebug() << "downloading thumbnail " << file->url();

    QNetworkReply *reply = _networkAccessManager->get(request);
    QEventLoop eventLoop;
    QObject::connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec();

    if (reply->error() == QNetworkReply::NoError)
    {
        qDebug() << "downloaded thumbnail " << file->url();

        QByteArray data = reply->readAll();
        image.loadFromData(data);
#if 1
        QtConcurrent::run(saveImage, image, file->localFilePath());
#else
        QString path = file->localFilePath();
        Utility::createPath(path);
        image.save(path);
#endif
    }

    return image;
}

bool ThumbnailImageProvider::saveImage(QImage image, QString path)
{
    Utility::createPath(path);
    bool saved = image.save(path);

    if (saved)
        qDebug() << "thumbnail saved as " << path;
    else
        qDebug() << "Error in ThumbnailImageProvider::saveImage()";

    return saved;
}
