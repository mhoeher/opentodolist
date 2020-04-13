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

#include "toplevelitem.h"

#include <QMetaEnum>

/**
 * @brief Constructor.
 */
TopLevelItem::TopLevelItem(const QString &filename, QObject *parent)
    : ComplexItem(filename, parent), m_color(White), m_tags()
{
    connect(this, &TopLevelItem::colorChanged, this, &ComplexItem::changed);
    connect(this, &TopLevelItem::tagsChanged, this, &ComplexItem::changed);
}

/**
 * @brief Constructor.
 */
TopLevelItem::TopLevelItem(QObject *parent) : TopLevelItem(QString(), parent) {}

/**
 * @brief Constructor.
 */
TopLevelItem::TopLevelItem(const QDir &dir, QObject *parent)
    : ComplexItem(dir, parent), m_color(White), m_tags()
{
    connect(this, &TopLevelItem::colorChanged, this, &ComplexItem::changed);
    connect(this, &TopLevelItem::tagsChanged, this, &ComplexItem::changed);
}

/**
 * @brief Destructor.
 */
TopLevelItem::~TopLevelItem() {}

QUuid TopLevelItem::parentId() const
{
    return m_libraryId;
}

/**
 * @brief The color of the item.
 */
TopLevelItem::Color TopLevelItem::color() const
{
    return m_color;
}

/**
 * @brief Set the item color.
 */
void TopLevelItem::setColor(const Color &color)
{
    if (m_color != color) {
        m_color = color;
        emit colorChanged();
    }
}

void TopLevelItem::setColor(const QString &color)
{
    QMetaEnum e = QMetaEnum::fromType<Color>();
    bool ok;
    Color c = static_cast<Color>(e.keysToValue(qUtf8Printable(color), &ok));
    if (ok) {
        setColor(c);
    }
}

/**
 * @brief A list of tags attached to the item.
 *
 * This is a list of tags that have been attached to the item. Tags can be used for filtering
 * items. The tags are sorted alphabetically.
 */
QStringList TopLevelItem::tags() const
{
    return m_tags;
}

void TopLevelItem::setTags(const QStringList &tags)
{
    if (m_tags != tags) {
        m_tags = tags;
        emit tagsChanged();
    }
}

/**
 * @brief Adds a new @p tag to the item.
 *
 * This adds a new tag to the item. If the tag already has been attached to the item before,
 * this method has no effect.
 */
void TopLevelItem::addTag(const QString &tag)
{
    if (!m_tags.contains(tag)) {
        m_tags.append(tag);
        m_tags.sort();
        emit tagsChanged();
    }
}

/**
 * @brief Removes a tag from the item.
 *
 * This removes the tag at the given @p index from the item. The index must be
 * valid (i.e. 0 <= index < item.tags().length()). If not, this method will assert.
 */
void TopLevelItem::removeTagAt(int index)
{
    Q_ASSERT(index >= 0 && index < m_tags.length());
    m_tags.removeAt(index);
    emit tagsChanged();
}

/**
 * @brief Removes the @p tag from the item (if it is assigned).
 */
void TopLevelItem::removeTag(const QString &tag)
{
    auto index = m_tags.indexOf(tag);
    if (index >= 0) {
        removeTagAt(index);
    }
}

/**
 * @brief Returns true if the item has been tagged with the given @p tag.
 */
bool TopLevelItem::hasTag(const QString &tag) const
{
    return m_tags.contains(tag);
}

/**
 * @brief The ID of the library the item belongs to.
 */
QUuid TopLevelItem::libraryId() const
{
    return m_libraryId;
}

/**
 * @brief Set the library ID.
 */
void TopLevelItem::setLibraryId(const QUuid &libraryId)
{
    if (m_libraryId != libraryId) {
        m_libraryId = libraryId;
        emit libraryIdChanged();
    }
}

QVariantMap TopLevelItem::toMap() const
{
    auto result = ComplexItem::toMap();

    QMetaEnum e = QMetaEnum::fromType<Color>();
    result["color"] = QString(e.valueToKey(m_color));

    result["tags"] = m_tags;

    return result;
}

void TopLevelItem::fromMap(QVariantMap map)
{
    ComplexItem::fromMap(map);

    setColor(map.value("color", m_color).toString());
    setTags(map.value("tags", m_tags).toStringList());
}
