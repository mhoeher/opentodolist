#include "image.h"

#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent>

/**
 * @brief Constructor.
 */
Image::Image(const QString &filename, QObject *parent) :
    TopLevelItem(filename, parent),
    m_image()
{
    connect(this, &Image::imageChanged, this, &ComplexItem::changed);
}

/**
 * @brief Constructor.
 */
Image::Image(QObject* parent) : Image(QString(), parent)
{
}

/**
 * @brief Constructor.
 */
Image::Image(const QDir& dir, QObject* parent) : TopLevelItem(dir, parent),
    m_image()
{
    connect(this, &Image::imageChanged, this, &ComplexItem::changed);
}

/**
 * @brief Destructor.
 */
Image::~Image()
{
}

/**
 * @brief Set the image path.
 *
 * This sets the image path and also triggers copying of the image is required. The behavior
 * of this function is as follows:
 *
 * - If a relative path is passed in, the image property is set to this path and nothing else
 *   happens.
 * - If an absolute path is passed in, then first a check is done whether the image is already
 *   inside the item's data directory. If this is the case, then the item's image property is
 *   set to that (relative) image path. If the path points to some file outside the
 *   image's data directory, then a copy operation of that file to the image's data directory is
 *   triggered. This operation runs in the background. As soon as the copying finished, the
 *   item's image property is set to the new image's location.
 */
void Image::setImage(const QString &image)
{
    if (m_image != image) {
        QFileInfo fi(image);
        if (fi.isRelative()) {
            m_image = image;
            emit imageChanged();
            save();
        } else {
            if (fi.absolutePath() == directory()) {
                m_image = fi.fileName();
                emit imageChanged();
                save();
            } else if (isValid()) {
                if (!fi.exists()) {
                    return;
                }
                QFile file(directory() + "/" + m_image);
                file.remove();
                QString targetFileName = QUuid::createUuid().toString() + ".res." + fi.completeSuffix();
                QFile::copy(image, directory() + "/" + targetFileName);
                m_image = targetFileName;
                emit imageChanged();
                save();
            }
        }
    }
}

/**
 * @brief Returns whether the image points to a valid file
 *
 * The validImage property is true if the image property points to an existing file
 * inside the item's data directory.
 */
bool Image::validImage() const
{
    return isValid() && QFile(directory() + "/" + m_image).exists();
}

QVariantMap Image::toMap() const
{
    auto result = TopLevelItem::toMap();
    result["image"] = m_image;
    return result;
}

void Image::fromMap(QVariantMap map)
{
    TopLevelItem::fromMap(map);
    setImage(map.value("image", m_image).toString());
}

bool Image::deleteItem()
{
    if (validImage()) {
        QFile(directory() + "/" + m_image).remove();
    }
    return TopLevelItem::deleteItem();
}
