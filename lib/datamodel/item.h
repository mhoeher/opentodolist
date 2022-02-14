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

#ifndef DATAMODEL_ITEM_H_
#define DATAMODEL_ITEM_H_

#include <QDir>
#include <QLoggingCategory>
#include <QObject>
#include <QPointer>
#include <QSharedPointer>
#include <QString>
#include <QUuid>
#include <QVariantMap>
#include <QDateTime>

class Cache;
class ItemChangedInhibitor;

/**
 * @brief Represents an arbitrary item stored in the cache.
 */
struct ItemCacheEntry
{
    ItemCacheEntry();
    ItemCacheEntry(const ItemCacheEntry& other) = default;

    QByteArray toByteArray() const;
    QByteArray toJson() const;
    static ItemCacheEntry fromByteArray(const QByteArray& data, const QByteArray& id);
    QByteArray serialize() const;
    static ItemCacheEntry deserialize(const QByteArray& data);

    QUuid id;
    QUuid parentId;
    QVariant data;
    QVariant metaData;
    QVariant calculatedData;
    bool valid;

private:
    QVariantMap toMap() const;
};

Q_DECLARE_METATYPE(ItemCacheEntry)

/**
 * @brief Base class for all items in a library.
 *
 * This class is the base class for any kind of item which can either directly or indirectly
 * be stored in a Library. It provides the common interface any such item must have.
 */
class Item : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QUuid uid READ uid NOTIFY uidChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString itemType READ itemType CONSTANT)
    Q_PROPERTY(QString filename READ filename NOTIFY filenameChanged)
    Q_PROPERTY(double weight READ weight WRITE setWeight NOTIFY weightChanged)
    Q_PROPERTY(bool isValid READ isValid NOTIFY filenameChanged)
    Q_PROPERTY(QDateTime createdAt READ createdAt NOTIFY createdAtChanged)
    Q_PROPERTY(QDateTime updatedAt READ updatedAt NOTIFY updatedAtChanged)
    Q_PROPERTY(QDateTime effectiveUpdatedAt READ effectiveUpdatedAt NOTIFY updatedAtChanged)

    friend class ItemChangedInhibitor;

public:
    static const QString FileNameSuffix;

    explicit Item(QObject* parent = nullptr);
    explicit Item(const QString& filename, QObject* parent = nullptr);
    explicit Item(const QDir& dir, QObject* parent = nullptr);
    virtual ~Item();

    /**
     * @brief Get the UUID of the parent item.
     *
     * This method returns the UUID of the parent to which the item
     * belongs. For top level items, this is the UUID of the library
     * the item belongs to. For other items, this is the UUID of
     * the direct parent item.
     */
    virtual QUuid parentId() const;

    Q_INVOKABLE virtual bool deleteItem();
    Q_INVOKABLE bool load();
    Q_INVOKABLE bool save();
    Q_INVOKABLE QVariant toVariant() const;
    Q_INVOKABLE void fromVariant(QVariant data);

    virtual Item* copyTo(const QDir& targetDirectory, const QUuid& targetLibraryUuid,
                         const QUuid& targetItemUid = QUuid());

    virtual void applyCalculatedProperties(const QVariantMap& properties);

    /**
     * @brief Check if the item is valid.
     *
     * This returns true if the item is valid (i.e. it has a representation on disk). Otherwise,
     * returns false.
     */
    bool isValid() const { return !m_filename.isEmpty(); }

    /**
     * @brief The title of the item as shown in the user interface.
     */
    QString title() const { return m_title; }

    void setTitle(const QString& title);

    /**
     * @brief The file holding the item data.
     *
     * The filename property holds the name of the file containing the item's data
     * on disk.
     */
    QString filename() const { return m_filename; }

    /**
     * @brief The globally unique ID of the item.
     */
    QUuid uid() const { return m_uid; }
    void setUid(const QUuid& uid);

    QString itemType() const;

    double weight() const;
    void setWeight(double weight);

    QString directory() const;

    static Item* createItem(QVariantMap map, QObject* parent = nullptr);
    static Item* createItem(QVariant variant, QObject* parent = nullptr);
    static Item* createItem(const QString& itemType, QObject* parent = nullptr);
    static Item* createItemFromFile(QString filename, QObject* parent = nullptr);

    ItemCacheEntry encache() const;
    static Item* decache(const ItemCacheEntry& entry, QObject* parent = nullptr);
    static Item* decache(const QVariant& entry, QObject* parent = nullptr);

    Cache* cache() const;
    void setCache(Cache* cache);

    QDateTime createdAt() const;

    QDateTime updatedAt() const;
    QDateTime effectiveUpdatedAt() const;

public slots:

signals:

    void titleChanged();
    void uidChanged();
    void isValidChanged();
    void filenameChanged();
    void weightChanged();
    void createdAtChanged();
    void updatedAtChanged();

    /**
     * @brief The item has been deleted.
     *
     * This signal is emitted when Item::deleteItem() is used to delete the item.
     */
    void itemDeleted(Item* item);

    /**
     * @brief Some properties of the item changed.
     */
    void changed();

    /**
     * @brief The item has been saved.
     *
     * This signal is emitted to indicate that the item has been saved to a
     * file.
     */
    void saved();

protected:
    virtual QVariantMap toMap() const;
    virtual void fromMap(QVariantMap map);

private:
    QPointer<Cache> m_cache;
    QString m_filename;
    QString m_title;
    QDateTime m_createdAt;
    QDateTime m_updatedAt;
    QDateTime m_childrenUpdatedAt;
    QUuid m_uid;
    double m_weight;
    bool m_loading;

    void setFilename(const QString& filename);

    void setupChangedSignal();

    void onCacheChanged();
    void onItemDataLoadedFromCache(const QVariant& entry);
    void onChanged();
    void setUpdateAt();
    void setChildrenUpdatedAt(const QDateTime& childrenUpdatedAt);
};

typedef QSharedPointer<Item> ItemPtr;

Q_DECLARE_METATYPE(ItemPtr)

QDebug operator<<(QDebug debug, const Item* item);

#endif // DATAMODEL_ITEM_H_
