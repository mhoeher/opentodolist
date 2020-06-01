/*
 * Copyright 2020 Martin Hoeher <martin@rpdev.net>
 +
 * This file is part of OpenTodoList.
 *
 * OpenTodoList is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * OpenTodoList is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenTodoList.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "synchronizer.h"

#include <functional>

#include <QDir>
#include <QFile>
#include <QMap>
#include <QMetaObject>

#include "webdavsynchronizer.h"
#include "utils/jsonutils.h"

static Q_LOGGING_CATEGORY(log, "OpenTodoList.Synchronizer", QtWarningMsg)

        /**
         * @brief The default file name for storing synchronizer settings.
         */
        const QString Synchronizer::SaveFileName = ".opentodolist.synchronizer";
const QString Synchronizer::LogFileName = ".opentodolist.sync.log";
const int Synchronizer::MaxLogEntries;

/**
 * @brief Constructor.
 *
 * This creates a new synchronizer. Synchronizers are initially invalid (indicated by the
 * @sa isNull property). The @sa setDirectory method must be used to point the synchronizer
 * to an existing local directory.
 */
Synchronizer::Synchronizer(QObject *parent)
    : QObject(parent),
      m_uuid(QUuid::createUuid()),
      m_accountUid(),
      m_validating(false),
      m_valid(false),
      m_synchronizing(false),
      m_creatingDirectory(false),
      m_findingLibraries(false),
      m_directory(),
      m_existingLibraries(),
      m_lastSync(),
      m_log()
{
}

/**
 * @brief Destructor.
 */
Synchronizer::~Synchronizer() {}

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
 * @brief Set the list of existing libraries.
 */
void Synchronizer::setExistingLibraries(const QVariantList &existingLibraries)
{
    if (existingLibraries != m_existingLibraries) {
        m_existingLibraries = existingLibraries;
        emit existingLibrariesChanged();
    }
}

/**
 * @brief Set the finding libraries status.
 *
 * This method shall be called in sub-classes to indicate that searching for
 * libraries started or finished.
 */
void Synchronizer::setFindingLibraries(bool findingLibraries)
{
    if (m_findingLibraries != findingLibraries) {
        m_findingLibraries = findingLibraries;
        emit findingLibrariesChanged();
    }
}

QUuid Synchronizer::accountUid() const
{
    return m_accountUid;
}

void Synchronizer::setAccountUid(const QUuid &accountUid)
{
    m_accountUid = accountUid;
}

/**
 * @brief Set the Account object to which the synchronizer belongs to.
 *
 * This method is called to pass in the Account to which this synchronizer belongs to.
 * The synchronizer can use it to pull out additional settings required to work.
 *
 * The default implementation does nothing. Concrete sub-classes should override this method
 * to extract whatever information they need.
 */
void Synchronizer::setAccount(Account *account)
{
    Q_UNUSED(account);
}

bool Synchronizer::loadLog()
{
    bool result = false;
    m_log.clear();
    if (!m_directory.isEmpty()) {
        QFile logFile(m_directory + "/" + LogFileName);
        if (logFile.open(QIODevice::ReadOnly)) {
            auto json = QJsonDocument::fromJson(logFile.readAll());
            if (json.isArray()) {
                auto array = json.toVariant().toList();
                for (auto obj : array) {
                    LogEntry entry;
                    auto map = obj.toMap();
                    entry.time = map.value("time").toDateTime();
                    entry.message = map.value("message").toString();
                    entry.type = map.value("type").value<LogType>();
                    m_log.append(entry);
                }
            }
            logFile.close();
            result = true;
        } else {
            qCWarning(::log) << "Failed to open log for reading:" << logFile.errorString();
        }
    }
    return result;
}

bool Synchronizer::saveLog()
{
    bool result = true;
    if (!m_directory.isEmpty() && !m_log.isEmpty()) {
        QFile log(m_directory + "/" + LogFileName);
        if (log.open(QIODevice::WriteOnly)) {
            QVariantList list;
            for (auto entry : m_log) {
                QVariantMap map;
                map["type"] = QVariant::fromValue(entry.type);
                map["time"] = entry.time;
                map["message"] = entry.message;
                list.append(map);
            }
            auto json = QJsonDocument::fromVariant(list).toJson(QJsonDocument::Indented);
            log.write(json);
            log.close();
            result = true;
        } else {
            qCWarning(::log) << "Failed to open log file for writing:" << log.errorString();
        }
    }
    return result;
}

/**
 * @brief Returns the log of the synchronizer.
 */
QList<Synchronizer::LogEntry> Synchronizer::log() const
{
    return m_log;
}

/**
 * @brief Get a debug stream to write a debug log entry.
 *
 * This returns a QDebug object which can be used to write an entry
 * to the log.
 */
QDebug Synchronizer::debug()
{
    return createDebugStream<Debug>();
}

/**
 * @brief Get a debug stream to write a warning log entry.
 *
 * This returns a QDebug object which can be used to write an entry
 * to the log.
 */
QDebug Synchronizer::warning()
{
    return createDebugStream<Warning>();
}

/**
 * @brief Get a debug stream to write an error log entry.
 *
 * This returns a QDebug object which can be used to write an entry
 * to the log.
 */
QDebug Synchronizer::error()
{
    return createDebugStream<Error>();
}

void Synchronizer::setLastSync(const QDateTime &lastSync)
{
    m_lastSync = lastSync;
}

/**
 * @brief The date and time when the last sync has been run.
 */
QDateTime Synchronizer::lastSync() const
{
    return m_lastSync;
}

/**
 * @brief The UID of the synchronizer.
 */
QUuid Synchronizer::uid() const
{
    return m_uuid;
}

bool Synchronizer::findingLibraries() const
{
    return m_findingLibraries;
}

QVariantList Synchronizer::existingLibraries() const
{
    return m_existingLibraries;
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
void Synchronizer::setDirectory(const QString &directory)
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
 * @brief Get the type name (aka the class name) of the synchronizer.
 */
QString Synchronizer::type() const
{
    return metaObject()->className();
}

/**
 * @brief Get the synchronizer for the given @p directory.
 */
Synchronizer *Synchronizer::fromDirectory(const QString &directory, QObject *parent)
{
    Synchronizer *result = nullptr;
    if (!directory.isEmpty()) {
        static QMap<QString, std::function<Synchronizer *(QObject *)>> Synchronizers = {
            { "WebDAVSynchronizer", [](QObject *parent) { return new WebDAVSynchronizer(parent); } }
        };
        QDir dir(directory);
        auto absFilePath = dir.absoluteFilePath(SaveFileName);
        auto map = JsonUtils::loadMap(absFilePath);
        auto type = map.value("type").toString();
        auto constructor = Synchronizers.value(type);
        if (!type.isEmpty()) {
            if (constructor) {
                result = constructor(parent);
                result->fromMap(map);
                result->setDirectory(directory);
            } else {
                qCWarning(::log) << "Unknown synchronizer type" << type;
            }
        }
    }
    return result;
}

/**
 * @brief Start searching for existing libraries.
 *
 * This method triggers a search for existing libraries in the currently
 * set remote directory of the synchronizer. When the search is finished,
 * the existingLibraries property of the synchronizer is updated.
 *
 * The default implementation of this method does nothing. Sub-classes
 * can implement it if the appropriate functionality is supported by
 * the respective backends.
 */
void Synchronizer::findExistingLibraries() {}

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
    result.insert("uid", m_uuid);
    result.insert("lastSync", m_lastSync);
    if (!m_accountUid.isNull()) {
        result.insert("account", m_accountUid);
    }
    return result;
}

/**
 * @brief Restore the serializer from a variant map.
 *
 * This is the reverse operation to @sa toMap(). It takes a variant map and extracts the settings
 * stored, applying them one by one to the properties of the synchronizer.
 */
void Synchronizer::fromMap(const QVariantMap &map)
{
    m_uuid = map.value("uid", m_uuid).toUuid();
    m_lastSync = map.value("lastSync", m_lastSync).toDateTime();
    m_accountUid = map.value("account", QUuid()).toUuid();
}

/**
 * @brief Constructor.
 */
SynchronizerExistingLibrary::SynchronizerExistingLibrary() : m_name(), m_path() {}

/**
 * @brief The name if the library.
 */
QString SynchronizerExistingLibrary::name() const
{
    return m_name;
}

/**
 * @brief Set the library name.
 */
void SynchronizerExistingLibrary::setName(const QString &name)
{
    m_name = name;
}

/**
 * @brief The path (relative to the Synchronizer's remote directory).
 */
QString SynchronizerExistingLibrary::path() const
{
    return m_path;
}

/**
 * @brief Set the path of the library.
 */
void SynchronizerExistingLibrary::setPath(const QString &path)
{
    m_path = path;
}

QUuid SynchronizerExistingLibrary::uid() const
{
    return m_uid;
}

void SynchronizerExistingLibrary::setUid(const QUuid &uid)
{
    m_uid = uid;
}

template<Synchronizer::LogType Type>
QDebug Synchronizer::createDebugStream()
{
    while (m_log.length() >= MaxLogEntries) {
        m_log.removeFirst();
    }
    LogEntry entry;
    entry.time = QDateTime::currentDateTime();
    entry.type = Type;
    m_log.append(entry);
    return QDebug(&m_log.last().message);
}
