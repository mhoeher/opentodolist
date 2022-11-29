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
#include <QJsonDocument>
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
Translations::Translations(QQmlEngine* engine, QObject* parent)
    : QObject(parent), m_engine(engine), m_translator(new QTranslator(this)), m_language()
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
void Translations::setLanguage(const QString& language)
{
    if (m_language != language) {
        m_language = language;
        apply();
        save();
        emit languageChanged();
    }
}

/**
 * @brief Returns the content of the file "languages.json".
 */
QVariantList Translations::availableLanguages() const
{
    QVariantList result;
    result << QVariantMap { { "code", "" }, { "name", tr("System Language") } };

    QFile languageFile(":/translations/languages.json");
    if (languageFile.open(QIODevice::ReadOnly)) {
        auto list = QJsonDocument::fromJson(languageFile.readAll()).toVariant().toList();
        result.append(list);
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
        auto language = m_language;
        language.replace("-", "_");
        QString filename = ":/translations/OpenTodoList-" + language + ".qm";
        if (QFile::exists(filename)) {
            if (m_translator->load(filename)) {
                qCDebug(log) << "Loaded translations from file" << filename;
                m_engine->retranslate();
                return;
            } else {
                qCWarning(log) << "Failed loading translation from file" << filename;
            }
        } else {
            qCWarning(log) << "Explicitly requested translation file" << filename << "not found";
        }
    }

    // Try to find a suitable language file, based on the system locale.
    auto systemLocale = QLocale::system();
    const auto uiLanguages = systemLocale.uiLanguages();
    QString langFile;

    // Pass 1: Is there a file matching exactly the user locale?
    for (auto uiLang : uiLanguages) {
        auto path = ":/translations/OpenTodoList-" + uiLang.replace("-", "_") + ".qm";
        if (QFile::exists(path)) {
            langFile = path;
            break;
        }
    }

    // Special pass for Chinese language variants. See
    // https://gitlab.com/rpdev/opentodolist/-/issues/511 for more details. Basically, if localized
    // translations are not available, we must correctly map to the "general" traditional/simplified
    // Chinese ones.
    if (langFile.isEmpty()) {
        for (const auto& uiLang : uiLanguages) {
            QString lang;
            if (uiLang == "zh-CN" || uiLang == "zh-SG") {
                // Map these to simplified chinese:
                lang = "zh_Hans";
            } else if (uiLang == "zh-HK" || uiLang == "zh-TW") {
                // And these to traditional ones:
                lang = "zh_Hant";
            }
            if (!lang.isEmpty()) {
                auto path = ":/translations/OpenTodoList-" + lang + ".qm";
                if (QFile::exists(path)) {
                    langFile = path;
                    break;
                }
            }
        }
    }

    // Pass 2: Is there a file with the language part only:
    if (langFile.isEmpty()) {
        for (auto uiLang : uiLanguages) {
            auto lang = uiLang.split("-")[0];
            auto path = ":/translations/OpenTodoList-" + lang + ".qm";
            if (QFile::exists(path)) {
                langFile = path;
                break;
            }
        }
    }

    // Pass 3: Last resort: Use any translation which is at least related:
    if (langFile.isEmpty()) {
        for (auto uiLang : uiLanguages) {
            auto lang = uiLang.split("-")[0];
            QDir translationsDir(":/translations");
            auto entries =
                    translationsDir.entryList({ "OpenTodoList-" + lang + "_*.qm" }, QDir::Files);
            if (!entries.isEmpty()) {
                langFile = ":/translations/" + entries[0];
            }
            break;
        }
    }

    if (!langFile.isEmpty()) {
        if (m_translator->load(langFile)) {
            qCDebug(log) << "Successfully loaded translation"
                         << "for UI languages" << systemLocale.uiLanguages();
            m_engine->retranslate();
        } else {
            qCDebug(log) << "Failed to load translation file" << langFile;
        }
    } else {
        qCDebug(log) << "No translations found for UI languages" << systemLocale.uiLanguages();
    }
}

} // namespace OpenTodoList
