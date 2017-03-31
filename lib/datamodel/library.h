#ifndef LIBRARY_H
#define LIBRARY_H

#include <QDir>
#include <QObject>
#include <QString>
#include <QVariantMap>

#include "image.h"
#include "itemcontainer.h"
#include "note.h"
#include "todolist.h"

class DirectoryWatcher;

/**
 * @brief A container for items.
 *
 * The Library class is a container for TopLevelItem objects. Each library has a directory, for
 * which it loads and makes the contained items available.
 */
class Library : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isValid READ isValid CONSTANT)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString directory READ directory CONSTANT)
    Q_PROPERTY(ItemContainer* topLevelItems READ topLevelItems CONSTANT)
    Q_PROPERTY(ItemContainer* todos READ todos CONSTANT)
    Q_PROPERTY(ItemContainer* tasks READ tasks CONSTANT)

public:

    static const QString LibraryFileName;

    explicit Library(QObject *parent = nullptr);
    explicit Library(const QString &directory, QObject* parent = nullptr);
    virtual ~Library();

    Q_INVOKABLE Note* addNote();
    Q_INVOKABLE Image* addImage();
    Q_INVOKABLE TodoList* addTodoList();


    bool isValid() const;

    /**
     * @brief The name of the libary as used in the user interface.
     */
    QString name() const      { return m_name; }
    void setName( const QString &name );

    /**
     * @brief Returns the directory which the library encapsulates.
     */
    QString directory() const { return m_directory; }

    Q_INVOKABLE void deleteLibrary();
    Q_INVOKABLE bool load();
    Q_INVOKABLE bool save();

    ItemContainer *topLevelItems();
    ItemContainer *todos();
    ItemContainer *tasks();

    QString newItemLocation() const;

signals:

    /**
     * @brief The name of the library changed.
     */
    void nameChanged();

    /**
     * @brief The library is deleted.
     */
    void libraryDeleted(Library *library);

    /**
     * @brief Loading the items of the library finished.
     *
     * This signal is emitted to indicate that loading the items in a library
     * has finished.
     */
    void loadingFinished();

private:

    QString                 m_name;
    QString                 m_directory;

    ItemContainer           m_topLevelItems;
    ItemContainer           m_todos;
    ItemContainer           m_tasks;

    DirectoryWatcher       *m_directoryWatcher;

    bool                    m_loading;

    QVariantMap toMap() const;
    void fromMap(QVariantMap map);

private slots:

    void appendItem(ItemPtr item);

};

#endif // LIBRARY_H
