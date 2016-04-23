#include "toplevelitemsmodel.h"

#include "library.h"

TopLevelItemsModel::TopLevelItemsModel(QObject* parent) :
    QAbstractListModel(parent)
{
}

Library* TopLevelItemsModel::library() const
{
    return m_library.data();
}

void TopLevelItemsModel::setLibrary(Library* library)
{
    if (m_library != library)
    {
        beginResetModel();
        if (!m_library.isNull())
        {
            disconnect(m_library.data(), &Library::itemChanged, this, &TopLevelItemsModel::itemChanged);    
            disconnect(m_library.data(), &Library::itemAdded, this, &TopLevelItemsModel::itemAdded);
            disconnect(m_library.data(), &Library::itemDeleted, this, &TopLevelItemsModel::itemDeleted);
        }
        m_library = library;
        if (!m_library.isNull())
        {
            connect(m_library.data(), &Library::itemChanged, this, &TopLevelItemsModel::itemChanged);    
            connect(m_library.data(), &Library::itemAdded, this, &TopLevelItemsModel::itemAdded);
            connect(m_library.data(), &Library::itemDeleted, this, &TopLevelItemsModel::itemDeleted);
        }
        endResetModel();
        emit libraryChanged();
    }
}

int TopLevelItemsModel::rowCount(const QModelIndex& parent) const
{
    int result = 0;
    if (!parent.isValid() && m_library)
    {
        result = m_library->items().length();
    }
    return result;
}

QVariant TopLevelItemsModel::data(const QModelIndex& index, int role) const
{
    QVariant result;
    if (!m_library.isNull() && index.isValid())
    {
        TopLevelItem *item = m_library->items().at(index.row());
        switch (role)
        {
        case Qt::DisplayRole:
            result = item->title();
            break;
        case ObjectRole:
            result = QVariant::fromValue<QObject*>(item);
            break;
        default:
            break;
        }
    }
    return result;
}

QHash<int, QByteArray> TopLevelItemsModel::roleNames() const
{
    auto result = QAbstractListModel::roleNames();
    result.insert(ObjectRole, "object");
    return result;
}

void TopLevelItemsModel::resetModel()
{
    beginResetModel();
    endResetModel();
}

void TopLevelItemsModel::itemChanged(TopLevelItem* item)
{
    if (!m_library.isNull())
    {
        for (int i = 0; i < m_library->items().length(); ++i)
        {
            if (item == m_library->items().at(i))
            {
                emit dataChanged(index(i, 0), index(i, 0));
                break;
            }
        }
    }
}

void TopLevelItemsModel::itemAdded()
{
    // TODO: Check why beginInsertRows() is not working
    /*if (!m_library.isNull())
    {
        beginInsertRows(QModelIndex(), 
                             m_library->items().length(), m_library->items().length());
        endInsertRows();
    }*/
    resetModel();
}

void TopLevelItemsModel::itemDeleted(int index)
{
    if (!m_library.isNull())
    {
        beginRemoveRows(QModelIndex(), index, index);
        endRemoveRows();
    }
}
