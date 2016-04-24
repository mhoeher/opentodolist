#include "todosmodel.h"

#include "todolist.h"
#include "todo.h"

TodosModel::TodosModel(QObject *parent)
    : AbstractListModel(parent),
      m_todoList(nullptr)
{
}

TodosModel::~TodosModel()
{
}

TodoList*TodosModel::todoList() const
{
    return m_todoList.data();
}

void TodosModel::setTodoList(TodoList* todoList)
{
    if (m_todoList.data() != todoList) 
    {
        beginResetModel();
        if (m_todoList) 
        {
            disconnect(m_todoList.data(), &TodoList::todoAdded, this, &TodosModel::todoAdded);
            disconnect(m_todoList.data(), &TodoList::todoDeleted, this, &TodosModel::todoDeleted);
            disconnect(m_todoList.data(), &TodoList::todoChanged, this, &TodosModel::todoChanged);
        }
        m_todoList = todoList;
        if (m_todoList) 
        {
            connect(m_todoList.data(), &TodoList::todoDeleted, this, &TodosModel::todoDeleted);
            connect(m_todoList.data(), &TodoList::todoAdded, this, &TodosModel::todoAdded);
            connect(m_todoList.data(), &TodoList::todoChanged, this, &TodosModel::todoChanged);
        }
        endResetModel();
        emit todoListChanged();
    }
}

QModelIndex TodosModel::index(int row, int column, const QModelIndex &parent) const
{
    if (m_todoList && !parent.isValid())
    {
        if (row >= 0 && row < m_todoList->todos().length())
        {
            return createIndex(row, column, m_todoList->todos().at(row));
        }
    }
    return QModelIndex();
}

int TodosModel::rowCount(const QModelIndex &parent) const
{
    int result = 0;
    if (m_todoList && !parent.isValid())
    {
        result = m_todoList->todos().length();
    }
    return result;
}

QVariant TodosModel::data(const QModelIndex &index, int role) const
{
    if (index.isValid())
    {
        Todo *todo = reinterpret_cast<Todo*>(index.internalPointer());
        switch (role)
        {
        case Qt::DisplayRole:
        case TitleRole:
            return todo->title();
        case ObjectRole:
        case TodoRole:
            return QVariant::fromValue<QObject*>(todo);
        case DoneRole:
            return todo->done();
        case NotesRole:
            return todo->notes();
        default:
            break;
        }
    }
    return QVariant();
}

QHash<int, QByteArray> TodosModel::roleNames() const
{
    auto result = AbstractListModel::roleNames();
    result.insert(ObjectRole, "object");
    result.insert(TodoRole, "todo");
    result.insert(DoneRole, "done");
    result.insert(TitleRole, "title");
    result.insert(NotesRole, "notes");
    return result;
}

void TodosModel::resetModel()
{
    beginResetModel();
    endResetModel();
}

void TodosModel::todoChanged(Todo* item)
{
    if (!m_todoList.isNull())
    {
        for (int i = 0; i < m_todoList->todos().length(); ++i)
        {
            if (item == m_todoList->todos().at(i))
            {
                auto index = this->index(i, 0);
                emit dataChanged(index, index);
                break;
            }
        }
    }
}

void TodosModel::todoAdded()
{
    if (!m_todoList.isNull())
    {
        auto addIndex = m_todoList->todos().length() - 1;
        beginInsertRows(QModelIndex(), addIndex, addIndex);
        endInsertRows();
    }
}

void TodosModel::todoDeleted(int index)
{
    if (!m_todoList.isNull())
    {
        beginRemoveRows(QModelIndex(), index, index);
        endRemoveRows();
    }
}
