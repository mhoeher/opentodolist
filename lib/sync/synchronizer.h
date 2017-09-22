#ifndef SYNCHRONIZER_H
#define SYNCHRONIZER_H

#include <QLoggingCategory>
#include <QObject>
#include <QUuid>
#include <QVariantMap>


/**
 * @brief Encapsulate information about an existing library.
 */
class SynchronizerExistingLibrary {
    Q_GADGET

    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QString path READ path)
    Q_PROPERTY(QUuid uid READ uid)

public:

    explicit SynchronizerExistingLibrary();

    QString name() const;
    void setName(const QString &name);

    QString path() const;
    void setPath(const QString &path);


    QUuid uid() const;
    void setUid(const QUuid &uid);

private:

    QString m_name;
    QString m_path;
    QUuid   m_uid;
};

/**
 * @brief Base class for file synchronization.
 *
 * The Synchronizer class serves as abstract base class for file based
 * synchronization. It is intended to be used to synchronize the files of
 * a Library with an arbitrary backend service (such as a web service). The Synchronizer
 * provides the abstract interface required for such tasks. Concrete sub-classes are then
 * introduced to implement the actual synchronization with various backends.
 */
class Synchronizer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool validating READ validating NOTIFY validatingChanged)
    Q_PROPERTY(bool valid READ valid NOTIFY validChanged)
    Q_PROPERTY(bool synchronizing READ synchronizing NOTIFY synchronizingChanged)
    Q_PROPERTY(bool findingLibraries READ findingLibraries NOTIFY findingLibrariesChanged)
    Q_PROPERTY(QString directory READ directory WRITE setDirectory NOTIFY directoryChanged)
    Q_PROPERTY(bool isNull READ isNull NOTIFY directoryChanged)
    Q_PROPERTY(QVariantList existingLibraries READ existingLibraries NOTIFY existingLibrariesChanged)
public:

    static const QString SaveFileName;

    explicit Synchronizer(QObject *parent = 0);
    virtual ~Synchronizer();

    bool validating() const;
    bool valid() const;
    bool synchronizing() const;

    QString directory() const;
    void setDirectory(const QString& directory);
    bool isNull() const;

    bool save() const;
    void restore();

    static Synchronizer* fromDirectory(const QString &dir, QObject* parent = nullptr);

    /**
     * @brief Validate the connection to the backend.
     *
     * This method can be used to check if a connection to the backend used for
     * storing files can be established. Sub-classes are supposed to:
     *
     * 1. Set the validating property via @p setValidating() to true.
     * 2. Set the valid property via @p setValid to false.
     * 3. Test the connection to the backend.
     * 4. Set the valid property to true or false depending on the outcome of the test.
     * 5. Set the validating property to false.
     *
     * The method shall be implemented non-blocking.
     */
    Q_INVOKABLE virtual void validate() = 0;


    /**
     * @brief Synchronize the local directory with the backend.
     *
     * This method starts a new synchronization with with the backend. Sub-classes
     * are supposed to implement this method following these steps:
     */
    virtual void synchronize() = 0;

    Q_INVOKABLE virtual void findExistingLibraries();

    virtual QVariantMap toMap() const;
    virtual void fromMap(const QVariantMap &map);
    virtual QString secretsKey() const;
    virtual QString secret() const;
    virtual void setSecret(const QString& secret);


    QVariantList existingLibraries() const;

    bool findingLibraries() const;


    QUuid uid() const;

signals:

    void validatingChanged();
    void validChanged();
    void synchronizingChanged();
    void directoryChanged();
    void existingLibrariesChanged();
    void findingLibrariesChanged();

public slots:

protected:
    void setValidating(bool validating);
    void setValid(bool valid);
    void setSynchronizing(bool synchronizing);
    void beginValidation();
    void endValidation(bool valid);
    void setExistingLibraries(const QVariantList &existingLibraries);
    void setFindingLibraries(bool findingLibraries);

private:

    QUuid   m_uuid;
    bool    m_validating;
    bool    m_valid;
    bool    m_synchronizing;
    bool    m_creatingDirectory;
    bool    m_findingLibraries;
    QString m_directory;
    QVariantList
            m_existingLibraries;

};


Q_DECLARE_METATYPE(SynchronizerExistingLibrary)
Q_DECLARE_LOGGING_CATEGORY(synchronizer)


#endif // SYNCHRONIZER_H
