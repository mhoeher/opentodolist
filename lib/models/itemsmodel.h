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

#ifndef MODELS_ITEMSMODEL_H_
#define MODELS_ITEMSMODEL_H_

#include <QAbstractListModel>
#include <QDateTime>
#include <QHash>
#include <QObject>
#include <QPointer>
#include <QTimer>

#include "datamodel/item.h"
#include "datastorage/cache.h"

class GetItemsQuery;

/**
 * @brief A model working on a Cache.
 *
 * This class allows to access items stored in a Cache. It allows some
 * basic filtering to limit the amount of data shown, e.g. only
 * direkt children of a particular item or from a library can be
 * shown.
 */
class ItemsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(Cache* cache READ cache WRITE setCache NOTIFY cacheChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QUuid parentItem READ parentItem WRITE setParentItem NOTIFY parentItemChanged)
    Q_PROPERTY(
            QString searchString READ searchString WRITE setSearchString NOTIFY searchStringChanged)
    Q_PROPERTY(QString tag READ tag WRITE setTag NOTIFY tagChanged)
    Q_PROPERTY(bool onlyDone READ onlyDone WRITE setOnlyDone NOTIFY onlyDoneChanged)
    Q_PROPERTY(bool onlyUndone READ onlyUndone WRITE setOnlyUndone NOTIFY onlyUndoneChanged)
    Q_PROPERTY(bool onlyWithDueDate READ onlyWithDueDate WRITE setOnlyWithDueDate NOTIFY
                       onlyWithDueDateChanged)
    Q_PROPERTY(bool defaultSearchResult READ defaultSearchResult WRITE setDefaultSearchResult NOTIFY
                       defaultSearchResultChanged)
    Q_PROPERTY(bool recursive READ recursive WRITE setRecursive NOTIFY recursiveChanged)
    Q_PROPERTY(
            QString overdueLabel READ overdueLabel WRITE setOverdueLabel NOTIFY overdueLabelChanged)
    Q_PROPERTY(QVariantMap timeSpans READ timeSpans WRITE setTimeSpans NOTIFY timeSpansChanged)
    Q_PROPERTY(QString itemType READ itemType WRITE setItemType NOTIFY itemTypeChanged)
public:
    enum Roles {
        ItemRole = Qt::UserRole,
        WeightRole,
        DueToRole,
        DueToSpanRole,
        TitleRole,
        CreatedAtRole,
        UpdatedAtRole,
        EffectiveDueToRole,
        EffectiveDueToSpanRole,
        UidRole
    };

    Q_ENUM(Roles)

    explicit ItemsModel(QObject* parent = nullptr);

    Cache* cache() const;
    void setCache(Cache* cache);

    int count() const;

    // QAbstractItemModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    QUuid parentItem() const;
    void setParentItem(const QUuid& parent);

    QString searchString() const;
    void setSearchString(const QString& searchString);

    QString tag() const;
    void setTag(const QString& tag);

    bool onlyDone() const;
    void setOnlyDone(bool value);

    bool onlyUndone() const;
    void setOnlyUndone(bool value);

    bool onlyWithDueDate() const;
    void setOnlyWithDueDate(bool value);

    bool defaultSearchResult() const;
    void setDefaultSearchResult(bool defaultSearchResult);

    bool recursive() const;
    void setRecursive(bool recursive);

    QString overdueLabel() const;
    void setOverdueLabel(const QString& overdueLabel);

    QVariantMap timeSpans() const;
    void setTimeSpans(const QVariantMap& timeSpans);

    Q_INVOKABLE int roleFromName(const QString& roleName) const;

    QString itemType() const;
    void setItemType(const QString& itemType);

    const QList<QUuid>& itemsToExclude() const;
    void setItemsToExclude(const QList<QUuid>& newItemsToExclude);

signals:

    void cacheChanged();
    void countChanged();
    void parentItemChanged();
    void searchStringChanged();
    void tagChanged();
    void onlyDoneChanged();
    void onlyUndoneChanged();
    void onlyWithDueDateChanged();
    void defaultSearchResultChanged();
    void recursiveChanged();
    void overdueLabelChanged();
    void timeSpansChanged();
    void itemTypeChanged();

    void itemsToExcludeChanged();

public slots:

private:
    QPointer<Cache> m_cache;
    QHash<QUuid, Item*> m_items;
    QList<QUuid> m_ids;
    QTimer m_fetchTimer;
    QUuid m_parentItem;
    QList<QUuid> m_itemsToExclude;

    QString m_searchString;
    QString m_tag;
    QString m_itemType;
    bool m_onlyDone;
    bool m_onlyUndone;
    bool m_onlyWithDueDate;
    bool m_defaultSearchResult;
    bool m_recursive;

    bool m_updating;

    QString m_overdueLabel;
    QVariantMap m_timeSpans;

    static bool itemMatches(ItemPtr item, QStringList words);

    QString timeSpanLabel(Item* item, int role) const;

    Q_PROPERTY(QList<QUuid> itemsToExclude READ itemsToExclude WRITE setItemsToExclude NOTIFY
                       itemsToExcludeChanged)

private slots:

    void reset();
    void fetch();
    std::function<bool(ItemPtr item, GetItemsQuery* query)> getFilterFn() const;
    void triggerFetch();
    void update(QVariantList items);
    void itemChanged();
};

#endif // MODELS_ITEMSMODEL_H_
