#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

#include <QObject>
#include <QQuickTextDocument>

#ifdef HAVE_KF5_SYNTAX_HIGHLIGHTING
#include <KF5/KSyntaxHighlighting/SyntaxHighlighter>
#include <KF5/KSyntaxHighlighting/Repository>
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
    Q_PROPERTY(QQuickTextDocument* document READ document WRITE setDocument
               NOTIFY documentChanged)
public:
    explicit SyntaxHighlighter(QObject *parent = nullptr);
    virtual ~SyntaxHighlighter();

    QQuickTextDocument *document() const;
    void setDocument(QQuickTextDocument *document);

signals:

    void documentChanged();

public slots:

private:
    KSyntaxHighlighting::SyntaxHighlighter *m_highlighter;
    KSyntaxHighlighting::Repository *m_repository;
    QQuickTextDocument *m_document;

};

#endif // SYNTAXHIGHLIGHTER_H
