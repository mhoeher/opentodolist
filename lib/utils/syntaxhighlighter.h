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

#ifndef UTILS_SYNTAXHIGHLIGHTER_H_
#define UTILS_SYNTAXHIGHLIGHTER_H_

#include <QObject>
#include <QQuickTextDocument>

#ifdef HAVE_KF5_SYNTAX_HIGHLIGHTING
#    include <KF5/KSyntaxHighlighting/SyntaxHighlighter>
#    include <KF5/KSyntaxHighlighting/Repository>
#else
namespace KSyntaxHighlighting {
class SyntaxHighlighter;
class Repository;
}
#endif

/**
 * @brief Syntax highligher for QML.
 */
class SyntaxHighlighter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQuickTextDocument *document READ document WRITE setDocument NOTIFY documentChanged)
    Q_PROPERTY(Theme theme READ theme WRITE setTheme NOTIFY themeChanged)
public:
    enum Theme { Light, Dark };

    Q_ENUM(Theme)

    explicit SyntaxHighlighter(QObject *parent = nullptr);
    virtual ~SyntaxHighlighter();

    QQuickTextDocument *document() const;
    void setDocument(QQuickTextDocument *document);

    Theme theme() const;
    void setTheme(const Theme &theme);

signals:

    void documentChanged();
    void themeChanged();

public slots:

private:
    KSyntaxHighlighting::SyntaxHighlighter *m_highlighter;
    KSyntaxHighlighting::Repository *m_repository;
    QQuickTextDocument *m_document;
    Theme m_theme;

    void applyTheme();
};

#endif // UTILS_SYNTAXHIGHLIGHTER_H_
