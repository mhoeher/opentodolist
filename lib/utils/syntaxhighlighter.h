#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

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

#endif // SYNTAXHIGHLIGHTER_H
