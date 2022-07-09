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

#ifndef UTILS_COLORS_H_
#define UTILS_COLORS_H_

#include <QObject>
#include <QColor>

class Color
{
    Q_GADGET

    Q_PROPERTY(QColor color READ color CONSTANT)
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(bool valid READ isValid CONSTANT)

public:
    Color();
    Color(const QColor& color, const QString& name);

    bool isValid() const;
    const QColor& color() const;
    const QString& name() const;

private:
    QColor m_color;
    QString m_name;
};

class Colors : public QObject
{
    Q_OBJECT
public:
    explicit Colors(QObject* parent = nullptr);

    Q_INVOKABLE QList<Color> loadRALColors() const;
    Q_INVOKABLE QList<Color> loadQtColors() const;
    Q_INVOKABLE QList<Color> loadColors() const;
};

Q_DECLARE_METATYPE(Color);
Q_DECLARE_METATYPE(QList<Color>);

#endif // UTILS_COLORS_H_
