#ifndef ITEMSSORTFILTERMODEL_H
#define ITEMSSORTFILTERMODEL_H

#include <QJSValue>
#include <QObject>
#include <QSortFilterProxyModel>
#include <QUuid>

class Item;
class Task;
class Todo;

class ItemsSortFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QJSValue sortFunction READ sortFunction WRITE setSortFunction
               NOTIFY sortFunctionChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QString searchString READ searchString
               WRITE setSearchString NOTIFY searchStringChanged)
    Q_PROPERTY(bool defaultSearchResult READ defaultSearchResult
               WRITE setDefaultSearchResult NOTIFY defaultSearchResultChanged)
    Q_PROPERTY(QString tag READ tag WRITE setTag NOTIFY tagChanged)
    Q_PROPERTY(bool onlyDone READ onlyDone WRITE setOnlyDone
               NOTIFY onlyDoneChanged)
    Q_PROPERTY(bool onlyUndone READ onlyUndone WRITE setOnlyUndone
               NOTIFY onlyUndoneChanged)
    Q_PROPERTY(QUuid todoList READ todoList WRITE setTodoList
               NOTIFY todoListChanged)
    Q_PROPERTY(QUuid todo READ todo WRITE setTodo NOTIFY todoChanged)
public:
    explicit ItemsSortFilterModel(QObject *parent = 0);

    QJSValue filterFunction() const;
    void setFilterFunction(QJSValue filterFunction);

    QJSValue sortFunction() const;
    void setSortFunction(QJSValue sortFunction);

    int count() const;

    QString searchString() const;
    void setSearchString(const QString &searchString);

    bool defaultSearchResult() const;
    void setDefaultSearchResult(bool defaultSearchResult);

    QString tag() const;
    void setTag(const QString &tag);

    bool onlyDone() const;
    void setOnlyDone(bool onlyDone);

    bool onlyUndone() const;
    void setOnlyUndone(bool onlyUndone);

    QUuid todoList() const;
    void setTodoList(const QUuid &todoList);

    QUuid todo() const;
    void setTodo(const QUuid &todo);

signals:

    void filterFunctionChanged();
    void sortFunctionChanged();
    void countChanged();
    void searchStringChanged();
    void defaultSearchResultChanged();
    void tagChanged();
    void onlyDoneChanged();
    void onlyUndoneChanged();
    void todoListChanged();
    void todoChanged();

public slots:

protected:
    // QSortFilterProxyModel interface
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;
    bool lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const override;

private:

    mutable QJSValue m_sortFunction;
    QString m_searchString;
    bool    m_defaultSearchResult;
    QString m_tag;
    bool    m_onlyDone;
    bool    m_onlyUndone;
    QUuid   m_todoList;
    QUuid   m_todo;

    bool itemMatchesFilter(Item *item) const;
    QList<Todo*> todosOf(Item* item) const;
    QList<Task*> tasksOf(Item* item) const;

};

#endif // ITEMSSORTFILTERMODEL_H
