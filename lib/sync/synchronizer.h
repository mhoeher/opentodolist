#ifndef SYNCHRONIZER_H
#define SYNCHRONIZER_H

#include <QLoggingCategory>
#include <QObject>
#include <QVariantMap>


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
    Q_PROPERTY(QString directory READ directory WRITE setDirectory NOTIFY directoryChanged)
    Q_PROPERTY(bool isNull READ isNull NOTIFY directoryChanged)
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
    virtual void validate() = 0;


    /**
     * @brief Synchronize the local directory with the backend.
     *
     * This method starts a new synchronization with with the backend. Sub-classes
     * are supposed to implement this method following these steps:
     */
    virtual void synchronize() = 0;

    virtual QVariantMap toMap() const;
    virtual void fromMap(const QVariantMap &map);


signals:

    void validatingChanged();
    void validChanged();
    void synchronizingChanged();
    void directoryChanged();

public slots:

protected:
    void setValidating(bool validating);
    void setValid(bool valid);
    void setSynchronizing(bool synchronizing);
    void beginValidation();
    void endValidation(bool valid);

private:

    bool    m_validating;
    bool    m_valid;
    bool    m_synchronizing;
    bool    m_creatingDirectory;
    QString m_directory;

};


Q_DECLARE_LOGGING_CATEGORY(synchronizer)


#endif // SYNCHRONIZER_H
