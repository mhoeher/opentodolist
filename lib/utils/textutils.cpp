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

#include "textutils.h"

#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextFormat>
#include <QTextBlock>
#include <QRegularExpression>
#include <QDomDocument>

/**
 * @class TextUtils
 *
 * Utilities to work with text such as formatting and converting between formats.
 */

/**
 * @brief Constructor.
 */
TextUtils::TextUtils(QObject* parent) : QObject(parent) {}

/**
 * @brief Converts the given markdown text into HTML.
 *
 * This takes the @p text (which is supposed to be in markdown format) and converts it to HTML.
 *
 * If a syntax @p highlighter is given, code blocks will be formatted accordingly.
 *
 * If a @p stylesheet it given, it will be injected into the generated HTML. This can be
 * used to style elements such as links.
 */
QString TextUtils::markdownToHtml(const QString& text, const QString& stylesheet,
                                  SyntaxHighlighter* highlighter) const
{
    QTextDocument doc;
    doc.setDefaultStyleSheet("");
    doc.setMarkdown(text, QTextDocument::MarkdownDialectGitHub);

    fixFormatting(doc);

    // Apply syntax highlighting
    highlightCodeBlocks(doc, highlighter);

    auto html = doc.toHtml();

    if (!stylesheet.isEmpty()) {
        auto endOfStyle = html.indexOf("</style>", Qt::CaseInsensitive);
        if (endOfStyle >= 0) {
            html.insert(endOfStyle, stylesheet);
        }
    }

    html = removeBodyStyle(html);

    return html;
}

/**
 * @brief Converts HTML into plain text.
 *
 * This function gets an @p html string as input and returns the text converted
 * to plain text.
 */
QString TextUtils::htmlToPlainText(const QString& html) const
{
    QTextDocument doc;
    doc.setHtml(html);
    return doc.toPlainText();
}

/**
 * @brief Fix some formatting issues.
 *
 * This function fixes several formatting issues that Qt's internal Markdown
 * parser introduces.
 */
void TextUtils::fixFormatting(QTextDocument& doc) const
{
    QTextCharFormat noForegroundColorFormat;
    noForegroundColorFormat.setForeground(QBrush());

    for (auto block = doc.begin(); block != doc.end(); block = block.next()) {
        // Add some space before and after each heading
        {
            auto format = block.blockFormat();
            if (format.property(QTextFormat::HeadingLevel).toInt() > 0) {
                format.setTopMargin(10);
                format.setBottomMargin(10);
                QTextCursor cursor(block);
                cursor.select(QTextCursor::LineUnderCursor);
                cursor.mergeBlockFormat(format);
            }
        }

        // Remove foreground color from links, so we can style them via CSS later.
        for (auto fragmentIt = block.begin(); fragmentIt != block.end(); ++fragmentIt) {
            auto fragment = fragmentIt.fragment();
            auto format = fragment.charFormat();

            if (format.isAnchor()) {
                QTextCursor cursor(block);
                cursor.select(QTextCursor::BlockUnderCursor);
                cursor.mergeCharFormat(noForegroundColorFormat);
            }
        }
    }
}

/**
 * @brief Highlight code blocks.
 *
 * This searches all code blocks in the given @p doc and applies syntaxt highlighting on them
 * using the given @p highlighter.
 */
void TextUtils::highlightCodeBlocks(QTextDocument& doc, SyntaxHighlighter* highlighter) const
{
    if (highlighter != nullptr) {
        bool finishedHighlighting = false;
        while (!finishedHighlighting) {
            finishedHighlighting = true;
            for (auto block = doc.begin(); block != doc.end(); block = block.next()) {
                auto blockFormat = block.blockFormat();
                auto blockCodeLanguage = blockFormat.property(QTextFormat::BlockCodeLanguage);
                auto isHighlighted = blockFormat.property(QTextFormat::UserProperty).toBool();
                if (!blockCodeLanguage.isNull() && !isHighlighted) {
                    // We need to do another round:
                    finishedHighlighting = false;

                    // Position the text cursor on the first block of this language:
                    QTextCursor cursor(block);
                    cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);

                    // Search for the last block of this language:
                    block = block.next();
                    while (block.blockFormat().property(QTextFormat::BlockCodeLanguage)
                                   == blockCodeLanguage
                           && block != doc.end()) {
                        cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);
                        cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
                        block = block.next();
                    }

                    // Mark the text as already being formatted
                    QTextBlockFormat newBlockFormat;
                    newBlockFormat.setProperty(QTextFormat::UserProperty, true);
                    cursor.mergeBlockFormat(newBlockFormat);

                    // Get the text, transform it and replace the content
                    auto code = cursor.selectedText() + "\n";
                    auto formattedCode =
                            highlighter->sourceToHtml(code, blockCodeLanguage.toString());
                    cursor.removeSelectedText();
                    cursor.insertHtml(formattedCode);

                    // Finish this round
                    break;
                }
            }
        }
    }
}

/**
 * @brief Remove the style section from the HTML body element.
 *
 * This prevents us from having hard-coded font metrics in the generated HTML.
 */
QString TextUtils::removeBodyStyle(const QString& html) const
{
    auto index = html.indexOf("<body");
    if (index >= 0) {
        const QString BeginOfStyle = "style=\"";
        index = html.indexOf(BeginOfStyle, index);
        if (index >= 0) {
            auto closingIndex = html.indexOf("\"", index + BeginOfStyle.length());
            if (closingIndex >= 0) {
                return html.mid(0, index) + html.mid(closingIndex + 1);
            }
        }
    }
    return html;
}
