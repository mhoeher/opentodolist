#ifndef TODOSMODEL_H
#define TODOSMODEL_H

#include <QPointer>

#include "abstractitemmodel.h"

// Forward:
class TodoList;

/**
   @brief Provides access to the todos stored in a TodoList object.
 */
class TodosModel : public AbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(TodoList* todoList READ todoList WRITE setTodoList NOTIFY todoListChanged)
    
public:
    
    enum Roles {
        ObjectRole = Qt::UserRole + 1,
        TodoRole,
        DoneRole,
        TitleRole,
        NotesRole
    };
    
    Q_ENUM(Roles)

    explicit TodosModel(QObject *parent = 0);
    virtual ~TodosModel();
    
    TodoList *todoList() const;
    void setTodoList(TodoList *todoList);
    
    // Basic functionality:
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    
    QHash<int, QByteArray> roleNames() const override;
    
    
signals:
    
    /**
       @brief The todoList property has changed.
     */
    void todoListChanged();
    
private:
    
    QPointer<TodoList> m_todoList;
    
private slots:
    
    void resetModel();
    
};

#endif // TODOSMODEL_H
