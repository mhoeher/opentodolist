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

#ifndef UTILS_FONTUTILS_H_
#define UTILS_FONTUTILS_H_

#include <QObject>
#include <QFont>
#include <QFontDatabase>

class FontUtils : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Categories of system fonts that can be retrieved.
     */
    enum class SystemFont {
        GeneralFont = QFontDatabase::GeneralFont, //!< The general font.
        FixedFont = QFontDatabase::FixedFont, //!< The fixed width font to use.
        TitleFont = QFontDatabase::TitleFont, //!< The font to use for titles.
        SmallestReadableFont = QFontDatabase::SmallestReadableFont //!< The smallest readable font.
    };
    Q_ENUM(SystemFont);

    explicit FontUtils(QObject* parent = nullptr);

    Q_INVOKABLE QFont systemFont(FontUtils::SystemFont systemFont);

signals:
};

#endif // UTILS_FONTUTILS_H_
