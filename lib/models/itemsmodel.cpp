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

#include "itemsmodel.h"

#include <QQmlEngine>
#include <QDateTime>

#include "datamodel/complexitem.h"
#include "datamodel/todolist.h"
#include "datamodel/todo.h"
#include "datamodel/notepage.h"

#include "datastorage/getitemsquery.h"
#include "datastorage/insertorupdateitemsquery.h"

ItemsModel::ItemsModel(QObject* parent)
    : QAbstractListModel(parent),
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
      m_recursive(false),
      m_updating(false)
{
    m_fetchTimer.setInterval(100);
    connect(&m_fetchTimer, &QTimer::timeout, this, &ItemsModel::fetch);
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
            disconnect(m_cache.data(), &Cache::dataChanged, this, &ItemsModel::triggerFetch);
        }
        reset();
        m_cache = cache;
        if (m_cache != nullptr) {
            connect(m_cache.data(), &Cache::dataChanged, this, &ItemsModel::triggerFetch);
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
    if (row < m_ids.length() && row >= 0) {
        auto id = m_ids.at(row);
        auto item = m_items.value(id);
        switch (role) {
        case Qt::DisplayRole:
        case ItemRole:
            return QVariant::fromValue<QObject*>(item);
        case DueToRole: {
            auto complexItem = qobject_cast<ComplexItem*>(item);
            if (complexItem) {
                return complexItem->dueTo().toString(Qt::ISODate);
            } else {
                return QVariant();
            }
        }
        case EffectiveDueToRole: {
            auto complexItem = qobject_cast<ComplexItem*>(item);
            if (complexItem) {
                return complexItem->effectiveDueTo().toString(Qt::ISODate);
            } else {
                return QVariant();
            }
        }
        case DueToSpanRole:
        case EffectiveDueToSpanRole:
            return timeSpanLabel(item, role);
        case WeightRole:
            return item->weight();
        case TitleRole:
            return item->title();
        case UidRole:
            return item->uid();
        case CreatedAtRole:
            return item->createdAt();
        case UpdatedAtRole:
            return item->updatedAt();
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
    result.insert(DueToRole, "dueTo");
    result.insert(DueToSpanRole, "dueToSpan");
    result.insert(TitleRole, "title");
    result.insert(CreatedAtRole, "createdAt");
    result.insert(UpdatedAtRole, "updatedAt");
    result.insert(EffectiveDueToRole, "effectiveDueTo");
    result.insert(EffectiveDueToSpanRole, "effectiveDueToSpan");
    result.insert(UidRole, "uid");
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
void ItemsModel::setParentItem(const QUuid& parentItem)
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
void ItemsModel::setSearchString(const QString& searchString)
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
void ItemsModel::setTag(const QString& tag)
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

/**
 * @brief Shall items be retrieved recursively.
 *
 * This property defines if items are retrieved recursively from the cache.
 * If this is false and a parent item is set, only direct children of this
 * item are retrieved. Otherwise, all children - direct and indirect -
 * are retrieved.
 */
bool ItemsModel::recursive() const
{
    return m_recursive;
}

/**
 * @brief Set if items shall be retrieved recursively.
 */
void ItemsModel::setRecursive(bool recursive)
{
    if (m_recursive != recursive) {
        m_recursive = recursive;
        triggerFetch();
        emit recursiveChanged();
    }
}

/**
 * @brief The label used for overdue items.
 */
QString ItemsModel::overdueLabel() const
{
    return m_overdueLabel;
}

/**
 * @brief Set the label used for overdue items.
 */
void ItemsModel::setOverdueLabel(const QString& overdueLabel)
{
    if (m_overdueLabel != overdueLabel) {
        m_overdueLabel = overdueLabel;
        auto rowCount = this->rowCount();
        if (rowCount > 0) {
            emit dataChanged(index(0), index(rowCount));
        }
        emit overdueLabelChanged();
    }
}

/**
 * @brief Time span definitions.
 *
 * This property holds a map where the keys are minimum dates and times and
 * the values are a label. If an item's due date is greater than a value in
 * this map, it is assigned the appropriate label. If none of the due dates
 * in the map is earlier than the overdueLabel is used for the item.
 */
QVariantMap ItemsModel::timeSpans() const
{
    return m_timeSpans;
}

/**
 * @brief Set the time spans used for grouping items by due date.
 */
void ItemsModel::setTimeSpans(const QVariantMap& timeSpans)
{
    if (m_timeSpans != timeSpans) {
        m_timeSpans = timeSpans;
        auto rowCount = this->rowCount();
        if (rowCount > 0) {
            emit dataChanged(index(0), index(rowCount - 1));
        }
        emit timeSpansChanged();
    }
}

/**
 * @brief Get the role from the @p roleName.
 */
int ItemsModel::roleFromName(const QString& roleName) const
{
    return roleNames().key(roleName.toUtf8(), -1);
}

/**
 * @brief Include only items with the given type.
 */
QString ItemsModel::itemType() const
{
    return m_itemType;
}

/**
 * @brief Set the itemType filter property.
 */
void ItemsModel::setItemType(const QString& itemType)
{
    if (m_itemType != itemType) {
        m_itemType = itemType;
        triggerFetch();
        emit itemTypeChanged();
    }
}

bool ItemsModel::itemMatches(ItemPtr item, QStringList words)
{
    for (auto word : words) {
        if (item->title().indexOf(word, 0, Qt::CaseInsensitive) >= 0) {
            return true;
        }
        auto complexItem = item.dynamicCast<ComplexItem>();
        if (complexItem != nullptr) {
            if (complexItem->notes().indexOf(word, 0, Qt::CaseInsensitive) >= 0) {
                return true;
            }
        }
    }
    return false;
}

QString ItemsModel::timeSpanLabel(Item* item, int role) const
{
    QString result;
    auto complexItem = qobject_cast<ComplexItem*>(item);
    QDateTime dueTo;
    if (complexItem) {
        switch (role) {
        case DueToSpanRole:
            dueTo = complexItem->dueTo();
            break;
        case EffectiveDueToSpanRole:
            dueTo = complexItem->effectiveDueTo();
            break;
        default:
            break;
        }
    }

    if (dueTo.isValid()) {
        result = m_overdueLabel;
        auto dueDate = dueTo.date().toString("yyyy-MM-dd");
        // Note: Keys in the map are sorted, so we iterate from least to most
        //       recent entries:
        for (auto key : m_timeSpans.keys()) {
            if (dueDate >= key) {
                result = m_timeSpans.value(key).toString();
            }
        }
    }
    return result;
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
        q->setRecursive(m_recursive);

        auto tag = m_tag;
        auto onlyDone = m_onlyDone;
        auto onlyUndone = m_onlyUndone;
        auto onlyWithDueDate = m_onlyWithDueDate;
        auto itemMatchesFilter = getFilterFn();
        auto defaultSearchResult = m_defaultSearchResult;
        auto itemType = m_itemType;

        q->setItemFilter([=](ItemPtr item, GetItemsQuery* query) {
            if (!itemType.isEmpty() && item->itemType() != itemType) {
                return false;
            }
            auto result = true;
            if (itemMatchesFilter) {
                result = itemMatchesFilter(item, query);
            } else {
                result = defaultSearchResult;
            }
            if (onlyDone) {
                auto done = item->property("done");
                if (!done.isNull() && !done.toBool()) {
                    result = false;
                }
            }
            if (onlyUndone) {
                auto done = item->property("done");
                if (!done.isNull() && done.toBool()) {
                    result = false;
                }
            }
            if (onlyWithDueDate && !item->property("dueTo").toDateTime().isValid()) {
                result = false;
            }
            if (!tag.isEmpty()) {
                if (!item->property("tags").toStringList().contains(tag)) {
                    result = false;
                }
            }
            return result;
        });
        connect(q, &GetItemsQuery::itemsAvailable, this, &ItemsModel::update, Qt::QueuedConnection);
        m_cache->run(q);
    }
}

std::function<bool(ItemPtr item, GetItemsQuery* query)> ItemsModel::getFilterFn() const
{
    auto words = m_searchString.split(QRegExp("\\s+"), QString::SkipEmptyParts);
    std::function<bool(ItemPtr, GetItemsQuery*)> itemMatchesFilter;
    if (!words.isEmpty()) {
        itemMatchesFilter = [=](ItemPtr item, GetItemsQuery* query) {
            bool result = false;
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
                            auto tasks = query->childrenOf(todo->uid());
                            if (std::any_of(tasks.begin(), tasks.end(), [=](const ItemPtr task) {
                                    return itemMatches(task, words);
                                })) {
                                result = true;
                                break;
                            }
                        }
                    }
                } else {
                    auto todo = item.dynamicCast<Todo>();
                    if (todo) {
                        auto tasks = query->childrenOf(todo->uid());
                        result = std::any_of(tasks.begin(), tasks.end(), [=](const ItemPtr task) {
                            return itemMatches(task, words);
                        });
                    } else {
                        auto note = item.dynamicCast<Note>();
                        if (note) {
                            auto pages = query->childrenOf(note->uid());
                            result = std::any_of(
                                    pages.begin(), pages.end(),
                                    [=](const ItemPtr page) { return itemMatches(page, words); });
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
#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
    auto idsToDelete = QSet<QUuid>::fromList(m_ids);
#else
    auto idsToDelete = QSet<QUuid>(m_ids.begin(), m_ids.end());
#endif
    QList<Item*> newItems;
    for (auto data : items) {
        auto item = Item::decache(data, this);
        auto id = item->uid();
        if (m_items.contains(id)) {
            auto existingItem = m_items.value(id);
            existingItem->fromVariant(item->toVariant());
            existingItem->applyCalculatedProperties(
                    data.value<ItemCacheEntry>().calculatedData.toMap());
            delete item;
            idsToDelete.remove(id);
        } else {
            auto item_ = Item::decache(data, this);
            newItems << item_;
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
        beginInsertRows(QModelIndex(), m_ids.length(), m_ids.length() + newItems.length() - 1);
        for (auto item : newItems) {
            connect(item, &Item::changed, this, &ItemsModel::itemChanged);
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
