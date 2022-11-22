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

#include <KSyntaxHighlighting/KSyntaxHighlighting/definition.h>
#include <KSyntaxHighlighting/KSyntaxHighlighting/repository.h>
#include <KSyntaxHighlighting/KSyntaxHighlighting/syntaxhighlighter.h>
#include <KSyntaxHighlighting/KSyntaxHighlighting/theme.h>

#include <algorithm>


#include "htmlsyntaxhighlighter.h"

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
    HtmlHighlighter highlighter;
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
    QString result;
    {
        auto unescapedSource = source;
        unescapedSource.replace("&amp;", "&");
        unescapedSource.replace("&lt;", "<");
        unescapedSource.replace("&gt;", ">");
        unescapedSource.replace("&quot;", "\"");
        auto sourceData = unescapedSource.toUtf8();
        QBuffer buffer(&sourceData);
        if (buffer.open(QIODevice::ReadOnly)) {
            result = highlighter.highlightData(&buffer);
        }
    }
    if (!result.isEmpty()) {
        return result;
    }
    return "<code>" + source + "</code>";
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
