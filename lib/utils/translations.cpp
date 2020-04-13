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

#include "utils/translations.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QLocale>
#include <QLoggingCategory>
#include <QQmlEngine>
#include <QQmlContext>
#include <QSettings>
#include <QTranslator>

namespace OpenTodoList {

static Q_LOGGING_CATEGORY(log, "OpenTodoList.Translator", QtDebugMsg);

/**
 * @class Translations
 * @brief Maintain application translations.
 *
 * This class is used to maintain (i.e. load and select) available translations
 * of the app. It can be attached to a QQmlEngine to let it dynamically change
 * the language of the app at runtime. Language settings are persisted, which
 * allows the user to change to language other than the system default.
 */
/**
 * @brief Constructor.
 *
 * Attaches the Translations object to the QML @p engine. It loads the
 * previously selected language settings (if present) and tries to use
 * a suitable translations file.
 */
Translations::Translations(QQmlEngine *engine, QObject *parent)
    : QObject(parent), m_engine(engine), m_translator(new QTranslator(this))
{
    q_check_ptr(m_engine);
    QCoreApplication::installTranslator(m_translator);
    load();
    apply();
    m_engine->rootContext()->setContextProperty("translations", this);
}

/**
 * @brief The currently selected language for translations.
 */
QString Translations::language() const
{
    return m_language;
}

/**
 * @brief Set the @p language to show the application in.
 */
void Translations::setLanguage(const QString &language)
{
    if (m_language != language) {
        m_language = language;
        apply();
        save();
        emit languageChanged();
    }
}

/**
 * @brief Get a list of all available languages.
 *
 * This function returns a list of all available languages that can be set as
 * current language.
 */
QStringList Translations::allLanguages() const
{
    QStringList result;
    result << "";
    for (const auto &entry :
         QDir(":/translations").entryList({ "opentodolist_*.qm" }, QDir::Files)) {
        auto key = entry.mid(13, entry.length() - 16);
        if (!result.contains(key)) {
            result << key;
        }
    }
    return result;
}

void Translations::load()
{
    QSettings settings;
    qCWarning(log) << settings.fileName();
    settings.beginGroup("Language");
    m_language = settings.value("language", m_language).toString();
    settings.endGroup();
}

void Translations::save()
{
    QSettings settings;
    settings.beginGroup("Language");
    settings.setValue("language", m_language);
    settings.endGroup();
}

void Translations::apply()
{
    if (!m_language.isEmpty()) {
        QString filename = ":/translations/opentodolist_" + m_language + ".qm";
        if (QFile::exists(filename)) {
            m_translator->load(filename);
            qCDebug(log) << "Loaded translations from file" << filename;
            m_engine->retranslate();
            return;
        } else {
            qCWarning(log) << "Explicitly requested translation file" << filename << "not found";
        }
    }

    auto systemLocale = QLocale::system();
    if (m_translator->load(systemLocale, "opentodolist", "_", ":/translations/")) {
        qCDebug(log) << "Successfully loaded translation"
                     << "for UI languages" << systemLocale.uiLanguages();
        m_engine->retranslate();
    } else {
        qCDebug(log) << "No translations found for UI languages" << systemLocale.uiLanguages();
    }
}

} // namespace OpenTodoList
