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

#ifndef UTILS_TEXTUTILS_H_
#define UTILS_TEXTUTILS_H_

#include <QObject>
#include <QQuickTextDocument>

#include "syntaxhighlighting.h"

class TextUtils : public QObject
{
    Q_OBJECT

public:
    explicit TextUtils(QObject* parent = nullptr);

    Q_INVOKABLE QString markdownToHtml(const QString& text, const QString& stylesheet = QString(),
                                       SyntaxHighlighter* highlighter = nullptr) const;

    Q_INVOKABLE QString htmlToPlainText(const QString& html) const;

private:
    void fixFormatting(QTextDocument& doc) const;
    void highlightCodeBlocks(QTextDocument& doc, SyntaxHighlighter* highlighter) const;
};

#endif // UTILS_TEXTUTILS_H_
