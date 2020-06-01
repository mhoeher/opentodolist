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

#ifndef UTILS_TRANSLATIONS_H_
#define UTILS_TRANSLATIONS_H_

#include <QObject>
#include <QString>
#include <QStringList>

class QTranslator;
class QQmlEngine;

namespace OpenTodoList {

class Translations : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
public:
    explicit Translations(QQmlEngine* engine, QObject* parent = nullptr);

    QString language() const;
    void setLanguage(const QString& language);

    Q_INVOKABLE QStringList allLanguages() const;

signals:

    void languageChanged();

public slots:

private:
    QQmlEngine* m_engine;
    QTranslator* m_translator;
    QString m_language;

    void load();
    void save();
    void apply();
};

} // namespace OpenTodoList

#endif // UTILS_TRANSLATIONS_H_
