#ifndef OPENTODOLISTCOREINTERFACES_H
#define OPENTODOLISTCOREINTERFACES_H

#include <QObject>
#include <QUuid>

// forward declarations
class ITodo;
class ITodoList;
class ITodoListFactory;

class ITodo
{

public:

    /**
       @brief Virtual destructor

       This is required to keep some compilers from complaining
     */
    virtual ~Todo() {}

    /**
       @brief The unique ID of the todo

       This should return the (globally) unique ID of the todo.
     */
    virtual QUuid id() const = 0;

    /**
       @brief The title of the todo
     */
    virtual QString title() const = 0;

    /**
       @brief Sets the todo's title
     */
    virtual void setTitle(const QString &title) = 0;

    /**
       @brief The description of the todo

       This returns the description of the todo. This is an HTML string which
       provides more information about the todo.
     */
    virtual QString description() const = 0;

    /**
       @brief Sets the description of the todo
     */
    void setDescription(const QString &description);

    virtual int progress() const = 0;
    virtual void setProgress(int progress) = 0;

    virtual int priority() const = 0;
    virtual void setPriority(int priority) = 0;

    virtual ITodo* parentTodo() const = 0;
    virtual void setParentTodo( ITodo* parentTodo ) = 0;

    virtual bool isDeleted() const = 0;
    virtual void setDeleted( bool deleted ) = 0;

    virtual QDateTime dueDate() const = 0;
    virtual void setDueDate( const QDateTime& dateTime ) = 0;

    virtual ITodoList* parent() const = 0;

    virtual bool canMoveToTodo(ITodo *newParent ) const = 0;

    virtual void cloneFrom( ITodo *otherTodo ) = 0;

signals:

    /**
       @brief The todo is about to be disposed

       This signal is emitted by a todo as soon as it is about to be
       disposed. The @p todo parameter carries along the todo itself.
       @param todo
     */
    void aboutToBeDisposed( QObject* todo );

public slots:

    void toggleCompleted( int newProgress = -1 );
    void dispose();

protected:

    void setId( QUuid id );

    /**
       @brief Destroies the todo

       Calling this method will cause the todo object being destroyed.
       This method will do nothing. Sub-classes can override it to do cleanup
       (e.g. removing locally allocated resources that were used to
       persist the todo).
     */
    virtual void destroy() {}

private:

    QUuid               m_id;
    QString             m_title;
    QString             m_description;
    int                 m_progress;
    int                 m_priority;
    Todo        *m_parentTodo;
    bool                m_deleted;
    QDateTime           m_dueDate;

    int                 m_lastProgress;

    TodoSortFilterModel *m_subTodosModel;

    QObject* todoList() const {
        return QObject::parent();
    }

private slots:

    void childDataChanged();

};

class OPENTODOLISTCORESHARED_EXPORT TodoList : public QObject
{

    Q_OBJECT

    Q_PROPERTY( QString name READ name WRITE setName NOTIFY nameChanged )
    Q_PROPERTY( QString displayName READ displayName NOTIFY nameChanged )
    Q_PROPERTY( QObject* todos READ todos CONSTANT )
    Q_PROPERTY( QObject* entries READ topLevelTodos CONSTANT )
    Q_PROPERTY( QObject* deletedTodos READ deletedTodos CONSTANT )
    Q_PROPERTY( QString type READ type CONSTANT )
    Q_PROPERTY( QString key READ key CONSTANT )

public:

    typedef ObjectModel< Todo > TodosList;

    explicit TodoList( const QString& key, const QString &type, const QVariant &settings = QVariant(), QObject *parent = 0);

    TodosList *todos() const;
    TodoSortFilterModel* topLevelTodos() const;
    TodoSortFilterModel* deletedTodos() const;
    Q_INVOKABLE virtual QObject *addTodo();
    Q_INVOKABLE QObject* addTodo( const QString& title, QObject* parentTodo );
    virtual QVariant settings();


    const QString& name() const;
    void setName( const QString& name );

    QString displayName() const;

    const QString& type() const {
        return m_type;
    }

    const QString& key() const {
        return m_key;
    }

signals:

    void nameChanged();

    void changed();

public slots:

protected:

    void appendTodo( Todo* todo );

private:

    TodosList *m_todos;
    TodoSortFilterModel *m_topLevelTodos;
    TodoSortFilterModel *m_deletedTodos;
    QString m_name;
    QString m_type;
    QString m_key;
    QString m_dir;

private slots:

    void todoParentChanged();

};

class OPENTODOLISTCORESHARED_EXPORT TodoListFactory : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString type READ type CONSTANT )
public:
    explicit TodoListFactory(const QString& type, QObject *parent = 0);

    /**
       @brief Creates a new todo list

       This will create a new todo list or restore one. The todo list will be
       made a child of the given @p parent. In case of a new list is created,
       both @p key and @p settings will be invalid values. Upon restoration,
       the key will be set to the previously generated key of the todo list and
       settings will be set to whatever has been returned from the
       AbstractTodoList::settings() method.
     */
    virtual TodoList* createTodoList( QObject *parent = 0,
                                              const QString& key = QString(),
                                              const QVariant &settings = QVariant() ) const = 0;

    const QString& type() const {
        return m_type;
    }

#ifdef Q_OS_ANDROID
    static QString androidExtStorageLocation();
#endif
    static QString localStorageLocation( const QString &type );
    QString localStorageLocation() const;

signals:

public slots:

private:

    QString m_type;

};
#endif // OPENTODOLISTCOREINTERFACES_H
