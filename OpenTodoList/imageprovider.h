#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QQuickImageProvider>

class ImageProvider : public QQuickImageProvider
{
public:
    explicit ImageProvider();

    QPixmap requestPixmap( const QString& id, QSize* size,
                           const QSize& requestedSize );
    
private:

    static void renderPie( QPixmap* pixmap, QString params );
    static void renderPolygon( QPixmap* pixmap, QString params );
    
};

#endif // IMAGEPROVIDER_H
