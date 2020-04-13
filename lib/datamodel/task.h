#ifndef TASK_H
#define TASK_H

#include "item.h"

#include <QObject>

/**
 *  @brief Represents a task inside a todo.
 *
 * The Task class represents a single task inside a todo. A task simply is a line of text
 * and a flag indicating whether or not the task is done. Hence, tasks model a simple check list
 * within a todo.
 */
class Task : public Item
{
    Q_OBJECT

    Q_PROPERTY(bool done READ done WRITE setDone NOTIFY doneChanged)
    Q_PROPERTY(QUuid todoUid READ todoUid WRITE setTodoUid NOTIFY todoUidChanged)

public:
    explicit Task(QObject *parent = nullptr);
    explicit Task(const QString &filename, QObject *parent = nullptr);
    explicit Task(const QDir &dir, QObject *parent = nullptr);
    ~Task() override;

    QUuid parentId() const override;

    bool done() const;
    void setDone(bool done);

    QUuid todoUid() const;
    void setTodoUid(const QUuid &todoUid);

signals:

    void doneChanged();
    void todoUidChanged();

public slots:

private:
    bool m_done;
    QUuid m_todoUid;

protected:
    // Item interface
    QVariantMap toMap() const override;
    void fromMap(QVariantMap map) override;
};

typedef QSharedPointer<Task> TaskPtr;

#endif // TASK_H
