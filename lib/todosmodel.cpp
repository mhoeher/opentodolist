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
            disconnect(m_todoList.data(), &TodoList::todosChanged, this, &TodosModel::resetModel);
            disconnect(m_todoList.data(), &TodoList::todosPropertiesChanged, this, &TodosModel::itemPropertiesChanged);
        }
        m_todoList = todoList;
        if (m_todoList) 
        {
            connect(m_todoList.data(), &TodoList::todosChanged, this, &TodosModel::resetModel);
            connect(m_todoList.data(), &TodoList::todosPropertiesChanged, this, &TodosModel::itemPropertiesChanged);
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
