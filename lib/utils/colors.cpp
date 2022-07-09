/*
 * Copyright 2022 Martin Hoeher <martin@rpdev.net>
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

#include "colors.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QLoggingCategory>
#include <QMutex>
#include <QMutexLocker>
#include <QVariantList>
#include <QVariantMap>

static Q_LOGGING_CATEGORY(log, "OpenTodoList.Colors", QtDebugMsg);

/**
 * @class Color
 * @brief Represents a single, named color.
 */

/**
 * @brief Constructs an invalid volod.
 */
Color::Color() : m_color(), m_name() {}

/**
 * @brief Constructs a color with the given @p color which is named @p name.
 */
Color::Color(const QColor& color, const QString& name) : m_color(color), m_name(name) {}

/**
 * @brief Returns true if the color is valid.
 */
bool Color::isValid() const
{
    return m_color.isValid();
}

/**
 * @brief Returns the associated color.
 */
const QColor& Color::color() const
{
    return m_color;
}

/**
 * @brief Returns the name of the color.
 */
const QString& Color::name() const
{
    return m_name;
}

/**
 * @class Colors
 * @brief Utility class to work with color information.
 */

/**
 * @brief Constructor.
 */
Colors::Colors(QObject* parent) : QObject { parent }
{
    qRegisterMetaType<QList<Color>>();
}

/**
 * @brief Returns a list of RAL colors.
 *
 * This will load and return a list of RAL color values.
 */
QList<Color> Colors::loadRALColors() const
{
    static QMutex lock;
    QMutexLocker l(&lock);

    static QList<Color>* result = nullptr;
    if (result == nullptr) {
        result = new QList<Color>;
        QFile file(":/ral.json");
        if (!file.open(QIODevice::ReadOnly)) {
            qCWarning(log) << "Failed to open" << file.fileName()
                           << "for reading:" << file.errorString();
            return *result;
        }

        QJsonParseError error;
        auto doc = QJsonDocument::fromJson(file.readAll(), &error);
        if (error.error != QJsonParseError::NoError) {
            qCWarning(log) << "Failed to parse" << file.fileName() << ":" << error.errorString();
            return *result;
        }

        // The first level is a map where keys are RAL color IDs and values are structrured.
        const auto map = doc.toVariant().toMap();
        for (const auto& color : map) {
            // The color is a map, which shall have at least the values "color/hex" as well as
            // "names"
            auto colorMap = color.toMap();
            auto colorHexString = colorMap.value("color").toMap().value("hex").toString();
            if (colorHexString.isEmpty()) {
                qCDebug(log) << "Encountered invalid RAL color - color hex code missing:"
                             << colorMap;
                continue;
            }
            auto name = colorMap.value("names").toMap().value("en").toString();
            if (name.isEmpty()) {
                qCDebug(log) << "Encountered invalid RAL color - color name missing:" << colorMap;
                continue;
            }

            *result << Color(QColor(colorHexString), name);
        }
    }
    return *result;
}

/**
 * @brief Returns a list of Qt's built in colors.
 */
QList<Color> Colors::loadQtColors() const
{
    QList<Color> result;
    const auto colorNames = QColor::colorNames();
    for (const auto& name : colorNames) {
        result << Color(QColor(name), name);
    }
    return result;
}

/**
 * @brief Load a list of named colors.
 *
 * If available, this returns a list of RAL colors. Otherwise, a list of Qt's built in colors
 * is returned.
 */
QList<Color> Colors::loadColors() const
{
    auto result = loadRALColors();
    if (result.isEmpty()) {
        result = loadQtColors();
    }
    return result;
}
