#include <QQmlEngine>
#include <QDateTime>

#include "itemsmodel.h"

#include "datamodel/complexitem.h"
#include "datamodel/todolist.h"
#include "datamodel/todo.h"

#include "datastorage/getitemsquery.h"
#include "datastorage/insertorupdateitemsquery.h"


ItemsModel::ItemsModel(QObject *parent) :
    QAbstractListModel(parent),
    m_cache(),
    m_items(),
    m_ids(),
    m_fetchTimer(),
    m_parentItem(),
    m_searchString(),
    m_tag(),
    m_onlyDone(false),
    m_onlyUndone(false),
    m_onlyWithDueDate(false),
    m_defaultSearchResult(true),
    m_updating(false)
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


/**
 * @brief Only include items which match the given search string.
 */
QString ItemsModel::searchString() const
{
    return m_searchString;
}


/**
 * @brief Set the search string.
 */
void ItemsModel::setSearchString(const QString &searchString)
{
    if (m_searchString != searchString) {
        m_searchString = searchString;
        triggerFetch();
        emit searchStringChanged();
    }
}


/**
 * @brief Only include items which have the given tag.
 */
QString ItemsModel::tag() const
{
    return m_tag;
}


/**
 * @brief Set the tag used for filtering.
 */
void ItemsModel::setTag(const QString &tag)
{
    if (m_tag != tag) {
        m_tag = tag;
        triggerFetch();
        emit tagChanged();
    }
}


/**
 * @brief Include only items which have their "done" property set to true.
 */
bool ItemsModel::onlyDone() const
{
    return m_onlyDone;
}


/**
 * @brief Set if only done items shall be included.
 */
void ItemsModel::setOnlyDone(bool value)
{
    if (m_onlyDone != value) {
        m_onlyDone = value;
        triggerFetch();
        emit onlyDoneChanged();
    }
}


/**
 * @brief Include only items which have their "done" property set to false.
 */
bool ItemsModel::onlyUndone() const
{
    return m_onlyUndone;
}


/**
 * @brief Set if only undone items shall be included.
 */
void ItemsModel::setOnlyUndone(bool value)
{
    if (m_onlyUndone != value) {
        m_onlyUndone = value;
        triggerFetch();
        emit onlyUndoneChanged();
    }
}


/**
 * @brief Only include open items which have a due date set.
 */
bool ItemsModel::onlyWithDueDate() const
{
    return m_onlyWithDueDate;
}


/**
 * @brief Set if only open items with a due date set shall be included.
 */
void ItemsModel::setOnlyWithDueDate(bool value)
{
    if (m_onlyWithDueDate != value) {
        m_onlyWithDueDate = value;
        triggerFetch();
        emit onlyWithDueDateChanged();
    }
}


/**
 * @brief The default search/filter result.
 *
 * This is the default result of the filter operation. If this is true,
 * all items are included unless they are not matched by a filter. If it is
 * false, all items are excluded, unless they are included by a filter.
 */
bool ItemsModel::defaultSearchResult() const
{
    return m_defaultSearchResult;
}


/**
 * @brief Set the default search/filter result.
 */
void ItemsModel::setDefaultSearchResult(bool defaultSearchResult)
{
    if (m_defaultSearchResult != defaultSearchResult) {
        m_defaultSearchResult = defaultSearchResult;
        triggerFetch();
        emit defaultSearchResultChanged();

    }
}


bool ItemsModel::itemMatches(ItemPtr item, QStringList words) {
    for (auto word : words) {
        if (item->title().indexOf(word, 0, Qt::CaseInsensitive) >= 0) {
            return true;
        }
        auto complexItem = item.dynamicCast<ComplexItem>();
        if (complexItem != nullptr) {
            if (complexItem->notes().indexOf(
                        word, 0, Qt::CaseInsensitive) >= 0) {
                return true;
            }
        }
    }
    return false;
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

        auto tag = m_tag;
        auto onlyDone = m_onlyDone;
        auto onlyUndone = m_onlyUndone;
        auto onlyWithDueDate = m_onlyWithDueDate;
        auto itemMatchesFilter = getFilterFn();

        q->setItemFilter([=](ItemPtr item, GetItemsQuery* query) {
            auto result = true;
            if (itemMatchesFilter) {
                result = itemMatchesFilter(item, query);
            }
            if (onlyDone && !item->property("done").toBool()) {
                result = false;
            }
            if (onlyUndone && item->property("done").toBool()) {
                result = false;
            }
            if (onlyWithDueDate &&
                    !item->property("dueTo").toDateTime().isNull()) {
                result = false;
            }
            return result;
        });
        connect(q, &GetItemsQuery::itemsAvailable,
                this, &ItemsModel::update);
        m_cache->run(q);
    }
}

std::function<bool (ItemPtr item, GetItemsQuery *query)> ItemsModel::getFilterFn() const
{
    auto words = m_searchString.split(QRegExp("\\s+"),
                                      QString::SkipEmptyParts);
    bool defaultSearchResult = m_defaultSearchResult;
    std::function<bool(ItemPtr, GetItemsQuery*)> itemMatchesFilter;
    if (!words.isEmpty()) {
        itemMatchesFilter = [=](ItemPtr item, GetItemsQuery *query) {
            bool result = defaultSearchResult;
            if (itemMatches(item, words)) {
                return true;
            } else {
                auto todoList = item.dynamicCast<TodoList>();
                if (todoList) {
                    for (auto todo : query->childrenOf(todoList->uid())) {
                        if (itemMatches(todo, words)) {
                            result = true;
                            break;
                        } else {
                            for (auto task :
                                 query->childrenOf(todo->uid())) {
                                if (itemMatches(task, words)) {
                                    result = true;
                                    break;
                                }
                            }
                            if (result) {
                                break;
                            }
                        }
                    }
                } else {
                    auto todo = item.dynamicCast<Todo>();
                    if (todo) {
                        for (auto task : query->childrenOf(todo->uid())) {
                            if (itemMatches(task, words)) {
                                result = true;
                                break;
                            }
                        }
                    }
                }
            }
            return result;
        };
    }
    return itemMatchesFilter;
}

void ItemsModel::triggerFetch()
{
    m_fetchTimer.start();
}

void ItemsModel::update(QVariantList items)
{
    m_updating = true;
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

    m_updating = false;
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
        if (!m_updating && m_cache != nullptr) {
            auto q = new InsertOrUpdateItemsQuery();
            q->add(item, InsertOrUpdateItemsQuery::Save);
            m_cache->run(q);
        }
    }
}
