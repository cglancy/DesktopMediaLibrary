#ifndef THUMBNAILIMAGEPROVIDER_H
#define THUMBNAILIMAGEPROVIDER_H

#include <QQuickImageProvider>

class QNetworkAccessManager;
class TreeModel;
class ImageFile;

class ThumbnailImageProvider : public QQuickImageProvider
{
public:
    ThumbnailImageProvider(QNetworkAccessManager *nam, TreeModel *treeModel);
    ~ThumbnailImageProvider();

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);

private:
    QImage downloadFile(ImageFile *file);
    static bool saveImage(QImage image, QString path);

private:
    QNetworkAccessManager *_networkAccessManager;
    TreeModel *_treeModel;
};

#endif // THUMBNAILIMAGEPROVIDER_H
