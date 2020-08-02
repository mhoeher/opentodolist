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

#include "syntaxhighlighting.h"

#include <QBuffer>
#include <QDebug>
#include <QFile>
#include <QTemporaryDir>

#include <algorithm>

#include "../3rdparty/KDE/syntax-highlighting/src/lib/definition.h"
#include "../3rdparty/KDE/syntax-highlighting/src/lib/htmlhighlighter.h"
#include "../3rdparty/KDE/syntax-highlighting/src/lib/repository.h"
#include "../3rdparty/KDE/syntax-highlighting/src/lib/syntaxhighlighter.h"
#include "../3rdparty/KDE/syntax-highlighting/src/lib/theme.h"

/**
 * @brief Constructor.
 */
SyntaxHighlighter::SyntaxHighlighter(QObject* parent)
    : QObject(parent),
      m_highlighter(nullptr),
      m_repository(nullptr),
      m_document(nullptr),
      m_theme(Light)
{
    // Initialize KDE syntax highlighting resources in case
    // we use static linking
    Q_INIT_RESOURCE(syntax_data);
    Q_INIT_RESOURCE(theme_data);
    m_highlighter = new KSyntaxHighlighting::SyntaxHighlighter(this);
    m_repository = new KSyntaxHighlighting::Repository();
    m_highlighter->setDefinition(m_repository->definitionForFileName("test.md"));
    applyTheme();
}

SyntaxHighlighter::~SyntaxHighlighter()
{
    delete m_repository;
}

/**
 * @brief The text document the highlighter works on.
 */
QQuickTextDocument* SyntaxHighlighter::document() const
{
    return m_document;
}

/**
 * @brief Set the document to highlight.
 */
void SyntaxHighlighter::setDocument(QQuickTextDocument* document)
{
    if (m_document != document) {
        m_document = document;
        emit documentChanged();
        if (m_document != nullptr) {
            m_highlighter->setDocument(m_document->textDocument());
        } else {
            m_highlighter->setDocument(nullptr);
        }
    }
}

SyntaxHighlighter::Theme SyntaxHighlighter::theme() const
{
    return m_theme;
}

void SyntaxHighlighter::setTheme(const Theme& theme)
{
    if (m_theme != theme) {
        m_theme = theme;
        emit themeChanged();
        applyTheme();
    }
}

/**
 * @brief Convert source code to HTML.
 *
 * This utility method takes some @p source code (which is written in the given @p language)
 * and converts it to "highlighted" text in HTML format.
 */
QString SyntaxHighlighter::sourceToHtml(const QString& source, const QString& language) const
{
    QTemporaryDir tmpDir;
    auto outputFileName = tmpDir.path() + "/" + "output.html";
    KSyntaxHighlighting::HtmlHighlighter highlighter;
    highlighter.setOutputFile(outputFileName);
    switch (m_theme) {
    case Dark:
        highlighter.setTheme(
                m_repository->defaultTheme(KSyntaxHighlighting::Repository::DarkTheme));
        break;
    default:
        highlighter.setTheme(
                m_repository->defaultTheme(KSyntaxHighlighting::Repository::LightTheme));
        break;
    }
    auto languageLower = language.toLower();
    auto definitions = m_repository->definitions();
    auto def = std::find_if(definitions.begin(), definitions.end(),
                            [&](auto d) { return d.name().toLower() == languageLower; });
    if (def != definitions.end()) {
        highlighter.setDefinition(*def);
    } else {
        highlighter.setDefinition(m_repository->definitionForFileName("test." + language));
    }
    {
        auto unescapedSource = source;
        unescapedSource.replace("&amp;", "&");
        unescapedSource.replace("&lt;", "<");
        unescapedSource.replace("&gt;", ">");
        unescapedSource.replace("&quot;", "\"");
        auto sourceData = unescapedSource.toUtf8();
        QBuffer buffer(&sourceData);
        if (buffer.open(QIODevice::ReadOnly)) {
            highlighter.highlightData(&buffer);
        }
    }
    QFile file(outputFileName);
    if (file.open(QIODevice::ReadOnly)) {
        auto result = file.readAll();
        file.close();
        const QByteArray BodyStart = "<body ";
        auto bodyStartIdx = result.indexOf(BodyStart);
        auto bodyEndIdx = result.indexOf("</body>");
        if (bodyStartIdx >= 0 && bodyEndIdx >= 0) {
            bodyStartIdx = bodyStartIdx + BodyStart.length();
            result = "<code " + result.mid(bodyStartIdx, bodyEndIdx - bodyStartIdx) + "</code>";
        }
        return result;
    }
    return "<code><pre>" + source + "</code></pre>";
}

void SyntaxHighlighter::applyTheme()
{
    switch (m_theme) {
    case Dark:
        m_highlighter->setTheme(
                m_repository->defaultTheme(KSyntaxHighlighting::Repository::DarkTheme));
        break;
    default:
        m_highlighter->setTheme(
                m_repository->defaultTheme(KSyntaxHighlighting::Repository::LightTheme));
        break;
    }
}
