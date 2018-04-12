#include "itemssortfiltermodel.h"

#include <QUuid>

#include "itemsmodel.h"

#include "datamodel/item.h"
#include "datamodel/library.h"
#include "datamodel/task.h"
#include "datamodel/todo.h"
#include "datamodel/todolist.h"

ItemsSortFilterModel::ItemsSortFilterModel(QObject *parent) :
    QSortFilterProxyModel(parent),
    m_searchString(),
    m_defaultSearchResult(true),
    m_tag(),
    m_onlyDone(false),
    m_onlyUndone(false),
    m_todoList(),
    m_todo(),
    m_minDueDate(),
    m_maxDueDate()
{
    setSortRole(ItemsModel::WeightRole);
    auto handleRowsChanged = [=](const QModelIndex&, int, int) {
        emit countChanged();
        sort(0);
    };
    connect(this, &QSortFilterProxyModel::rowsInserted, handleRowsChanged);
    connect(this, &QSortFilterProxyModel::rowsRemoved, handleRowsChanged);
    connect(this, &QSortFilterProxyModel::modelReset,
            [=]() {
        emit countChanged();
        sort(0);
    });
    connect(this, &ItemsSortFilterModel::dataChanged,
            [=](const QModelIndex&, const QModelIndex, QVector<int>) {
        sort(0);
    });
    connect(this, &ItemsSortFilterModel::sourceModelChanged, [=]() {
        sort(0);
    });
}


/**
 * @brief The number of items in the model.
 */
int ItemsSortFilterModel::count() const
{
    return rowCount();
}

bool ItemsSortFilterModel::filterAcceptsRow(
        int source_row, const QModelIndex& source_parent) const
{
    Q_UNUSED(source_parent);

    auto item = sourceModel()->data(sourceModel()->index(source_row, 0),
                                    ItemsModel::ItemRole).value<Item*>();
    bool result = true;

    if (item != nullptr) {
        if (!m_tag.isEmpty()) {
            result = result &&
                    item->property("tags").toStringList().contains(m_tag);
        }

        if (m_onlyDone) {
            result = result && item->property("done").toBool();
        }

        if (m_onlyUndone) {
            result = result && !item->property("done").toBool();
        }

        if (!m_todoList.isNull()) {
            result = result &&
                    (item->property("todoListUid").toUuid() == m_todoList);
        }

        if (!m_todo.isNull()) {
            result = result && (item->property("todoUid").toUuid() == m_todo);
        }

        if (!m_minDueDate.isNull()) {
            auto dueTo = item->property("dueTo").toDateTime();
            result = result && !dueTo.isNull() && (dueTo >= m_minDueDate);
        }

        if (!m_maxDueDate.isNull()) {
            auto dueTo = item->property("dueTo").toDateTime();
            result = result && !dueTo.isNull() && (dueTo < m_maxDueDate);
        }

        result = result && itemMatchesFilter(item);
    }

    return result;
}

QDateTime ItemsSortFilterModel::maxDueDate() const
{
    return m_maxDueDate;
}

void ItemsSortFilterModel::setMaxDueDate(const QDateTime &maxDueDate)
{
    if (m_maxDueDate != maxDueDate) {
        m_maxDueDate = maxDueDate;
        invalidateFilter();
        emit maxDueDateChanged();
    }
}

QDateTime ItemsSortFilterModel::minDueDate() const
{
    return m_minDueDate;
}

void ItemsSortFilterModel::setMinDueDate(const QDateTime &minDueDate)
{
    if (m_minDueDate != minDueDate) {
        m_minDueDate = minDueDate;
        invalidateFilter();
        emit minDueDateChanged();
    }
}


/**
 * @brief Show tasks belonging to the given todo.
 *
 * If this is set to a valid UUID, only tasks will be shown which have their
 * parent todo UID set to this one.
 */
QUuid ItemsSortFilterModel::todo() const
{
    return m_todo;
}


/**
 * @brief Set the todo filter.
 */
void ItemsSortFilterModel::setTodo(const QUuid &todo)
{
    if (m_todo != todo) {
        m_todo = todo;
        emit todoChanged();
        invalidateFilter();
    }
}


/**
 * @brief Show only todos belonging to the given todo list.
 *
 * If this filter is set to a valid UUID, only todos will be shown where the
 * todo list UID equals the set one.
 */
QUuid ItemsSortFilterModel::todoList() const
{
    return m_todoList;
}


/**
 * @brief Set the todo list filter.
 */
void ItemsSortFilterModel::setTodoList(const QUuid &todoList)
{
    if (m_todoList != todoList) {
        m_todoList = todoList;
        emit todoListChanged();
        invalidateFilter();
    }
}


/**
 * @brief Show only items where the "done" property is set to false.
 */
bool ItemsSortFilterModel::onlyUndone() const
{
    return m_onlyUndone;
}


/**
 * @brief Set the only undone filter.
 */
void ItemsSortFilterModel::setOnlyUndone(bool onlyUndone)
{
    if (m_onlyUndone != onlyUndone) {
        m_onlyUndone = onlyUndone;
        emit onlyUndoneChanged();
        invalidateFilter();
    }
}


/**
 * @brief Show only items which have the "done" property set to true.
 */
bool ItemsSortFilterModel::onlyDone() const
{
    return m_onlyDone;
}


/**
 * @brief Set the only done filter.
 */
void ItemsSortFilterModel::setOnlyDone(bool onlyDone)
{
    if (m_onlyDone != onlyDone) {
        m_onlyDone = onlyDone;
        emit onlyDoneChanged();
        invalidateFilter();
    }
}

/**
 * @brief Filter by tag.
 *
 * If the tag property is set to a non-empty string, only items
 * will be shown that have the given tag set on them.
 */
QString ItemsSortFilterModel::tag() const
{
    return m_tag;
}


/**
 * @brief Set the tag filter.
 */
void ItemsSortFilterModel::setTag(const QString &tag)
{
    if (m_tag != tag) {
        m_tag = tag;
        emit tagChanged();
        invalidateFilter();
    }
}

bool ItemsSortFilterModel::itemMatchesFilter(Item *item) const
{
    bool result = m_defaultSearchResult;
    if (!m_searchString.isEmpty()) {
        result = false;
        auto words = m_searchString.split(QRegExp("\\s+"),
                                          QString::SkipEmptyParts);
        auto itemMatches = [=](Item *item) {
            for (auto word : words) {
                if (item->title().indexOf(word, 0, Qt::CaseInsensitive) >= 0) {
                    return true;
                }
                auto complexItem = dynamic_cast<ComplexItem*>(item);
                if (complexItem != nullptr) {
                    if (complexItem->notes().indexOf(
                                word, 0, Qt::CaseInsensitive) >= 0) {
                        return true;
                    }
                }
            }
            return false;
        };
        if (itemMatches(item)) {
            result = true;
        } else {
            auto todos = todosOf(item);
            for (auto todo : todos) {
                if (itemMatches(todo)) {
                    result = true;
                    break;
                } else {
                    auto tasks = tasksOf(todo);
                    for (auto task : tasks) {
                        if (itemMatches(task)) {
                            result = true;
                            break;
                        }
                    }
                    if (result) {
                        break;
                    }
                }
            }
            if (!result) {
                auto tasks = tasksOf(item);
                for (auto task : tasks) {
                    if (itemMatches(task)) {
                        result = true;
                        break;
                    }
                }
            }
        }
    }
    return result;
}

QList<Todo *> ItemsSortFilterModel::todosOf(Item *item) const
{
    QList<Todo*> result;
    auto todoList = dynamic_cast<TodoList*>(item);
    if (todoList != nullptr) {
        auto library = todoList->library();
        if (library != nullptr) {
            auto todos = library->todos();
            for (int i = 0; i < todos->count(); ++i) {
                auto todo = dynamic_cast<Todo*>(todos->get(i));
                if (todo != nullptr && todo->todoListUid() == item->uid()) {
                    result << todo;
                }
            }
        }
    }
    return result;
}

QList<Task *> ItemsSortFilterModel::tasksOf(Item *item) const
{
    QList<Task*> result;
    auto todo = dynamic_cast<Todo*>(item);
    if (todo != nullptr) {
        auto library = todo->library();
        if (library != nullptr) {
            auto tasks = library->tasks();
            for (int i = 0; i < tasks->count(); ++i) {
                auto task = dynamic_cast<Task*>(tasks->get(i));
                if (task != nullptr && task->todoUid() == item->uid()) {
                    result << task;
                }
            }
        }
    }
    return result;
}


/**
 * @brief The default filter result in case no search string is set.
 */
bool ItemsSortFilterModel::defaultSearchResult() const
{
    return m_defaultSearchResult;
}


/**
 * @brief Set the default search string result.
 */
void ItemsSortFilterModel::setDefaultSearchResult(bool defaultSearchResult)
{
    if (m_defaultSearchResult != defaultSearchResult) {
        m_defaultSearchResult = defaultSearchResult;
        emit defaultSearchResultChanged();
        invalidateFilter();
    }
}


/**
 * @brief Set the search string for the model.
 *
 * This property holds the search string for the model. By default, it is
 * empty; in this case, visibility of items is determined by the
 * @sa defaultSearchResult property. Otherwise, only items are shown which
 * match the search string.
 */
QString ItemsSortFilterModel::searchString() const
{
    return m_searchString;
}


/**
 * @brief Set the search string.
 */
void ItemsSortFilterModel::setSearchString(const QString &searchString)
{
    if (m_searchString != searchString) {
        m_searchString = searchString;
        emit searchStringChanged();
        invalidateFilter();
    }
}
