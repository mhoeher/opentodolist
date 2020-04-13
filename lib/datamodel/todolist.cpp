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

#include "todolist.h"

#include <QQmlEngine>

#include "library.h"
#include "todo.h"
#include "fileutils.h"

/**
 * @brief Constructor.
 */
TodoList::TodoList(QObject *parent) : TodoList(QString(), parent) {}

/**
 * @brief Constructor.
 */
TodoList::TodoList(const QString &filename, QObject *parent) : TopLevelItem(filename, parent) {}

/**
 * @brief Constructor.
 */
TodoList::TodoList(const QDir &dir, QObject *parent) : TopLevelItem(dir, parent) {}

/**
 * @brief Destructor.
 */
TodoList::~TodoList() {}
