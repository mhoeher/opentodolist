#include "syntaxhighlighter.h"

#ifdef HAVE_KF5_SYNTAX_HIGHLIGHTING
#    include <KF5/KSyntaxHighlighting/Definition>
#    include <KF5/KSyntaxHighlighting/Theme>
#endif

#include <QDebug>

/**
 * @brief Constructor.
 */
SyntaxHighlighter::SyntaxHighlighter(QObject *parent)
    : QObject(parent),
      m_highlighter(nullptr),
      m_repository(nullptr),
      m_document(nullptr),
      m_theme(Light)
{
#ifdef HAVE_KF5_SYNTAX_HIGHLIGHTING
    m_highlighter = new KSyntaxHighlighting::SyntaxHighlighter(this);
    m_repository = new KSyntaxHighlighting::Repository();
    m_highlighter->setDefinition(m_repository->definitionForFileName("test.md"));
    applyTheme();
#endif
}

SyntaxHighlighter::~SyntaxHighlighter()
{
#ifdef HAVE_KF5_SYNTAX_HIGHLIGHTING
    delete m_repository;
#endif
}

/**
 * @brief The text document the highlighter works on.
 */
QQuickTextDocument *SyntaxHighlighter::document() const
{
    return m_document;
}

/**
 * @brief Set the document to highlight.
 */
void SyntaxHighlighter::setDocument(QQuickTextDocument *document)
{
    if (m_document != document) {
        m_document = document;
        emit documentChanged();
#ifdef HAVE_KF5_SYNTAX_HIGHLIGHTING
        if (m_document != nullptr) {
            m_highlighter->setDocument(m_document->textDocument());
        } else {
            m_highlighter->setDocument(nullptr);
        }
#endif
    }
}

SyntaxHighlighter::Theme SyntaxHighlighter::theme() const
{
    return m_theme;
}

void SyntaxHighlighter::setTheme(const Theme &theme)
{
    if (m_theme != theme) {
        m_theme = theme;
        emit themeChanged();
        applyTheme();
    }
}

void SyntaxHighlighter::applyTheme()
{
    qWarning() << "Setting theme to" << m_theme;
#ifdef HAVE_KF5_SYNTAX_HIGHLIGHTING
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
#endif
}
