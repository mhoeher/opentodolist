#ifndef ITEMSMODEL_H
#define ITEMSMODEL_H

#include <QAbstractListModel>
#include <QHash>
#include <QObject>
#include <QPointer>
#include <QTimer>

#include "datamodel/item.h"
#include "datastorage/cache.h"

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

signals:

    void cacheChanged();
    void countChanged();
    void parentItemChanged();

public slots:

private:

    QPointer<Cache> m_cache;
    QHash<QUuid, Item*> m_items;
    QList<QUuid> m_ids;
    QTimer m_fetchTimer;
    QUuid m_parentItem;

private slots:

    void reset();
    void fetch();
    void triggerFetch();
    void update(QVariantList items);
    void itemChanged();
};

#endif // ITEMSMODEL_H
