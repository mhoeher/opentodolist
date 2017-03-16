#ifndef LIBRARY_H
#define LIBRARY_H

#include <QObject>
#include <QString>
#include <QVariantMap>

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

public:

    static const QString LibraryFileName;

    explicit Library(QObject *parent = nullptr);
    explicit Library(const QString &directory, QObject* parent = nullptr);
    virtual ~Library();


    bool isValid() const { return !m_directory.isEmpty(); }

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

    QVariantMap toMap() const;
    void fromMap(QVariantMap map);

signals:

    /**
     * @brief The name of the library changed.
     */
    void nameChanged();

    /**
     * @brief The library is deleted.
     */
    void libraryDeleted(Library *library);

private:

    QString                 m_name;
    QString                 m_directory;

};

#endif // LIBRARY_H
