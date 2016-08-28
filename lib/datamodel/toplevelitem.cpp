#include "toplevelitem.h"

#include <QMetaEnum>

/**
  @brief The list of persistent properties of the TopLevelItem class.
  */
const QStringList TopLevelItem::PersistentProperties = {"colorName", "tags"};

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
    m_color(White),
    m_tags(QStringList())
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
    m_color(White),
    m_tags(QStringList())
{
    if (loadItem) {
        initializeItem();
    }
}

/**
   @brief A list of tags attached to the item.
   
   This is a list of tags that have been attached to the item. Tags can be used for filtering
   items. The tags are sorted alphabetically.
 */
QStringList TopLevelItem::tags() const
{
    return m_tags;
}

void TopLevelItem::setTags(const QStringList& tags)
{
    if (m_tags != tags)
    {
        m_tags = tags;
        emit tagsChanged();
        saveItem();
    }
}

/**
   @brief Adds a new @p tag to the item.
   
   This adds a new tag to the item. If the tag already has been attached to the item before,
   this method has no effect.
 */
void TopLevelItem::addTag(const QString& tag)
{
    if (!m_tags.contains(tag))
    {
        m_tags.append(tag);
        m_tags.sort();
        saveItem();
        emit tagsChanged();
    }
}

/**
   @brief Removes a tag from the item.
   
   This removes the tag at the given @p index from the item. The index must be
   valid (i.e. 0 <= index < item.tags().length()). If not, this method will assert.
 */
void TopLevelItem::removeTagAt(int index)
{
    Q_ASSERT(index >= 0 && index < m_tags.length());
    m_tags.removeAt(index);
    saveItem();
    emit tagsChanged();
}

/**
   @brief Removes the @p tag from the item (if it is assigned).
 */
void TopLevelItem::removeTag(const QString& tag)
{
    auto index = m_tags.indexOf(tag);
    if (index >= 0) {
        removeTagAt(index);
    }
}

/**
   @brief Returns true if the item has been tagged with the given @p tag.
 */
bool TopLevelItem::hasTag(const QString& tag) const
{
    return m_tags.contains(tag);
}
