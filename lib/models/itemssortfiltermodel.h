#ifndef ITEMSSORTFILTERMODEL_H
#define ITEMSSORTFILTERMODEL_H

#include <QDateTime>
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
    Q_PROPERTY(QDateTime minDueDate READ minDueDate WRITE setMinDueDate
               NOTIFY minDueDateChanged)
    Q_PROPERTY(QDateTime maxDueDate READ maxDueDate WRITE setMaxDueDate
               NOTIFY maxDueDateChanged)
public:
    explicit ItemsSortFilterModel(QObject *parent = nullptr);

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

    QDateTime minDueDate() const;
    void setMinDueDate(const QDateTime &minDueDate);

    QDateTime maxDueDate() const;
    void setMaxDueDate(const QDateTime &maxDueDate);

signals:

    void countChanged();
    void searchStringChanged();
    void defaultSearchResultChanged();
    void tagChanged();
    void onlyDoneChanged();
    void onlyUndoneChanged();
    void todoListChanged();
    void todoChanged();
    void minDueDateChanged();
    void maxDueDateChanged();

public slots:

protected:
    // QSortFilterProxyModel interface
    bool filterAcceptsRow(int source_row, const QModelIndex& source_parent) const override;

private:

    QString m_searchString;
    bool    m_defaultSearchResult;
    QString m_tag;
    bool    m_onlyDone;
    bool    m_onlyUndone;
    QUuid   m_todoList;
    QUuid   m_todo;
    QDateTime m_minDueDate;
    QDateTime m_maxDueDate;

    bool itemMatchesFilter(Item *item) const;
    QList<Todo*> todosOf(Item* item) const;
    QList<Task*> tasksOf(Item* item) const;

};

#endif // ITEMSSORTFILTERMODEL_H
