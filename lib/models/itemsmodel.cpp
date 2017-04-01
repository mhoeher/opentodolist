#include "itemsmodel.h"

#include <QQmlEngine>

ItemsModel::ItemsModel(QObject *parent) :
    QAbstractListModel(parent),
    m_container()
{

}

/**
 * @brief The item container the model works on.
 */
ItemContainer*ItemsModel::container() const
{
    return m_container.data();
}

/**
 * @brief Set the item container.
 */
void ItemsModel::setContainer(ItemContainer* container)
{
    if (container != m_container) {
        if (m_container != nullptr) {
            disconnect(m_container.data(), &ItemContainer::itemAdded,
                       this, &ItemsModel::itemAdded);
            disconnect(m_container.data(), &ItemContainer::itemDeleted,
                       this, &ItemsModel::itemDeleted);
            disconnect(m_container.data(), &ItemContainer::cleared,
                       this, &ItemsModel::cleared);
        }
        beginResetModel();
        m_container = container;
        endResetModel();
        if (m_container != nullptr) {
            connect(m_container.data(), &ItemContainer::itemAdded,
                    this, &ItemsModel::itemAdded);
            connect(m_container.data(), &ItemContainer::itemDeleted,
                    this, &ItemsModel::itemDeleted);
            connect(m_container.data(), &ItemContainer::cleared,
                    this, &ItemsModel::cleared);
        }
        emit containerChanged();
    }
}

int ItemsModel::rowCount(const QModelIndex& parent) const
{
    if (m_container && !parent.isValid()) {
        return m_container->count();
    } else {
        return 0;
    }
}

QVariant ItemsModel::data(const QModelIndex& index, int role) const
{
    int row = index.row();
    if (m_container && row < m_container->count()) {
        auto item = m_container->item(row);
        switch (role) {
        case Qt::DisplayRole:
        case ItemRole:
            QQmlEngine::setObjectOwnership(item.data(), QQmlEngine::CppOwnership);
            return QVariant::fromValue<QObject*>(item.data());
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
    return result;
}

void ItemsModel::itemAdded(int index)
{
    beginInsertRows(QModelIndex(), index, index);
    endInsertRows();
}

void ItemsModel::itemDeleted(int index)
{
    beginRemoveRows(QModelIndex(), index, index);
    endRemoveRows();
}

void ItemsModel::cleared()
{
    beginResetModel();
    endResetModel();
}
