#include "image.h"

#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent>


/**
   @brief The item type used by the Image class.
 */
const QString Image::ItemType = "Image";

/**
  @brief The list of persistent properties of the Image class.
  */
const QStringList Image::PersistentProperties = {"image"};

/**
   @brief Constructor.
 */
Image::Image(const QString &directory, QObject *parent) : 
  TopLevelItem(false, directory, ItemType, PersistentProperties, parent),
  m_image(),
  m_busy(false)
{
  initializeItem();
}

/**
   @brief Set the image path.
   
   This sets the image path and also triggers copying of the image is required. The behavior
   of this function is as follows:
   
   - If a relative path is passed in, the image property is set to this path and nothing else
     happens.
   - If an absolute path is passed in, then first a check is done whether the image is already
     inside the item's data directory. If this is the case, then the item's image property is
     set to that (relative) image path. If the path points to some file outside the
     image's data directory, then a copy operation of that file to the image's data directory is
     triggered. This operation runs in the background. As soon as the copying finished, the
     item's image property is set to the new image's location.
 */
void Image::setImage(const QString &image)
{
  if (m_image != image) {
    QFileInfo fi(image);
    if (fi.isRelative()) {
      m_image = image;
      emit imageChanged();
      saveItem();
    } else {
      if (fi.absolutePath() == directory()) {
        m_image = fi.fileName();
        emit imageChanged();
        saveItem();
      } else {
        if (!fi.exists()) {
          return;
        }
        setBusy(true);
        QString localFileName = findLocalFileName(image);
        QFuture<void> copy = QtConcurrent::run(Image::copyImage, image, directory() + "/" + localFileName);
        QtConcurrent::run(Image::removeImage, directory() + "/" + m_image);
        auto *copyWatcher = new QFutureWatcher<void>(this);
        connect(copyWatcher, &QFutureWatcher<void>::finished, [this,localFileName]() {
          this->m_image = localFileName;
          emit this->imageChanged();
          this->saveItem();
          this->setBusy(false);
        });
        copyWatcher->setFuture(copy);
      }
    }
  }
}

/**
   @brief Returns whether the image points to a valid file
   
   The validImage property is true if the image property points to an existing file
   inside the item's data directory.
 */
bool Image::validImage() const
{
  return QFile(directory() + "/" + m_image).exists();
}

void Image::setBusy(bool busy)
{
  m_busy = busy;
  emit busyChanged();
}

QString Image::findLocalFileName(const QString &image) const
{
  QFileInfo fi(image);
  QString localFileName = fi.fileName();
  int i = 0;
  while (QFile::exists(directory() + "/" + localFileName)) {
    localFileName = fi.completeBaseName() + " - " + QString::number(++i) + fi.suffix();
  }
  return localFileName;
}

void Image::copyImage(const QString &imgFrom, const QString &imgTo)
{
  QFile file(imgFrom);
  if (!file.copy(imgTo)) {
    qWarning().noquote().nospace()
        << "Failed to copy image from " << imgFrom << " to " << imgTo << ": " << file.errorString();
  }
}

void Image::removeImage(const QString &image)
{
  QFile file(image);
  if (!file.remove()) {
    qWarning().noquote().nospace() 
        << "Failed to remove image " << image << ": " << file.errorString();
  }
}
