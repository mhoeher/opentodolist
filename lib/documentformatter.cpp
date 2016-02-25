/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2014 - 2015 Martin Höher <martin@rpdev.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "documentformatter.h"

#include <QTextBlockFormat>

DocumentFormatter::DocumentFormatter(QObject *parent) :
  QObject(parent),
  m_target( 0 ),
  m_document( 0 ),
  m_selectionStart( 0 ),
  m_selectionEnd( 0 ),
  m_currentParagraphStyle(UnknownParagraphStyle)
{
  connect(this, &DocumentFormatter::targetChanged, this, &DocumentFormatter::reset);
  connect(this, &DocumentFormatter::selectionStartChanged, this, &DocumentFormatter::reset);
  connect(this, &DocumentFormatter::selectionEndChanged, this, &DocumentFormatter::reset);
}

/**
   @brief The text editor the formatter is working on
 */
QQuickItem *DocumentFormatter::target() const
{
  return m_target;
}

/**
   @brief Sets the text editor the formatter is working on
   
   This sets the target component we are working on to @p target. The target must be
   a text editing component (e.g. TextEdit) which has a textDocument property.
 */
void DocumentFormatter::setTarget(QQuickItem *target)
{
  m_document = 0;
  m_target = target;
  if ( target ) {
    QVariant docAsVariant = target->property( "textDocument" );
    if ( docAsVariant.isValid() && docAsVariant.canConvert<QQuickTextDocument*>() ) {
      QQuickTextDocument *qqtd = docAsVariant.value<QQuickTextDocument*>();
      if ( qqtd ) {
        m_document = qqtd->textDocument();
      }
    }
  }
  emit targetChanged();
}

int DocumentFormatter::selectionStart() const
{
  return m_selectionStart;
}

void DocumentFormatter::setSelectionStart(int selectionStart)
{
  m_selectionStart = selectionStart;
  emit selectionStartChanged();
}

int DocumentFormatter::selectionEnd() const
{
  return m_selectionEnd;
}

void DocumentFormatter::setSelectionEnd(int selectionEnd)
{
  m_selectionEnd = selectionEnd;
  emit selectionEndChanged();
}

bool DocumentFormatter::bold() const
{
  QTextCursor cursor = wordCursor();
  return cursor.isNull() ? false : cursor.charFormat().fontWeight() == QFont::Bold;
}

void DocumentFormatter::setBold(bool bold)
{
  QTextCursor cursor = wordCursor();
  QTextCharFormat format;
  format.setFontWeight( bold ? QFont::Bold : QFont::Normal );
  cursor.mergeCharFormat( format );
  emit boldChanged();
}

bool DocumentFormatter::italic() const
{
  QTextCursor cursor = wordCursor();
  return cursor.isNull() ? false : cursor.charFormat().fontItalic();
}

void DocumentFormatter::setItalic(bool italic)
{
  QTextCursor cursor = wordCursor();
  QTextCharFormat format;
  format.setFontItalic( italic );
  cursor.mergeCharFormat( format );
  emit italicChanged();
}

bool DocumentFormatter::underline() const
{
  QTextCursor cursor = wordCursor();
  return cursor.isNull() ? false : cursor.charFormat().fontUnderline();
}

void DocumentFormatter::setUnderline(bool underline)
{
  QTextCursor cursor = wordCursor();
  QTextCharFormat format;
  format.setFontUnderline( underline );
  cursor.mergeCharFormat( format );
  emit underlineChanged();
}

bool DocumentFormatter::strikethrough() const
{
  QTextCursor cursor = wordCursor();
  return cursor.isNull() ? false : cursor.charFormat().fontStrikeOut();
}

void DocumentFormatter::setStrikethrough(bool strikethrough)
{
  QTextCursor cursor = wordCursor();
  QTextCharFormat format;
  format.setFontStrikeOut( strikethrough );
  cursor.mergeCharFormat( format );
  emit strikethroughChanged();
}

bool DocumentFormatter::alignLeft() const
{
  QTextCursor cursor = blockCursor();
  return cursor.isNull() ? false : cursor.blockFormat().alignment() == Qt::AlignLeft;
}

void DocumentFormatter::setAlignLeft(bool alignLeft)
{
  if ( alignLeft ) {
    setAlignment( Qt::AlignLeft );
  }
}

bool DocumentFormatter::alignCenter() const
{
  QTextCursor cursor = blockCursor();
  return cursor.isNull() ? false : cursor.blockFormat().alignment() == Qt::AlignCenter;
}

void DocumentFormatter::setAlignCenter(bool alignCenter)
{
  if ( alignCenter ) {
    setAlignment( Qt::AlignCenter );
  }
}

bool DocumentFormatter::alignRight() const
{
  QTextCursor cursor = blockCursor();
  return cursor.isNull() ? false : cursor.blockFormat().alignment() == Qt::AlignRight;
}

void DocumentFormatter::setAlignRight(bool alignRight)
{
  if ( alignRight ) {
    setAlignment( Qt::AlignRight );
  }
}

bool DocumentFormatter::justify() const
{
  QTextCursor cursor = blockCursor();
  return cursor.isNull() ? false : cursor.blockFormat().alignment() == Qt::AlignJustify;
}

void DocumentFormatter::setJustify(bool justify)
{
  if ( justify ) {
    setAlignment( Qt::AlignJustify );
  }
}

bool DocumentFormatter::unorderedList() const
{
  return false;
}

void DocumentFormatter::setUnorderedList(bool unorderedList)
{
  if ( unorderedList ) {
    QTextCursor cursor = blockCursor();
    cursor.createList( QTextListFormat::ListDisc );
  }
}

bool DocumentFormatter::orderedList() const
{
  return false;
}

void DocumentFormatter::setOrderedList(bool orderedList)
{
  if ( orderedList ) {
    QTextCursor cursor = blockCursor();
    cursor.createList( QTextListFormat::ListDecimal );
  }
}

DocumentFormatter::ParagraphStyle DocumentFormatter::paragraphStyle() const
{
  return UnknownParagraphStyle;
}

void DocumentFormatter::setParagraphStyle(DocumentFormatter::ParagraphStyle paragraphStyle)
{
  m_currentParagraphStyle = paragraphStyle;
  emit paragraphStyleChanged();
  if (m_document && paragraphStyle != UnknownParagraphStyle) {
    QTextCursor cursor = blockCursor();
    if (!cursor.isNull()) {
      QString selectedText = cursor.selectedText();
      QString surroundWith = "";
      static const QHash<ParagraphStyle,QString> SurroundWidth = {
        {H1, "h1"},
        {H2, "h2"},
        {H3, "h3"},
        {H4, "h4"},
        {H5, "h5"},
        {H6, "h6"},
        {Code, "pre"},
      };
      surroundWith = SurroundWidth.value(paragraphStyle);
      if (!surroundWith.isEmpty()) {
        selectedText = QString("<%1>%2</%1>").arg(surroundWith).arg(selectedText);
      }
      cursor.removeSelectedText();
      cursor.insertHtml(selectedText);
    }
  }
}

void DocumentFormatter::reset()
{
  m_currentParagraphStyle = UnknownParagraphStyle;
  emit boldChanged();
  emit italicChanged();
  emit underlineChanged();
  emit strikethroughChanged();
  emit alignLeftChanged();
  emit alignCenterChanged();
  emit alignRightChanged();
  emit justifyChanged();
  emit unorderedListChanged();
  emit orderedListChanged();
  emit paragraphStyleChanged();
}

void DocumentFormatter::increaseIndentation()
{
  if (m_document) {
    QTextCursor cursor = this->cursor();
    QTextBlockFormat format = cursor.blockFormat();
    format.setIndent(format.indent() + 1);
    cursor.mergeBlockFormat(format);
  }
}

void DocumentFormatter::decreaseIndentation()
{
  if (m_document) {
    QTextCursor cursor = this->cursor();
    QTextBlockFormat format = cursor.blockFormat();
    format.setIndent(format.indent() - 1);
    cursor.mergeBlockFormat(format);
  }
}

QTextCursor DocumentFormatter::cursor() const
{
  if ( m_document ) {
    QTextCursor cursor = QTextCursor( m_document );
    cursor.setPosition( m_selectionStart );
    if ( m_selectionStart != m_selectionEnd ) {
      cursor.setPosition( m_selectionEnd, QTextCursor::KeepAnchor );
    }
    return cursor;
  }
  return QTextCursor();
}

QTextCursor DocumentFormatter::wordCursor() const
{
  QTextCursor cursor = this->cursor();
  if ( cursor.isNull() ) {
    return cursor;
  }
  if ( !cursor.hasSelection() ) {
    cursor.select(QTextCursor::WordUnderCursor);
  }
  return cursor;
}

QTextCursor DocumentFormatter::blockCursor() const
{
  QTextCursor cursor = this->cursor();
  if ( cursor.isNull() ) {
    return cursor;
  }
  if ( !cursor.hasSelection() ) {
    cursor.select(QTextCursor::BlockUnderCursor);
  }
  return cursor;
}

void DocumentFormatter::setAlignment(Qt::Alignment alignment)
{
  QTextCursor cursor = blockCursor();
  QTextBlockFormat format;
  format.setAlignment( alignment );
  cursor.mergeBlockFormat( format );
  emit alignLeftChanged();
  emit alignCenterChanged();
  emit alignRightChanged();
  emit justifyChanged();
}
