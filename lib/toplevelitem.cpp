#include "toplevelitem.h"

#include <QMetaEnum>

/**
  @brief The list of persistent properties of the TopLevelItem class.
  */
const QStringList TopLevelItem::PersistentProperties = {"colorName"};

/**
   @brief The item type used by top level items.
 */
const QString TopLevelItem::ItemType = "TopLevelItem";

/**
   @brief Constructor.
 */
TopLevelItem::TopLevelItem(const QString &directory,
                           const QString &itemType,
                           const QStringList &persistentProperties,
                           QObject *parent) :
  ComplexItem(false, directory, itemType, persistentProperties + PersistentProperties, parent),
  m_color(White)
{
  initializeItem();
}

/**
   @brief Set the item color.
 */
void TopLevelItem::setColor(const Color &color)
{
  if (m_color != color) {
    m_color = color;
    emit colorChanged();
    saveItem();
  }
}

/**
   @brief Returns the name of the color of the item.
 */
QString TopLevelItem::colorName() const {
  QMetaEnum e = QMetaEnum::fromType<Color>();
  return e.valueToKey(m_color);
}

/**
   @brief Sets the color of the item via color string.
 */
void TopLevelItem::setColorName(const QString &color)
{
  QMetaEnum e = QMetaEnum::fromType<Color>();
  bool ok;
  Color c = static_cast<Color>(e.keysToValue(qUtf8Printable(color), &ok));
  if (ok) {
    setColor(c);
  }
}

TopLevelItem::TopLevelItem(bool loadItem,
                           const QString &directory,
                           const QString &itemType,
                           const QStringList &persistentProperties,
                           QObject *parent) :
  ComplexItem(false, directory, itemType, persistentProperties + PersistentProperties, parent),
  m_color(White)
{
  if (loadItem) {
    initializeItem();
  }
}
