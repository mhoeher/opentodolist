#include "itemcontainer.h"

/**
 * @brief Constructor.
 */
ItemContainer::ItemContainer(QObject *parent) : QObject(parent),
    m_items(),
    m_uidMap()
{
}

/**
 * @brief Destructor.
 */
ItemContainer::~ItemContainer()
{

}

/**
 * @brief Get the number of items in the container.
 */
int ItemContainer::count() const
{
    return m_items.length();
}

/**
 * @brief Get the item at the given index.
 *
 * This method returns the item at the given @p index or an invalid
 * pointer if index is out of bounds.
 */
ItemPtr ItemContainer::item(int index) const
{
    if (index >= 0 && index < count()) {
        return m_items.at(index);
    }
    return ItemPtr();
}

/**
 * @brief Add an item to the
 * @param item
 */
void ItemContainer::addItem(ItemPtr item)
{
    if (!item.isNull()) {
        m_items.append(item);
        m_uidMap.insert(item->uid(), item);
        emit itemAdded(count() - 1);
    }
}

/**
 * @brief Update an item in the container.
 *
 * This updates an item in the container with the properties of the given @p item.
 * The item to be updated is identified by its uid.
 */
void ItemContainer::updateItem(ItemPtr item)
{
    if (!item.isNull()) {
        auto existingItem = m_uidMap.value(item->uid());
        if (!existingItem.isNull()) {
            existingItem->fromVariant(item->toVariant());
        }
    }
}

/**
 * @brief Remove an item from the container.
 *
 * This removes an item from the container. The item to be removed is identified by the
 * uid. If it matches the uid of the @p item, it will be removed and the @sa itemDeleted signal
 * is emitted with the index of the item removed.
 */
void ItemContainer::deleteItem(ItemPtr item)
{
    if (!item.isNull()) {
        for (int i = 0; i < m_items.length(); ++i) {
            auto existingItem = m_items.at(i);
            if (existingItem->uid() == item->uid()) {
                m_items.removeAt(i);
                m_uidMap.remove(item->uid());
                emit itemDeleted(i);
                break;
            }
        }
    }
}

/**
 * @brief Update or add an item.
 *
 * This is a combination of the addItem and updateItem methods: It checks first if an
 * item with the same uid already is stored in the container. If this is the case, this item
 * is updated with the values in the @p item. Otherwise, item itself is added to the container.
 */
void ItemContainer::updateOrInsert(ItemPtr item)
{
    if (!item.isNull()) {
        auto existingItem = m_uidMap.value(item->uid());
        if (existingItem.isNull()) {
            addItem(item);
        } else {
            existingItem->fromVariant(item->toVariant());
        }
    }
}

/**
 * @brief Remove all items from the container.
 */
void ItemContainer::clear()
{
    m_items.clear();
    m_uidMap.clear();
    emit cleared();
}
