/*
 * Copyright 2020 Martin Hoeher <martin@rpdev.net>
 +
 * This file is part of OpenTodoList.
 *
 * OpenTodoList is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * OpenTodoList is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenTodoList.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "image.h"

#include <QFuture>
#include <QFutureWatcher>
#include <QLoggingCategory>
#include <QMimeDatabase>
#include <QSaveFile>
#include <QtConcurrent>

static Q_LOGGING_CATEGORY(log, "OpenTodoList.Image", QtDebugMsg);

/**
 * @brief Constructor.
 */
Image::Image(const QString& filename, QObject* parent) : TopLevelItem(filename, parent), m_image()
{
    connect(this, &Image::imageChanged, this, &ComplexItem::changed);
}

/**
 * @brief Constructor.
 */
Image::Image(QObject* parent) : Image(QString(), parent) {}

/**
 * @brief Constructor.
 */
Image::Image(const QDir& dir, QObject* parent) : TopLevelItem(dir, parent), m_image()
{
    connect(this, &Image::imageChanged, this, &ComplexItem::changed);
}

/**
 * @brief Destructor.
 */
Image::~Image() {}

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
void Image::setImage(const QString& image)
{
    if (m_image != image) {
        if (!isValid()) {
            // @todo Probably related to https://gitlab.com/rpdev/opentodolist/issues/202
            m_image = image;
            emit imageChanged();
        } else {
            QFileInfo fi(image);
            if (fi.isRelative() && !image.startsWith("content://")) {
                m_image = image;
                emit imageChanged();
            } else {
                if (fi.absolutePath() == directory()) {
                    m_image = fi.fileName();
                    emit imageChanged();
                } else if (isValid()) {
                    if (!fi.exists()) {
                        return;
                    }

                    // 1. Remove existing image (if any):
                    QFile file(directory() + "/" + m_image);
                    if (file.exists() && !m_image.isEmpty()) {
                        // Empty image would yield path to folder, which cannot
                        // be deleted via QFile (which is what we want).
                        if (!file.remove()) {
                            qCWarning(log) << "Failed to remove existing image "
                                              "file"
                                           << file.fileName() << ":" << file.errorString();
                        }
                    }

                    // 2. Create folder to put image into (if needed):
                    {
                        QDir dir(directory());
                        if (!dir.exists()) {
                            if (!dir.mkpath(".")) {
                                qCWarning(log) << "Failed to created item "
                                                  "directory"
                                               << dir.absolutePath();
                            }
                        }
                    }

                    // 3. Copy over the external file:
                    QString targetFileName = QUuid::createUuid().toString() + ".res.";
                    auto completeSuffix = fi.completeSuffix();
                    if (completeSuffix.contains("?")) {
                        // On some platforms (e.g. iOS), we get a URL with
                        // additional parameters (i.e. ?key=val). Keep only
                        // the suffix and remove everything after the first
                        // question mark character:
                        completeSuffix = completeSuffix.split("?").first();
                    }
                    if (completeSuffix.isEmpty()) {
                        // On some platforms (e.g. Android) we get URLs. These
                        // can be consumed by Qt, but we won't get any useful information
                        // from the URL itself. Hence, try to derive the information from
                        // the file directly:
                        QMimeDatabase mdb;
                        auto mimetype = mdb.mimeTypeForFile(image);
                        if (mimetype.isValid()) {
                            completeSuffix = "." + mimetype.preferredSuffix();
                        }
                    }
                    targetFileName += completeSuffix;
                    auto targetFilePath = directory() + "/" + targetFileName;

                    // First, try to copy the file as is:
                    if (!QFile::copy(image, targetFilePath)) { // NOLINT
                        // Simple copy did not work, manually copy the file:
                        QFile src(image);
                        if (src.open(QIODevice::ReadOnly)) {
                            QSaveFile dst(targetFilePath);
                            if (dst.open(QIODevice::WriteOnly)) {
                                while (!src.atEnd()) {
                                    dst.write(src.read(1024));
                                }
                                dst.commit();
                            } else {
                                qCWarning(log) << "Failed to open" << targetFilePath
                                               << "for writing:" << dst.errorString();
                            }
                            src.close();
                        } else {
                            qCWarning(log) << "Failed to open" << image
                                           << "for reading:" << src.errorString();
                        }
                    }

                    m_image = targetFileName;
                    emit imageChanged();
                }
            }
        }
    }
}

void Image::setImageUrl(const QUrl& imageUrl)
{
    if (imageUrl.scheme() == "content") {
        // On Android, pass-thru the content:// URL as is, see
        // https://www.volkerkrause.eu/2019/02/16/qt-open-files-on-android.html
        setImage(imageUrl.toString());
    } else {
        setImage(imageUrl.toLocalFile());
    }
}

/**
 * @brief The URL of the image of the item.
 */
QUrl Image::imageUrl() const
{
    if (m_image.isEmpty()) {
        return QUrl();
    } else {
        return QUrl::fromLocalFile(directory() + "/" + m_image);
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

Item* Image::copyTo(const QDir& targetDirectory, const QUuid& targetLibraryUuid,
                    const QUuid& targetItemUid)
{
    auto result = TopLevelItem::copyTo(targetDirectory, targetLibraryUuid, targetItemUid);
    auto image = qobject_cast<Image*>(result);
    if (image) {
        QFileInfo fi(imageUrl().toLocalFile());
        image->m_image.clear();
        if (fi.exists()) {
            auto targetImageFileName =
                    QUuid::createUuid().toString() + ".res." + fi.completeSuffix();
            auto targetImageFilePath = targetDirectory.absoluteFilePath(targetImageFileName);
            if (QFile::copy(fi.absoluteFilePath(), targetImageFilePath)) { // NOLINT
                image->m_image = targetImageFileName;
            }
        }
    }
    return result;
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
