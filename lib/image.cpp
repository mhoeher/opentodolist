#include "image.h"

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
  m_image()
{
  initializeItem();
}

/**
   @brief Set the image path.
 */
void Image::setImage(const QString &image)
{
  if (m_image != image) {
    m_image = image;
    emit imageChanged();
    saveItem();
  }
}
