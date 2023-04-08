/*
 * Copyright 2023 Martin Hoeher <martin@rpdev.net>
 *
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

#include "fontutils.h"

/**
 * @class FontUtils
 * @brief Utilities to work with fonts.
 */

/**
 * @brief Constructor.
 */
FontUtils::FontUtils(QObject* parent) : QObject { parent } {}

/**
 * @brief Look up a system font.
 *
 * This looks up a system default font for the category @p systemFont.
 */
QFont FontUtils::systemFont(SystemFont systemFont)
{
    return QFontDatabase::systemFont(static_cast<QFontDatabase::SystemFont>(systemFont));
}
