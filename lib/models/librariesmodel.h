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

#ifndef MODELS_LIBRARIESMODEL_H_
#define MODELS_LIBRARIESMODEL_H_

#include <QAbstractListModel>
#include <QHash>
#include <QObject>
#include <QPointer>
#include <QUuid>

class Cache;
class Library;

/**
 * @brief A model which makes the libraries available.
 *
 * This class implements a model which make the libraries defined in an
 * application's cache available.
 */
class LibrariesModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(Cache* cache READ cache WRITE setCache NOTIFY cacheChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    enum Roles { LibraryRole = Qt::UserRole, NameRole };
    Q_ENUM(Roles);

    explicit LibrariesModel(QObject* parent = nullptr);

    Cache* cache() const;
    void setCache(Cache* cache);

    int count() const;
    Q_INVOKABLE Library* get(int row) const;

    // QAbstractItemModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

signals:

    /**
     * @brief The cache property has changed.
     */
    void cacheChanged();

    /**
     * @brief The model finished updating from the cache.
     */
    void updateFinished();

    /**
     * @brief The number of libraries changed.
     */
    void countChanged();

public slots:

private:
    QPointer<Cache> m_cache;
    QHash<QUuid, Library*> m_libraries;
    QList<QUuid> m_uids;
    bool m_updating;

    void fetch();

private slots:

    void librariesAvailable(QVariantList libraries);
    void libraryChanged();
};

#endif // MODELS_LIBRARIESMODEL_H_
