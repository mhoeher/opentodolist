#include "synchronizer.h"

#include "genericdavsynchronizer.h"
#include "nextcloudsynchronizer.h"

#include <functional>

#include <QDir>
#include <QFile>
#include <QMap>
#include <QMetaObject>

#include "utils/jsonutils.h"


Q_LOGGING_CATEGORY(synchronizer, "net.rpdev.opentodolist.Synchronizer", QtDebugMsg)


/**
 * @brief The default file name for storing synchronizer settings.
 */
const QString Synchronizer::SaveFileName = ".opentodolist.synchronizer";


/**
 * @brief Constructor.
 *
 * This creates a new synchronizer. Synchronizers are initially invalid (indicated by the
 * @sa isNull property). The @sa setDirectory method must be used to point the synchronizer
 * to an existing local directory.
 */
Synchronizer::Synchronizer(QObject *parent) : QObject(parent),
    m_validating(false),
    m_valid(false),
    m_synchronizing(false),
    m_directory()
{
}


/**
 * @brief Destructor.
 */
Synchronizer::~Synchronizer()
{
}


/**
 * @brief A validating is currently running.
 *
 * The validating property indicates, that currently the synchronizer checks if
 * a connection to the backend can be established. As soon as the validation is done, the
 * @sa valid property will be set to indicate the status fof the validation.
 *
 * @sa setValidating
 */
bool Synchronizer::validating() const
{
    return m_validating;
}


/**
 * @brief Synchronizer::setValidating
 *
 * Set the validating property. This is intended to be used by sub-classes.
 */
void Synchronizer::setValidating(bool validating)
{
    if (m_validating != validating) {
        m_validating = validating;
        emit validatingChanged();
    }
}


/**
 * @brief Indicates whether a connection to the backend can be established.
 *
 * This property is set to true to indicate that a connection to the backend can be
 * established.
 *
 * @sa setValid
 */
bool Synchronizer::valid() const
{
    return m_valid;
}


/**
 * @brief Set the valid property.
 *
 * This method is supposed to be used by sub-classes in their implementation of the
 * @p validate() method.
 */
void Synchronizer::setValid(bool valid)
{
    if (m_valid != valid) {
        m_valid = valid;
        emit validChanged();
    }
}


/**
 * @brief A synchronization is currently running.
 *
 * The synchronizing property indicated whether currently a sync is running.
 *
 * @sa setSynchronizing
 */
bool Synchronizer::synchronizing() const
{
    return m_synchronizing;
}


/**
 * @brief Set the synchronizing property.
 *
 * Sub-classes are supposed to use this method in their implementation of the
 * synchronize() method to set the synchronizing property.
 */
void Synchronizer::setSynchronizing(bool synchronizing)
{
    if (m_synchronizing != synchronizing) {
        m_synchronizing = synchronizing;
        emit synchronizingChanged();
    }
}

/**
 * @brief Begin validation.
 *
 * Begin the validation process. This is a utility method which sets
 * the validating property to true and the valid property to false.
 *
 * @sa setValidating
 * @sa setValid
 * @sa endValidation
 */
void Synchronizer::beginValidation()
{
    setValidating(true);
    setValid(false);
}

/**
 * @brief End validation.
 *
 * This is a utility method which sets the validating property to false and the valid
 * property to @p valid.
 *
 * @sa setValidating
 * @sa setValid
 * @sa beginValidation
 */
void Synchronizer::endValidation(bool valid)
{
    setValidating(false);
    setValid(valid);
}


/**
 * @brief The local directory to sync.
 *
 * The directory holds the full path to the local directory to sync to the backend.
 *
 * @sa isNull
 */
QString Synchronizer::directory() const
{
    return m_directory;
}


/**
 * @brief Set the local directory to sync.
 */
void Synchronizer::setDirectory(const QString& directory)
{
    m_directory = directory;
}


/**
 * @brief Indicates if the synchronizer is valid.
 *
 * This property indicates whether or not the synchronizer is valid. A valid
 * synchronizer must have a local directory set and this directory must exist.
 */
bool Synchronizer::isNull() const
{
    return m_directory.isEmpty() || !QDir(m_directory).exists();
}


/**
 * @brief Save the settings of the synchronizer.
 *
 * This saves the settings of the synchronizer to the directory the
 * synchronizer works on.
 */
bool Synchronizer::save() const
{
    auto result = false;
    if (!isNull()) {
        QDir dir(m_directory);
        auto settingsFileName = dir.absoluteFilePath(SaveFileName);
        result = JsonUtils::patchJsonFile(settingsFileName, toMap());
    }
    return result;
}


/**
 * @brief Load settings from the synchronizer's directory.
 */
void Synchronizer::restore()
{
    if (!isNull()) {
        QDir dir(m_directory);
        auto settingsFileName = dir.absoluteFilePath(SaveFileName);
        bool ok;
        auto map = JsonUtils::loadMap(settingsFileName, &ok);
        if (ok) {
            fromMap(map);
        }
    }
}


/**
 * @brief Get the synchronizer for the given @p directory.
 */
Synchronizer* Synchronizer::fromDirectory(const QString& directory,
                                                QObject* parent)
{
    Synchronizer* result = nullptr;
    if (!directory.isEmpty()) {
        static QMap<QString, std::function<Synchronizer* (QObject*)>> Synchronizers = {
            {
                "NextCloudSynchronizer",
                [](QObject* parent) { return new NextCloudSynchronizer(parent); }
            },
            {
                "GenericDAVSynchronizer",
                [](QObject* parent) { return new GenericDAVSynchronizer(parent); }
            }
        };
        QDir dir(directory);
        auto absFilePath = dir.absoluteFilePath(SaveFileName);
        auto map = JsonUtils::loadMap(absFilePath);
        auto type = map.value("type").toString();
        auto constructor = Synchronizers.value(type);
        if (constructor) {
            result = constructor(parent);
            result->fromMap(map);
        } else {
            qCDebug(synchronizer) << "Unknown synchronizer type" << type;
        }
    }
    return result;
}


/**
 * @brief Save the settings of the synchronizer to a variant map.
 *
 * This method saves the settings of the synchronizer to a variant map. This is used for
 * serialization.
 *
 * @sa fromMap()
 */
QVariantMap Synchronizer::toMap() const
{
    QVariantMap result;
    result.insert("type", QString(metaObject()->className()));
    return result;
}


/**
 * @brief Restore the serializer from a variant map.
 *
 * This is the reverse operation to @sa toMap(). It takes a variant map and extracts the settings
 * stored, applying them one by one to the properties of the synchronizer.
 */
void Synchronizer::fromMap(const QVariantMap& map)
{
    Q_UNUSED(map);
}
