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

#include <QSet>
#include <QQmlEngine>

#include "datamodel/library.h"
#include "datastorage/cache.h"
#include "datastorage/librariesitemsquery.h"
#include "datastorage/insertorupdateitemsquery.h"
#include "models/librariesmodel.h"

/**
 * @brief Constructor.
 */
LibrariesModel::LibrariesModel(QObject* parent)
    : QAbstractListModel(parent), m_cache(nullptr), m_libraries(), m_uids(), m_updating(false)
{
    qRegisterMetaType<QList<Library*>>();
    connect(this, &LibrariesModel::rowsInserted, this, &LibrariesModel::countChanged);
    connect(this, &LibrariesModel::rowsRemoved, this, &LibrariesModel::countChanged);
}

/**
 * @brief The cache the model operates on.
 */
Cache* LibrariesModel::cache() const
{
    return m_cache.data();
}

/**
 * @brief Set the cache property.
 */
void LibrariesModel::setCache(Cache* cache)
{
    if (m_cache != cache) {
        if (m_cache) {
            disconnect(m_cache.data(), &Cache::dataChanged, this, &LibrariesModel::fetch);
        }
        m_cache = cache;
        if (m_cache) {
            connect(m_cache.data(), &Cache::dataChanged, this, &LibrariesModel::fetch);
        } else {
            beginResetModel();
            m_uids.clear();
            for (auto lib : m_libraries.values()) {
                delete lib;
            }
            m_libraries.clear();
        }
        fetch();
        emit cacheChanged();
    }
}

/**
 * @brief The number of libraries in the model.
 */
int LibrariesModel::count() const
{
    return rowCount();
}

/**
 * @brief Get the library for the given @p row.
 */
Library* LibrariesModel::get(int row) const
{
    if (row >= 0 && row < count()) {
        return m_libraries.value(m_uids.at(row));
    }
    return nullptr;
}

/**
 * @brief Returns the loaded libraries as a list.
 */
QList<Library*> LibrariesModel::libraries() const
{
    return m_libraries.values();
}

void LibrariesModel::fetch()
{
    if (m_cache) {
        auto q = new LibrariesItemsQuery();
        q->setIncludeCalculatedValues(true);
        connect(q, &LibrariesItemsQuery::librariesAvailable, this,
                &LibrariesModel::librariesAvailable);
        m_cache->run(q);
    }
}

void LibrariesModel::librariesAvailable(QVariantList libraries)
{
    m_updating = true;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    auto librariesToRemove = QSet<QUuid>(m_uids.begin(), m_uids.end());
#else
    auto librariesToRemove = QSet<QUuid>::fromList(m_uids);
#endif
    QList<Library*> newItems;
    for (auto entry : libraries) {
        auto lib = Library::decache(entry, this);
        auto uid = lib->uid();
        librariesToRemove.remove(uid);
        if (m_libraries.contains(uid)) {
            auto l = m_libraries.value(uid);
            l->fromVariant(lib->toVariant());
            l->applyCalculatedData(entry.value<LibraryCacheEntry>().calculatedData.toMap());
            delete lib;
        } else {
            newItems << lib;
        }
    }

    for (auto uid : librariesToRemove) {
        auto index = m_uids.indexOf(uid);
        beginRemoveRows(QModelIndex(), index, index);
        m_uids.removeOne(uid);
        auto lib = m_libraries.take(uid);
        delete lib;
        endRemoveRows();
    }

    if (!newItems.isEmpty()) {
        auto firstIndex = m_uids.length();
        auto lastIndex = firstIndex + newItems.length() - 1;
        beginInsertRows(QModelIndex(), firstIndex, lastIndex);
        for (auto lib : newItems) {
            QQmlEngine::setObjectOwnership(lib, QQmlEngine::CppOwnership);
            connect(lib, &Library::changed, this, &LibrariesModel::libraryChanged);
            m_uids.append(lib->uid());
            m_libraries.insert(lib->uid(), lib);
        }
        endInsertRows();
    }
    m_updating = false;
    emit updateFinished();
}

void LibrariesModel::libraryChanged()
{
    auto lib = qobject_cast<Library*>(sender());
    if (lib) {
        auto uid = lib->uid();
        auto idx = m_uids.indexOf(uid);
        if (idx >= 0) {
            emit dataChanged(index(idx), index(idx));
            if (!m_updating && m_cache != nullptr) {
                auto q = new InsertOrUpdateItemsQuery();
                q->add(lib, InsertOrUpdateItemsQuery::Save);
                m_cache->run(q);
            }
        }
    }
}

int LibrariesModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    } else {
        return m_uids.length();
    }
}

QVariant LibrariesModel::data(const QModelIndex& index, int role) const
{
    QVariant result;
    if (index.isValid()) {
        auto row = index.row();
        if (row < rowCount()) {
            auto uid = m_uids.at(row);
            auto lib = m_libraries.value(uid);
            switch (role) {
            case LibraryRole:
                result = QVariant::fromValue(lib);
                break;
            case NameRole:
                return lib->name();
            }
        }
    }
    return result;
}

QHash<int, QByteArray> LibrariesModel::roleNames() const
{
    auto result = QAbstractListModel::roleNames();
    result.insert(LibraryRole, "library");
    result.insert(NameRole, "name");
    return result;
}
