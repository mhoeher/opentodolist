#ifndef LIBRARY_H
#define LIBRARY_H

#include <functional>

#include <QDir>
#include <QLoggingCategory>
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
    Q_PROPERTY(QUuid uid READ uid WRITE setUid NOTIFY uidChanged)
    Q_PROPERTY(bool isValid READ isValid CONSTANT)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QStringList tags READ tags NOTIFY tagsChanged)
    Q_PROPERTY(QString directory READ directory CONSTANT)
    Q_PROPERTY(ItemContainer* topLevelItems READ topLevelItems CONSTANT)
    Q_PROPERTY(ItemContainer* todos READ todos CONSTANT)
    Q_PROPERTY(ItemContainer* tasks READ tasks CONSTANT)
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)

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

    Q_INVOKABLE void deleteLibrary(bool deleteFiles = false);
    void deleteLibrary(bool deleteFiles, std::function<void ()> callback);
    Q_INVOKABLE bool load();
    Q_INVOKABLE bool save();

    ItemContainer *topLevelItems();
    ItemContainer *todos();
    ItemContainer *tasks();

    QString newItemLocation() const;

    static QStringList years(const QString &directory);
    static QStringList months(const QString &directory, const QString &year);

    bool loading() const;
    QUuid uid() const;
    QStringList tags() const;

signals:

    /**
     * @brief The UID of the library has changed.
     */
    void uidChanged();

    /**
     * @brief The name of the library changed.
     */
    void nameChanged();

    /**
     * @brief The list of tags changed.
     */
    void tagsChanged();

    /**
     * @brief The library is deleted.
     */
    void libraryDeleted(Library *library);

    void loadingChanged();

    /**
     * @brief Loading the items of the library finished.
     *
     * This signal is emitted to indicate that loading the items in a library
     * has finished.
     */
    void loadingFinished();

private:

    QUuid                   m_uid;
    QString                 m_name;
    QString                 m_directory;

    ItemContainer           m_topLevelItems;
    ItemContainer           m_todos;
    ItemContainer           m_tasks;

    DirectoryWatcher       *m_directoryWatcher;

    bool                    m_loading;

    QVariantMap toMap() const;
    void fromMap(QVariantMap map);

    void setUid(const QUuid& uid);
    void setLoading(bool loading);

private slots:

    void appendItem(ItemPtr item);

};

Q_DECLARE_LOGGING_CATEGORY(library);

#endif // LIBRARY_H
