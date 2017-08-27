#include "itemcontainer.h"

#include <limits>

#include <QMutexLocker>
#include <QtConcurrent>
#include <QThreadPool>


/**
 * @brief Constructor.
 */
ItemContainer::ItemContainer(QObject *parent) : QObject(parent),
    m_items(),
    m_uidMap(),
    m_threadPool(new QThreadPool(this)),
    m_lock(QMutex::Recursive),
    m_minWeight(std::numeric_limits<double>::infinity()),
    m_maxWeight(-std::numeric_limits<double>::infinity())
{
    qRegisterMetaType<ItemPtr>();
    m_threadPool->setMaxThreadCount(1);
}

/**
 * @brief Destructor.
 */
ItemContainer::~ItemContainer()
{
    delete m_threadPool;
}

/**
 * @brief Get the number of items in the container.
 */
int ItemContainer::count() const
{
    QMutexLocker l(&m_lock);
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
    QMutexLocker l(&m_lock);
    if (index >= 0 && index < m_items.length()) {
        return m_items.at(index);
    }
    return ItemPtr();
}

/**
 * @brief Add an item to the container.
 *
 * This adds the @p item to the container. The itemAdded() signal
 * is emitted with the index of the newly inserted item once
 * the insertion finishes.
 *
 * @note Insertion is done in a background thread. The signal
 * is fired delayed. This method has no effect if the them either
 * is a nullptr or the UID of the item already is known to the
 * container.
 *
 * @sa updateOrInsert()
 * @sa updateItem()
 */
void ItemContainer::addItem(ItemPtr item)
{
    QMutexLocker l(&m_lock);
    if (!item.isNull() && !m_uidMap.contains(item->uid())) {
        connect(item.data(), &Item::weightChanged,
                this, static_cast<void(ItemContainer::*)()>(&ItemContainer::updateWeights));
        QtConcurrent::run(m_threadPool, [=]() {
            QMutexLocker l(&m_lock);
            connect(item.data(), &Item::itemDeleted, this, &ItemContainer::handleDeleteItem);
            connect(item.data(), &Item::changed, this, &ItemContainer::handleItemChanged);
            m_items.append(item);
            m_uidMap.insert(item->uid(), item);
            this->updateWeights(item.data());
            QMetaObject::invokeMethod(
                        this, "itemAdded",
                        Qt::QueuedConnection,
                        Q_ARG(int, m_items.length() - 1));
        });
    }
}

/**
 * @brief Update an item in the container.
 *
 * This updates an item in the container with the properties of the given @p item.
 * The item to be updated is identified by its uid.
 *
 * @note The update is run in a background thread. If no item
 * with the same UID as the item is found, the method basically
 * has no effect.
 */
void ItemContainer::updateItem(ItemPtr item)
{
    if (!item.isNull()) {
        QtConcurrent::run(m_threadPool, [=]() {
            QMutexLocker l(&m_lock);
            auto existingItem = m_uidMap.value(item->uid());
            if (!existingItem.isNull()) {
                QMetaObject::invokeMethod(
                            this, "patchItem",
                            Qt::QueuedConnection,
                            Q_ARG(ItemPtr, existingItem),
                            Q_ARG(QVariant, item->toVariant()));
            }
        });
    }
}

/**
 * @brief Remove an item from the container.
 *
 * This removes an item from the container. The item to be removed is identified by the
 * uid. If it matches the uid of the @p item, it will be removed and the @sa itemDeleted signal
 * is emitted with the index of the item removed.
 *
 * @note Removal is done in a background thread. The signal
 * is hence emitted delayed. Also note that if this method
 * is called first and - while it still runs - another operation
 * is used which inserts an item with the same UID, the recently
 * added new item will not be deleted.
 */
void ItemContainer::deleteItem(ItemPtr item)
{
    if (item) {
        handleDeleteItem(item.data());
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
        ItemPtr existingItem;
        {
            QMutexLocker l(&m_lock);
            existingItem = m_uidMap.value(item->uid());
        }
        if (existingItem.isNull()) {
            addItem(item);
        }
        else {
            existingItem->fromVariant(item->toVariant());
        }
    }
}

/**
 * @brief Remove all items from the container.
 */
void ItemContainer::clear()
{
    QMutexLocker l(&m_lock);
    m_items.clear();
    m_uidMap.clear();
    emit cleared();
}

/**
 * @brief Gets the weight for the next item.
 *
 * This method returns the weight for the next item to be added to the container.
 */
double ItemContainer::nextItemWeight() const
{
    QMutexLocker l(&m_lock);
    auto result = m_minWeight;
    if (count() == 0) {
        result = 0.0;
    }
    result -= static_cast<double>(qrand()) / RAND_MAX;
    return result;
}

/**
 * @brief Remove the item from the container.
 */
void ItemContainer::handleDeleteItem(Item* item)
{
    if (item != nullptr) {
        QtConcurrent::run(m_threadPool, [=]() {
            auto c = count();
            for (int i = 0; i < c; ++i) {
                m_lock.lock();
                auto existingItem = m_items.at(i);
                m_lock.unlock();
                if (existingItem->uid() == item->uid()) {
                    QMutexLocker l(&m_lock);
                    m_items.removeAt(i);
                    m_uidMap.remove(item->uid());
                    QMetaObject::invokeMethod(
                                this, "itemDeleted",
                                Qt::QueuedConnection,
                                Q_ARG(int, i));
                    break;
                }
            }
        });
    }
}

void ItemContainer::handleItemChanged()
{
    auto sender = this->sender();
    Item* item = static_cast<Item*>(sender);
    QtConcurrent::run(m_threadPool, [=]() {
        QMutexLocker l(&m_lock);
        for (int i = 0; i < m_items.count(); ++i) {
            auto it = m_items.at(i);
            if (it.data() == item) {
                QMetaObject::invokeMethod(
                            this, "emitItemChanged",
                            Qt::QueuedConnection,
                            Q_ARG(int, i));
                break;
            }
        }
    });
}

void ItemContainer::emitItemChanged(int index)
{
    if (index >= 0 && index < m_items.count()) {
        emit itemChanged(index);
    }
}

/**
 * @brief Update the @p item with the given @p data.
 */
void ItemContainer::patchItem(ItemPtr item, QVariant data)
{
    item->fromVariant(data);
}

void ItemContainer::updateWeights(Item* item)
{
    QMutexLocker l(&m_lock);
    m_maxWeight = std::max(m_maxWeight, item->weight());
    m_minWeight = std::min(m_minWeight, item->weight());
}

void ItemContainer::updateWeights()
{
    updateWeights(static_cast<Item*>(sender()));
}

