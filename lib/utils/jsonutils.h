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

#ifndef UTILS_JSONUTILS_H_
#define UTILS_JSONUTILS_H_

#include <QString>
#include <QVariantMap>
#include <QLoggingCategory>

/**
 * @brief Various utils to work with JSON.
 *
 * This namespace holds utility functions to work with JSON data.
 */
namespace JsonUtils {

bool patchJsonFile(const QString& filename, const QVariantMap& data, bool* changed = nullptr);
QVariantMap loadMap(const QString& filename, bool* ok = nullptr);

}

#endif // UTILS_JSONUTILS_H_
