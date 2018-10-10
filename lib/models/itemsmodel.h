#ifndef ITEMSMODEL_H
#define ITEMSMODEL_H

#include <QAbstractListModel>
#include <QHash>
#include <QObject>
#include <QPointer>
#include <QTimer>

#include "datamodel/item.h"
#include "datastorage/cache.h"

class GetItemsQuery;

/**
 * @brief A model working on a item container.
 *
 * This class implements the QAbstractListModel interface for items
 * contained in a ItemContainer.
 */
class ItemsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(Cache* cache READ cache WRITE setCache NOTIFY cacheChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QUuid parentItem READ parentItem WRITE setParentItem NOTIFY parentItemChanged)
    Q_PROPERTY(QString searchString READ searchString WRITE setSearchString NOTIFY searchStringChanged)
    Q_PROPERTY(QString tag READ tag WRITE setTag NOTIFY tagChanged)
    Q_PROPERTY(bool onlyDone READ onlyDone WRITE setOnlyDone NOTIFY onlyDoneChanged)
    Q_PROPERTY(bool onlyUndone READ onlyUndone WRITE setOnlyUndone NOTIFY onlyUndoneChanged)
    Q_PROPERTY(bool onlyWithDueDate READ onlyWithDueDate WRITE setOnlyWithDueDate NOTIFY onlyWithDueDateChanged)
    Q_PROPERTY(bool defaultSearchResult READ defaultSearchResult WRITE setDefaultSearchResult NOTIFY defaultSearchResultChanged)
public:
    enum Roles {
        ItemRole = Qt::UserRole,
        WeightRole
    };

    Q_ENUM(Roles)

    explicit ItemsModel(QObject *parent = nullptr);

    Cache* cache() const;
    void setCache(Cache *cache);

    int count() const;

    // QAbstractItemModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    QUuid parentItem() const;
    void setParentItem(const QUuid &parent);

    QString searchString() const;
    void setSearchString(const QString &searchString);

    QString tag() const;
    void setTag(const QString &tag);

    bool onlyDone() const;
    void setOnlyDone(bool value);

    bool onlyUndone() const;
    void setOnlyUndone(bool value);

    bool onlyWithDueDate() const;
    void setOnlyWithDueDate(bool value);

    bool defaultSearchResult() const;
    void setDefaultSearchResult(bool defaultSearchResult);

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

public slots:

private:

    QPointer<Cache> m_cache;
    QHash<QUuid, Item*> m_items;
    QList<QUuid> m_ids;
    QTimer m_fetchTimer;
    QUuid m_parentItem;

    QString m_searchString;
    QString m_tag;
    bool m_onlyDone;
    bool m_onlyUndone;
    bool m_onlyWithDueDate;
    bool m_defaultSearchResult;

    static bool itemMatches(ItemPtr item, QStringList words);

private slots:

    void reset();
    void fetch();
    std::function<bool(ItemPtr item, GetItemsQuery* query)> getFilterFn() const;
    void triggerFetch();
    void update(QVariantList items);
    void itemChanged();
};

#endif // ITEMSMODEL_H
