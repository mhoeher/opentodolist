#include "imageprovider.h"

#include <QPainter>
#include <QRegExp>

ImageProvider::ImageProvider() :
    QQuickImageProvider(QQuickImageProvider::Pixmap)
{
}

QPixmap ImageProvider::requestPixmap(const QString &id, QSize *size,
                                     const QSize &requestedSize)
{
    static int defaultWidth = 100;
    static int defaultHeight = 100;

    int width = requestedSize.width() > 0 ?
                requestedSize.width() : defaultWidth;
    int height = requestedSize.height() > 0 ?
                requestedSize.height() : defaultHeight;
    if ( size ) {
        *size = QSize( width, height );
    }

    QPixmap result( width, height );

    QStringList path = id.split( "/" );
    if ( path.size() > 0 ) {
        QString type = path.first();
        path.pop_front();
        if ( type == "pie" ) {
            renderPie( &result, path.join("/") );
        }
    }

    return result;
}

void ImageProvider::renderPie(QPixmap *pixmap, QString params)
{
    pixmap->fill( Qt::transparent );
    int percentage = 100;
    QString color = "transparent";
    QString fillColor = "transparent";
    QRegExp argPercentage( "percentage=([^,]*)" );
    QRegExp argColor( "color=([^,]*)" );
    QRegExp argFillColor( "fill=([^,]*)" );
    if ( argPercentage.indexIn( params ) >= 0 ) {
        percentage = argPercentage.cap( 1 ).toInt( 0, 0 );
    }
    if ( argColor.indexIn( params ) >= 0 ) {
        color = argColor.cap( 1 );
    }
    if ( argFillColor.indexIn( params ) >= 0 ) {
        fillColor = argFillColor.cap( 1 );
    }
    QPainter painter( pixmap );
    painter.setRenderHints( QPainter::Antialiasing, true );
    QPen pen( QColor( color ), 1 );
    painter.setPen( pen );
    painter.setBrush( QColor( fillColor ) );
    QRectF rectangle(0, 0, pixmap->width(), pixmap->height());
    int spanAngle = 360 * 16 * percentage / 100;
    int startAngle = 90 * 16 - spanAngle;
    painter.drawPie( rectangle, startAngle, spanAngle );
}
