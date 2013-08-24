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
        } else if ( type == "polygon" ) {
            renderPolygon( &result, path.join( "/" ) );
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

void ImageProvider::renderPolygon(QPixmap *pixmap, QString params)
{
    pixmap->fill( Qt::transparent );
    QString color = "transparent";
    QString fillColor = "transparent";
    QVector<QPoint> points;

    QRegExp argColor( "color=([^,]*)" ),
            argFillColor( "fill=([^,]*)" ),
            argPoints( "points=([^,]*)");

    if ( argColor.indexIn( params ) >= 0 ) {
        color = argColor.cap( 1 );
    }
    if ( argFillColor.indexIn( params ) >= 0 ) {
        fillColor = argFillColor.cap( 1 );
    }

    int xMin = -1,
        xMax = -1,
        yMin = -1,
        yMax = -1;
    if ( argPoints.indexIn( params ) >= 0 ) {
        QStringList tuples = argPoints.cap( 1 ).split( "-" );
        foreach ( QString tuple, tuples ) {
            QStringList data = tuple.split( ":" );
            if ( data.size() == 2 ) {
                int x = data[0].toInt(),
                    y = data[1].toInt();
                points << QPoint( x, y );
                xMin = xMin < 0 ? x : qMin( xMin, x );
                xMax = xMax < 0 ? x : qMax( xMax, x );
                yMin = yMin < 0 ? y : qMin( yMin, y );
                yMax = yMax < 0 ? y : qMax( yMax, y );
            }
        }
    }

    int w = xMax - xMin;
    int h = yMax - yMin;
    if ( w > 0 && h > 0 ) {
        for ( int i = 0; i < points.size(); ++i ) {
            QPoint& point = points[ i ];
            point.setX( ( point.x() - xMin ) * pixmap->width() / w );
            point.setY( ( point.y() - yMin ) * pixmap->height() / h );
        }
    }

    if ( !points.isEmpty() ) {
        QPainter painter( pixmap );
        QPen pen( QColor( color ), 1 );
        painter.setPen( pen );
        QBrush brush( QColor( fillColor ), Qt::SolidPattern );
        painter.setBrush( brush );
        painter.drawPolygon( points.data(), points.size() );
    }
}
