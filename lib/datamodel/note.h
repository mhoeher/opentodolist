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

#ifndef DATAMODEL_NOTE_H_
#define DATAMODEL_NOTE_H_

#include <QObject>

#include "toplevelitem.h"


/**
 * @brief A class representing a single note.
 *
 * The Note class represents a single note inside a library. A note is like a
 * sticky note, whith a short headline and (optionally) some additional text providing
 * further details.
 */
class Note : public TopLevelItem
{
    Q_OBJECT
public:
    explicit Note(QObject *parent = nullptr);
    explicit Note(const QString &filename, QObject *parent = nullptr);
    explicit Note(const QDir &dir, QObject *parent = nullptr);
    virtual ~Note();
};

typedef QSharedPointer<Note> NotePtr;

#endif // DATAMODEL_NOTE_H_
