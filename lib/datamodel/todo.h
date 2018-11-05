#ifndef TODO_H
#define TODO_H

#include "complexitem.h"

#include <QObject>
#include <QPointer>


class TodoList;
class Task;

/**
 * @brief A single todo inside a todo list.
 *
 * A todo represents an item in a TodoList. Besides a title, due date and description, each todo
 * has a done state. Furthermore, a todo can have arbitrary many sub Task objects.
 */
class Todo : public ComplexItem
{
    Q_OBJECT

    Q_PROPERTY(bool done READ done WRITE setDone NOTIFY doneChanged)
    Q_PROPERTY(QUuid todoListUid READ todoListUid WRITE setTodoListUid NOTIFY todoListUidChanged)
    Q_PROPERTY(int percentageDone READ percentageDone NOTIFY percentageDoneChanged)

public:

    explicit Todo(const QString &filename, QObject *parent = nullptr);
    explicit Todo(QObject* parent = nullptr);
    explicit Todo(const QDir &dir, QObject *parent = nullptr);
    ~Todo() override;

    QUuid parentId() const override;

    bool done() const;
    void setDone(bool done);

    QUuid todoListUid() const;
    void setTodoListUid(const QUuid& todoListUid);

    int percentageDone() const;

    // Item interface
    void applyCalculatedProperties(const QVariantMap &properties) override;

signals:

    void doneChanged();
    void todoListUidChanged();
    void percentageDoneChanged();

public slots:

protected:


    QVariantMap toMap() const override;
    void fromMap(QVariantMap map) override;

private:

    QUuid m_todoListUid;
    int m_percentageDone;
    bool m_done;

};

typedef QSharedPointer<Todo> TodoPtr;

#endif // TODO_H
