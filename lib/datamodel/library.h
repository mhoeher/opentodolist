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

#ifndef DATAMODEL_LIBRARY_H_
#define DATAMODEL_LIBRARY_H_

#include <QDir>
#include <QLoggingCategory>
#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QColor>

#include <functional>

#include "image.h"
#include "note.h"
#include "todolist.h"

class Application;
class Synchronizer;
class Cache;

/**
 * @brief Represents a library stored in the cache.
 */
struct LibraryCacheEntry
{
    LibraryCacheEntry();
    LibraryCacheEntry(const LibraryCacheEntry& other) = default;
    LibraryCacheEntry& operator=(const LibraryCacheEntry& other) = default;
    bool operator==(const LibraryCacheEntry& other) const;

    QByteArray toByteArray() const;
    static LibraryCacheEntry fromByteArray(const QByteArray& data, const QByteArray& id);

    QUuid id;
    QVariant data;
    QVariant metaData;
    QVariant calculatedData;
    bool valid;
};

Q_DECLARE_METATYPE(LibraryCacheEntry)

/**
 * @brief A container for items.
 *
 * The Library class is a container for TopLevelItem objects. Each library has
 * a directory, for which it loads and makes the contained items available.
 */
class Library : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUuid uid READ uid WRITE setUid NOTIFY uidChanged)
    Q_PROPERTY(bool isValid READ isValid CONSTANT)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QStringList tags READ tags NOTIFY tagsChanged)
    Q_PROPERTY(QString directory READ directory CONSTANT)
    Q_PROPERTY(bool hasSynchronizer READ hasSynchronizer CONSTANT)
    Q_PROPERTY(bool isInDefaultLocation READ isInDefaultLocation CONSTANT)

    friend class Application;

public:
    static const QString LibraryFileName;

    explicit Library(QObject* parent = nullptr);
    explicit Library(const QString& directory, QObject* parent = nullptr);
    virtual ~Library();

    bool isValid() const;

    LibraryCacheEntry encache() const;
    static Library* decache(const LibraryCacheEntry& entry, QObject* parent = nullptr);
    static Library* decache(const QVariant& entry, QObject* parent = nullptr);
    Library* clone();

    /**
     * @brief The name of the libary as used in the user interface.
     */
    QString name() const { return m_name; }
    void setName(const QString& name);

    /**
     * @brief Returns the directory which the library encapsulates.
     */
    QString directory() const { return m_directory; }

    void deleteLibrary();
    Q_INVOKABLE bool load();
    Q_INVOKABLE bool save();
    Q_INVOKABLE QVariant syncLog();

    QDir newItemLocation() const;

    static QStringList years(const QString& directory);
    static QStringList months(const QString& directory, const QString& year);
    static QStringList itemFiles(const QString& directory, const QString& year,
                                 const QString& month);

    bool isInDefaultLocation() const;

    QUuid uid() const;
    QStringList tags() const;
    void setTags(const QStringList& tags);

    void fromJson(const QByteArray& data);

    bool hasSynchronizer() const;

    Q_INVOKABLE Synchronizer* createSynchronizer(QObject* parent = nullptr) const;

    Cache* cache() const;
    void setCache(Cache* cache);

    QVariant toVariant();
    void fromVariant(const QVariant& data);
    void applyCalculatedData(const QVariantMap& properties);

    static QString defaultLibrariesLocation();

    const QColor& color() const;
    void setColor(const QColor& newColor);

    Q_INVOKABLE void resetColor();

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
     * @brief The color used to represent the library has changed.
     */
    void colorChanged();

    /**
     * @brief The list of tags changed.
     */
    void tagsChanged();

    /**
     * @brief A library is about to be deleted.
     *
     * This signal is emitted to indicate that the @p library is aboit to be
     * deleted. Clients might connect to this signal to do any additional clean
     * up required before the library data is removed.
     */
    void deletingLibrary(Library* library);

    /**
     * @brief The library is deleted.
     */
    void libraryDeleted(Library* library);

    /**
     * @brief Loading the items of the library finished.
     *
     * This signal is emitted to indicate that loading the items in a library
     * has finished.
     */
    void loadingFinished();

    /**
     * @brief The library needs to be synced.
     *
     * This signal is emitted to indicate that the library needs to be
     * synchronized.
     */
    void needSync();

    /**
     * @brief Some property of the library has changed.
     */
    void changed();

    /**
     * @brief The cache the library is associated with has changed.
     */
    void cacheChanged();

private:
    QUuid m_uid;
    QString m_name;
    QColor m_color;
    QColor m_defaultColor;
    QString m_directory;
    QPointer<Cache> m_cache;

    // Calculated properties
    QStringList m_tags;

    bool m_itemDataChanged;

    bool m_secretsMissing;

    QVariantMap toMap() const;
    void fromMap(QVariantMap map);

    void setUid(const QUuid& uid);
    void onCacheChanged();
    void onLibraryDataLoadedFromCache(const QVariant& entry);
    void onChanged();

    void calculateDefaultColor();
};

#endif // DATAMODEL_LIBRARY_H_
