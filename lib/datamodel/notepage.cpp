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

#include "notepage.h"

NotePage::NotePage(QObject *parent) : ComplexItem(QString(), parent) {}

NotePage::NotePage(const QString &filename, QObject *parent)
    : ComplexItem(filename, parent), m_noteUid()
{
    connect(this, &NotePage::noteUidChanged, this, &NotePage::changed);
}

NotePage::NotePage(const QDir &dir, QObject *parent) : ComplexItem(dir, parent), m_noteUid()
{
    connect(this, &NotePage::noteUidChanged, this, &NotePage::changed);
}

NotePage::~NotePage() {}

QUuid NotePage::noteUid() const
{
    return m_noteUid;
}

void NotePage::setNoteUid(const QUuid &noteUid)
{
    if (m_noteUid != noteUid) {
        m_noteUid = noteUid;
        emit noteUidChanged();
    }
}

QUuid NotePage::parentId() const
{
    return m_noteUid;
}

QVariantMap NotePage::toMap() const
{
    auto result = ComplexItem::toMap();
    result["noteUid"] = m_noteUid;
    return result;
}

void NotePage::fromMap(QVariantMap map)
{
    ComplexItem::fromMap(map);
    setNoteUid(map.value("noteUid", m_noteUid).toUuid());
}
