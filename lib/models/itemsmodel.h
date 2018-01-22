#ifndef ITEMSMODEL_H
#define ITEMSMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QPointer>

#include "datastorage/itemcontainer.h"

/**
 * @brief A model working on a item container.
 *
 * This class implements the QAbstractListModel interface for items
 * contained in a ItemContainer.
 */
class ItemsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(ItemContainer* container READ container WRITE setContainer NOTIFY containerChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    enum Roles {
        ItemRole = Qt::UserRole,
        WeightRole
    };

    Q_ENUM(Roles)

    explicit ItemsModel(QObject *parent = 0);

    ItemContainer* container() const;
    void setContainer(ItemContainer *container);

    int count() const;

    // QAbstractItemModel interface
    int rowCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

signals:

    void containerChanged();
    void countChanged();

public slots:

private:

    QPointer<ItemContainer> m_container;

private slots:

    void itemAdded(int index);
    void itemDeleted(int index);
    void itemChanged(int index);
    void cleared();
};

#endif // ITEMSMODEL_H
