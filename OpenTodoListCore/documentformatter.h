#ifndef DOCUMENTFORMATTER_H
#define DOCUMENTFORMATTER_H

#include <QObject>
#include <QQuickTextDocument>
#include <QQuickItem>
#include <QTextCursor>
#include <QTextCharFormat>

/**
   @brief Handles formatting of a text editing component in QML

   This class is intended to be used with a QML text editing component. It is used
   to change formatting of text (which is not directly possible inside QML code).
 */
class DocumentFormatter : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QQuickItem* target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(int selectionStart READ selectionStart WRITE setSelectionStart NOTIFY selectionStartChanged)
    Q_PROPERTY(int selectionEnd READ selectionEnd WRITE setSelectionEnd NOTIFY selectionEndChanged)
    Q_PROPERTY(int cursorPosition READ cursorPosition WRITE setCursorPosition NOTIFY cursorPositionChanged)
    Q_PROPERTY(bool bold READ bold WRITE setBold NOTIFY boldChanged)
    Q_PROPERTY(bool italic READ italic WRITE setItalic NOTIFY italicChanged)
    Q_PROPERTY(bool underline READ underline WRITE setUnderline NOTIFY underlineChanged)
    Q_PROPERTY(bool strikethrough READ strikethrough WRITE setStrikethrough NOTIFY strikethroughChanged )
    Q_PROPERTY(bool alignLeft READ alignLeft WRITE setAlignLeft NOTIFY alignLeftChanged)
    Q_PROPERTY(bool alignCenter READ alignCenter WRITE setAlignCenter NOTIFY alignCenterChanged)
    Q_PROPERTY(bool alignRight READ alignRight WRITE setAlignRight NOTIFY alignRightChanged)
    Q_PROPERTY(bool justify READ justify WRITE setJustify NOTIFY justifyChanged)
    Q_PROPERTY(bool unorderedList READ unorderedList WRITE setUnorderedList NOTIFY unorderedListChanged)
    Q_PROPERTY(bool orderedList READ orderedList WRITE setOrderedList NOTIFY orderedListChanged)
public:
    explicit DocumentFormatter(QObject *parent = 0);

    QQuickItem *target() const;
    void setTarget(QQuickItem *target);

    int selectionStart() const;
    void setSelectionStart(int selectionStart);

    int selectionEnd() const;
    void setSelectionEnd(int selectionEnd);

    int cursorPosition() const;
    void setCursorPosition(int cursorPosition);

    bool bold() const;
    void setBold( bool bold );

    bool italic() const;
    void setItalic( bool italic );

    bool underline() const;
    void setUnderline( bool underline );

    bool strikethrough() const;
    void setStrikethrough( bool strikethrough );

    bool alignLeft() const;
    void setAlignLeft( bool alignLeft );

    bool alignCenter() const;
    void setAlignCenter( bool alignCenter );

    bool alignRight() const;
    void setAlignRight( bool alignRight );

    bool justify() const;
    void setJustify( bool justify );

    bool unorderedList() const;
    void setUnorderedList( bool unorderedList );

    bool orderedList() const;
    void setOrderedList( bool orderedList );

signals:

    void targetChanged();
    void selectionStartChanged();
    void selectionEndChanged();
    void cursorPositionChanged();
    void boldChanged();
    void italicChanged();
    void underlineChanged();
    void strikethroughChanged();
    void alignLeftChanged();
    void alignCenterChanged();
    void alignRightChanged();
    void justifyChanged();
    void unorderedListChanged();
    void orderedListChanged();

public slots:

    void reset();

private:

    QQuickItem    *m_target;
    QTextDocument *m_document;
    int            m_selectionStart;
    int            m_selectionEnd;
    int            m_cursorPosition;

    QTextCursor cursor() const;
    QTextCursor wordCursor() const;
    QTextCursor blockCursor() const;

    void setAlignment(Qt::Alignment alignment);

};

#endif // DOCUMENTFORMATTER_H