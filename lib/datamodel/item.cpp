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

#include "item.h"

#include <QCborMap>
#include <QCborValue>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QJsonDocument>
#include <QVariant>
#include <QVariantMap>
#include <QtGlobal>

#include "datamodel/image.h"
#include "datamodel/note.h"
#include "datamodel/notepage.h"
#include "datamodel/todolist.h"
#include "datamodel/todo.h"
#include "datamodel/task.h"

#include "fileutils.h"
#include "utils/jsonutils.h"
#include "datastorage/cache.h"
#include "datastorage/getitemquery.h"
#include "datastorage/insertorupdateitemsquery.h"

const QString Item::FileNameSuffix = "otl";

class ItemChangedInhibitor
{
public:
    explicit ItemChangedInhibitor(Item* item)
    {
        q_check_ptr(item);
        m_item = item;
        m_loading = item->m_loading;
        m_item->m_loading = true;
    }

    ~ItemChangedInhibitor() { m_item->m_loading = m_loading; }

private:
    Item* m_item;
    bool m_loading;
};

/**
 * @brief Constructor.
 */
ItemCacheEntry::ItemCacheEntry()
    : id(), parentId(), data(), metaData(), calculatedData(), valid(false)
{
}

/**
 * @brief Convert the entry to a byte array.
 */
QByteArray ItemCacheEntry::toByteArray() const
{
    auto map = toMap();

    QCborValue cbor(QCborMap::fromVariantMap(map));
    return cbor.toCbor();
}

QByteArray ItemCacheEntry::toJson() const
{
    auto map = toMap();
    return QJsonDocument::fromVariant(map).toJson();
}

ItemCacheEntry ItemCacheEntry::fromByteArray(const QByteArray& data, const QByteArray& id)
{
    ItemCacheEntry result;
    // Make a copy of the data - this ensures the data is properly aligned:
    QByteArray alignedData(data.constData(), data.length());
    QCborParserError error;
    auto cbor = QCborValue::fromCbor(alignedData, &error);
    if (error.error == QCborError::NoError) {
        auto map = cbor.toMap().toVariantMap();
        if (map["type"] == Item::staticMetaObject.className()) {
            result.valid = true;
            result.id = QUuid(id);
            result.data = map["data"];
            result.metaData = map["meta"];
            result.parentId = map["parent"].toUuid();
        }
    }
    return result;
}

/**
 * @brief Serialize the cache entry.
 *
 * This serializes the data such that it can be stored as one data blob (e.g. for snapshoting the
 * data).
 */
QByteArray ItemCacheEntry::serialize() const
{
    QVariantMap map;
    map["id"] = id.toByteArray();
    map["data"] = toByteArray();
    return QCborValue(QCborMap::fromVariantMap(map)).toCbor();
}

/**
 * @brief Restore an entry previously created via the serialize() method.
 */
ItemCacheEntry ItemCacheEntry::deserialize(const QByteArray& data)
{
    ItemCacheEntry result;
    QCborParserError error;
    auto cbor = QCborValue::fromCbor(data, &error);
    if (error.error == QCborError::NoError) {
        auto map = cbor.toMap().toVariantMap();
        result = fromByteArray(map["data"].toByteArray(), map["id"].toByteArray());
    }
    return result;
}

QVariantMap ItemCacheEntry::toMap() const
{
    QVariantMap map;
    map["data"] = data;
    map["meta"] = metaData;
    map["type"] = Item::staticMetaObject.className();
    map["parent"] = parentId;
    return map;
}

/**
 * @brief Creates an invalid item.
 *
 * This constructs an invalid item, i.e. an item with no file associated on disk with it.
 *
 * @sa isValid()
 */
Item::Item(QObject* parent) : Item(QString(), parent) {}

/**
 * @brief Create an item.
 *
 * This constructor creates an item which stores its data in the @p filename.
 */
Item::Item(const QString& filename, QObject* parent)
    : QObject(parent),
      m_cache(),
      m_filename(filename),
      m_title(),
      m_createdAt(QDateTime::currentDateTimeUtc()),
      m_updatedAt(m_createdAt),
      m_childrenUpdatedAt(),
      m_uid(QUuid::createUuid()),
      m_weight(0.0),
      m_loading(false)
{
    setupChangedSignal();
}

/**
 * @brief Create a new item in the @p dir.
 */
Item::Item(const QDir& dir, QObject* parent) : Item(parent)
{
    if (dir.exists()) {
        m_filename = dir.absoluteFilePath(m_uid.toString() + "." + FileNameSuffix);
    }
}

/**
 * @brief Destructor.
 */
Item::~Item() {}

QUuid Item::parentId() const
{
    return QUuid();
}

/**
 * @brief Delete the item.
 *
 * This method can be called to permanently delete the item from the library.
 * This will remove the data file on disk representing the item (if such a file exists)
 * and emit the itemDeleted() signal afterwards.
 *
 * @note This is a virtual method. Sub-classes of the Item class might override
 *       it to add custom delete handling.
 */
bool Item::deleteItem()
{
    bool result = false;
    if (isValid()) {
        QFile file(m_filename);
        if (file.exists()) {
            result = file.remove();
        }
    }
    emit itemDeleted(this);
    return result;
}

/**
 * @brief Load the item from disk.
 *
 * This method reads back the item data from disk.
 */
bool Item::load()
{
    bool ok = false;
    auto map = JsonUtils::loadMap(m_filename, &ok);
    if (ok) {
        ItemChangedInhibitor inhibitor(this);
        fromMap(map);
    }
    return ok;
}

/**
 * @brief Save the item data back to disk.
 *
 * Use this to trigger a save operation of the item back to disk. This should in particular
 * be called in property setters of sub-classes of the item class.
 *
 * @note This method will have no effect during a load() or any other operation during
 * which the item is restored from its persisted state.
 */
bool Item::save()
{
    bool result = false;
    if (!m_loading) {
        if (isValid()) {
            bool changedValue;
            result = JsonUtils::patchJsonFile(m_filename, toMap(), &changedValue);
            if (changedValue) {
                emit saved();
            }
        }
    }
    return result;
}

/**
 * @brief Save the item to a QVariant for persistence.
 *
 * This saves the Item to a QVariant representation. This representation can be used to
 * create an instance of the item in one thread and transfer it to another.
 */
QVariant Item::toVariant() const
{
    QVariantMap result;
    result["filename"] = m_filename;
    result["data"] = toMap();
    return result;
}

/**
 * @brief Restore the item from a QVariant.
 *
 * Restores the item from the @p data created using toVariant().
 */
void Item::fromVariant(QVariant data)
{
    ItemChangedInhibitor inhibitor(this);
    QVariantMap map = data.toMap();
    setFilename(map.value("filename", m_filename).toString());
    fromMap(map.value("data", QVariantMap()).toMap());
}

/**
 * @brief Clone the item.
 *
 * This returns a clone of the item, i.e. a new instance (of the same type) with all the properties
 * cloned from the other item.
 */
Item* Item::clone()
{
    auto result = Item::decache(this->encache());
    result->finishCloning(this);
    result->setCache(this->cache());
    return result;
}

/**
 * @brief Create a copy of the item.
 *
 * This creates a copy of this item in the @p targetDirectory. The item will belong to the library
 * with the @p targetLibraryUid. If the item is not a TopLevelItem, it will be a child of the item
 * identified by the @p targetItemUid.
 *
 * On success, the instance of the created item is returned. On failure, a nullptr is returned.
 */
Item* Item::copyTo(const QDir& targetDirectory, const QUuid& targetLibraryUuid,
                   const QUuid& targetItemUid)
{
    Q_UNUSED(targetLibraryUuid);
    Q_UNUSED(targetItemUid);

    // 1. Create a 1:1 copy.
    auto result = Item::decache(encache());

    // 2. Adjust some key attributes.
    if (result) {
        result->m_uid = QUuid::createUuid();
        result->m_filename = targetDirectory.absoluteFilePath(result->m_uid.toString() + "."
                                                              + Item::FileNameSuffix);
        result->m_createdAt = QDateTime::currentDateTimeUtc();
        result->m_updatedAt = result->m_createdAt;
    }

    return result;
}

/**
 * @brief Saves the properties of the item to a QVariantMap.
 */
QVariantMap Item::toMap() const
{
    QVariantMap result;
    result["itemType"] = itemType();
    result["uid"] = m_uid;
    if (m_createdAt.isValid()) {
        result["createdAt"] = m_createdAt.toString(Qt::ISODate);
    }
    if (m_updatedAt.isValid()) {
        result["updatedAt"] = m_updatedAt.toString(Qt::ISODate);
    }
    result["title"] = m_title;
    result["weight"] = m_weight;
    return result;
}

/**
 * @brief Restores the properties of the item from the @p map.
 */
void Item::fromMap(QVariantMap map)
{
    setUid(map.value("uid", m_uid).toUuid());
    m_createdAt = QDateTime::fromString(map.value("createdAt").toString(), Qt::ISODate);
    m_updatedAt = QDateTime::fromString(map.value("updatedAt").toString(), Qt::ISODate);
    setTitle(map.value("title", m_title).toString());
    setWeight(map.value("weight", m_weight).toDouble());
}

/**
 * @brief Finish cloning the item.
 *
 * This method is called by Item::clone() on the cloned instance. @p source is the item that this
 * one has just been cloned from. This method can be used to copy over calculated properties from
 * the source item.
 */
void Item::finishCloning(Item* source)
{
    m_childrenUpdatedAt = source->m_childrenUpdatedAt;
    m_parents = source->m_parents;
}

/**
 * @brief A list of parents of the item UIDs, starting with the top most (i.e. the library).
 */
const QVector<QUuid>& Item::parents() const
{
    return m_parents;
}

/**
 * @brief Set the item's parent UIDs list.
 */
void Item::setParents(const QVector<QUuid>& newParents)
{
    if (m_parents == newParents)
        return;
    m_parents = newParents;
    emit parentsChanged();
}

/**
 * @brief The date and time when the item was last updated.
 */
QDateTime Item::updatedAt() const
{
    if (m_updatedAt.isValid()) {
        return m_updatedAt;
    } else {
        // For old items which have no updatedAt timestamp yet, assume
        // an old one so they always sort last.
        QDateTime result;
        result.setSecsSinceEpoch(0);
        return result;
    }
}

/**
 * @brief The effective date when the item has been updated at.
 *
 * This returns the date and time when the item has been last updated. In contrast to the
 * updatedAt, this takes into account modifications of children.
 */
QDateTime Item::effectiveUpdatedAt() const
{
    auto result = m_updatedAt;
    if (m_childrenUpdatedAt.isValid() && m_childrenUpdatedAt > result) {
        result = m_childrenUpdatedAt;
    }
    return result;
}

/**
 * @brief The date and time when the item has been created.
 */
QDateTime Item::createdAt() const
{
    if (m_createdAt.isValid()) {
        return m_createdAt;
    } else {
        // For old items which have no createdAt timestamp yet, assume
        // an old one so they always sort last.
        QDateTime result;
        result.setMSecsSinceEpoch(0);
        return result;
    }
}

/**
 * @brief Apply properties calculated from the database.
 *
 * This function is called when restoring an item from the cache.
 * Sub-classes can override it to apply properties which have been
 * calculated automatically when the item is read from the item Cache.
 */
void Item::applyCalculatedProperties(const QVariantMap& properties)
{
    setChildrenUpdatedAt(
            properties.value("childrenUpdatedAt", QVariant::fromValue(m_childrenUpdatedAt))
                    .toDateTime());
    setParents(properties.value("parents", QVariant::fromValue(m_parents)).value<QVector<QUuid>>());
}

/**
 * @brief The Cache the item is connected to.
 */
Cache* Item::cache() const
{
    return m_cache.data();
}

/**
 * @brief Set the cache the item is connected to.
 */
void Item::setCache(Cache* cache)
{
    if (m_cache != cache) {
        if (m_cache != nullptr) {
            disconnect(m_cache.data(), &Cache::dataChanged, this, &Item::onCacheChanged);
            disconnect(this, &Item::changed, this, &Item::onChanged);
        }
        m_cache = cache;
        if (m_cache != nullptr) {
            connect(m_cache.data(), &Cache::dataChanged, this, &Item::onCacheChanged);
            connect(this, &Item::changed, this, &Item::onChanged);
        }
    }
}

/**
 * @brief The weight of the item.
 *
 * This property holds the weight of the item. The weight property is used to
 * manually sort items. This is done by ordering the items in a list by their
 * weight and allowing the user to set the weight.
 */
double Item::weight() const
{
    return m_weight;
}

/**
 * @brief Set the weight property.
 */
void Item::setWeight(double weight)
{
    if (!qFuzzyCompare(m_weight, weight)) {
        m_weight = weight;
        emit weightChanged();
    }
}

/**
 * @brief The directory which contains the item's data.
 *
 * This returns the directory where the item's data files are stored. If the
 * item is invalid, returns an empty string.
 */
QString Item::directory() const
{
    if (isValid()) {
        return QFileInfo(m_filename).path();
    }
    return QString();
}

/**
 * @brief Create an item from a map.
 *
 * @sa toMap()
 */
Item* Item::createItem(QVariantMap map, QObject* parent)
{
    auto result = createItem(map.value("itemType").toString(), parent);
    if (result != nullptr) {
        ItemChangedInhibitor inhibitor(result);
        result->fromMap(map);
    }
    return result;
}

/**
 * @brief Create an item from a variant.
 *
 * @sa toVariant()
 */
Item* Item::createItem(QVariant variant, QObject* parent)
{
    auto result =
            createItem(variant.toMap().value("data").toMap().value("itemType").toString(), parent);
    if (result != nullptr) {
        result->fromVariant(variant);
    }
    return result;
}

/**
 * @brief Create an item from a string type.
 *
 * This creates an item from a string which holds an item type name. If the string
 * does not match one of the known item names, this function returns a null pointer.
 */
Item* Item::createItem(const QString& itemType, QObject* parent)
{
    if (itemType == "Image") {
        return new Image(parent);
    } else if (itemType == "Note") {
        return new Note(parent);
    } else if (itemType == "NotePage") {
        return new NotePage(parent);
    } else if (itemType == "TodoList") {
        return new TodoList(parent);
    } else if (itemType == "Todo") {
        return new Todo(parent);
    } else if (itemType == "Task") {
        return new Task(parent);
    } else {
        return nullptr;
    }
}

Item* Item::createItemFromFile(QString filename, QObject* parent)
{
    Item* result = nullptr;
    bool ok;
    auto map = JsonUtils::loadMap(filename, &ok);
    if (ok) {
        result = createItem(map, parent);
        if (result != nullptr) {
            ItemChangedInhibitor inhibitor(result);
            result->setFilename(filename);
        }
    }
    return result;
}

ItemCacheEntry Item::encache() const
{
    ItemCacheEntry result;
    result.id = m_uid;
    result.parentId = parentId();
    result.data = toMap();
    QVariantMap meta;
    meta["filename"] = FileUtils::toPersistedPath(m_filename);
    result.metaData = meta;
    result.valid = true;
    return result;
}

Item* Item::decache(const ItemCacheEntry& entry, QObject* parent)
{
    Item* result = nullptr;
    if (entry.valid) {
        result = Item::createItem(entry.data.toMap(), parent);
        if (result) {
            ItemChangedInhibitor inhibitor(result);
            result->applyCalculatedProperties(entry.calculatedData.toMap());
            auto meta = entry.metaData.toMap();
            auto path = meta["filename"].toString();
            path = FileUtils::fromPersistedPath(path);
            result->setFilename(path);
            auto topLevelItem = qobject_cast<TopLevelItem*>(result);
            if (topLevelItem != nullptr) {
                topLevelItem->setLibraryId(entry.parentId);
            }
        }
    }
    return result;
}

Item* Item::decache(const QVariant& entry, QObject* parent)
{
    auto cacheEntry = entry.value<ItemCacheEntry>();
    return decache(cacheEntry, parent);
}

/**
   @brief Sets the title of the item.
 */
void Item::setTitle(const QString& title)
{
    if (m_title != title) {
        m_title = title;
        emit titleChanged();
    }
}

/**
 * @brief The type of the item.
 *
 * This property holds the type name of the item (e.g. "TodoList" or "Task"). It
 * is written into the item's data file and used when reading information back.
 */
QString Item::itemType() const
{
    return metaObject()->className();
}

void Item::setUid(const QUuid& uid)
{
    // Note: This shall not trigger a save operation! Change of uid shall happen
    // only on de-serialization, hence, no need to trigger a save right now.
    if (m_uid != uid) {
        m_uid = uid;
        emit uidChanged();
    }
}

void Item::setFilename(const QString& filename)
{
    // Note: Same as for setUid(), this shall not trigger a save() operation.
    if (m_filename != filename) {
        m_filename = filename;
        emit filenameChanged();
    }
}

void Item::setupChangedSignal()
{
    // Update the updateAt property for every change:
    connect(this, &Item::changed, this, &Item::setUpdateAt);

    // Connect individual property update signals to the changed signal:
    connect(this, &Item::titleChanged, this, &Item::changed);
    connect(this, &Item::uidChanged, this, &Item::changed);
    connect(this, &Item::filenameChanged, this, &Item::changed);
    connect(this, &Item::weightChanged, this, &Item::changed);
}

void Item::onCacheChanged()
{
    if (m_cache) {
        auto q = new GetItemQuery();
        q->setUid(m_uid);
        connect(q, &GetItemQuery::itemLoaded, this, &Item::onItemDataLoadedFromCache,
                Qt::QueuedConnection);
        m_cache->run(q);
    }
}

void Item::onItemDataLoadedFromCache(const QVariant& entry)
{
    ItemPtr item(Item::decache(entry));
    if (item != nullptr) {
        ItemChangedInhibitor inhibitor(this);
        this->fromMap(item->toMap());
        this->applyCalculatedProperties(entry.value<ItemCacheEntry>().calculatedData.toMap());
    }
}

void Item::onChanged()
{
    if (m_cache != nullptr) {
        auto q = new InsertOrUpdateItemsQuery();
        q->add(this, InsertOrUpdateItemsQuery::Save);
        m_cache->run(q);
    }
}

void Item::setUpdateAt()
{
    if (!m_loading) {
        m_updatedAt = QDateTime::currentDateTimeUtc();
    }
}

void Item::setChildrenUpdatedAt(const QDateTime& childrenUpdatedAt)
{
    if (m_childrenUpdatedAt == childrenUpdatedAt) {
        return;
    }
    m_childrenUpdatedAt = childrenUpdatedAt;
    emit updatedAtChanged();
}

/**
   @brief Write an item to a debug stream.
 */
QDebug operator<<(QDebug debug, const Item* item)
{
    QDebugStateSaver saver(debug);
    Q_UNUSED(saver)

    if (item) {
        debug.nospace() << item->itemType() << "(\"" << item->title() << "\" [" << item->uid()
                        << "])";
    } else {
        debug << "Item(nullptr)";
    }
    return debug;
}
