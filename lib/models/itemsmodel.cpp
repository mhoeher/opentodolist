#include "itemsmodel.h"

#include "datastorage/getitemsquery.h"

#include <QQmlEngine>

ItemsModel::ItemsModel(QObject *parent) :
    QAbstractListModel(parent),
    m_cache(),
    m_items(),
    m_ids(),
    m_fetchTimer(),
    m_parentItem()
{
    m_fetchTimer.setInterval(100);
    connect(&m_fetchTimer, &QTimer::timeout,
            this, &ItemsModel::fetch);
    m_fetchTimer.setSingleShot(true);
}

/**
 * @brief The item cache the model works on.
 */
Cache* ItemsModel::cache() const
{
    return m_cache.data();
}

/**
 * @brief Set the item container.
 */
void ItemsModel::setCache(Cache* cache)
{
    if (cache != m_cache) {
        if (m_cache != nullptr) {
            disconnect(m_cache.data(), &Cache::dataChanged,
                       this, &ItemsModel::triggerFetch);
        }
        reset();
        m_cache = cache;
        if (m_cache != nullptr) {
            connect(m_cache.data(), &Cache::dataChanged,
                    this, &ItemsModel::triggerFetch);
        }
        emit cacheChanged();
    }
}

/**
 * @brief The number of items in the model.
 */
int ItemsModel::count() const
{
    return rowCount();
}

int ItemsModel::rowCount(const QModelIndex& parent) const
{
    if (!parent.isValid()) {
        return m_ids.length();
    } else {
        return 0;
    }
}

QVariant ItemsModel::data(const QModelIndex& index, int role) const
{
    int row = index.row();
    if (row < m_ids.length()) {
        auto id = m_ids.at(row);
        auto item = m_items.value(id);
        switch (role) {
        case Qt::DisplayRole:
        case ItemRole:
            return QVariant::fromValue<QObject*>(item);
        case WeightRole:
            return item->weight();
        default:
            break;
        }
    }
    return QVariant();
}

QHash<int, QByteArray> ItemsModel::roleNames() const
{
    auto result = QAbstractListModel::roleNames();
    result.insert(ItemRole, "object");
    result.insert(WeightRole, "weight");
    return result;
}


/**
 * @brief The ID of the parent item to retrieve items for.
 */
QUuid ItemsModel::parentItem() const
{
    return m_parentItem;
}


/**
 * @brief Set the parent item to retrieve items for.
 */
void ItemsModel::setParentItem(const QUuid &parentItem)
{
    if (m_parentItem != parentItem) {
        m_parentItem = parentItem;
        emit parentItemChanged();
        triggerFetch();
    }
}


void ItemsModel::reset()
{
    emit beginResetModel();
    m_ids.clear();
    for (auto item : m_items.values()) {
        delete item;
    }
    m_items.clear();
    triggerFetch();
    emit endResetModel();
    emit countChanged();
}

void ItemsModel::fetch()
{
    if (m_cache) {
        auto q = new GetItemsQuery();
        if (!m_parentItem.isNull()) {
            q->setParent(m_parentItem);
        }
        connect(q, &GetItemsQuery::itemsAvailable,
                this, &ItemsModel::update);
        m_cache->run(q);
    }
}

void ItemsModel::triggerFetch()
{
    m_fetchTimer.start();
}

void ItemsModel::update(QVariantList items)
{
    auto idsToDelete = QSet<QUuid>::fromList(m_ids);
    QList<Item*> newItems;
    for (auto data : items) {
        auto item = Item::decache(data, this);
        auto id = item->uid();
        if (m_items.contains(id)) {
            auto existingItem = m_items.value(id);
            existingItem->fromVariant(item->toVariant());
            delete item;
            idsToDelete.remove(id);
        } else {
            auto item = Item::decache(data, this);
            newItems << item;
        }
    }

    if (!idsToDelete.isEmpty()) {
        for (auto id : idsToDelete) {
            auto index = m_ids.indexOf(id);
            beginRemoveRows(QModelIndex(), index, index);
            auto item = m_items.take(id);
            delete item;
            m_ids.removeAt(index);
            endRemoveRows();
        }
    }

    if (!newItems.isEmpty()) {
        beginInsertRows(
                    QModelIndex(),
                    m_ids.length(),
                    m_ids.length() + newItems.length() - 1);
        for (auto item : newItems) {
            connect(item, &Item::changed,
                    this, &ItemsModel::itemChanged);
            m_ids.append(item->uid());
            m_items.insert(item->uid(), item);
            QQmlEngine::setObjectOwnership(item, QQmlEngine::CppOwnership);
        }
        endInsertRows();
    }

    if (!idsToDelete.isEmpty() || !newItems.isEmpty()) {
        emit countChanged();
    }
}

void ItemsModel::itemChanged()
{
    auto item = qobject_cast<Item*>(sender());
    if (item) {
        auto id = item->uid();
        if (m_items.contains(id)) {
            auto index = m_ids.indexOf(id);
            auto modelIndex = this->index(index);
            emit dataChanged(modelIndex, modelIndex);
        }
    }
}
