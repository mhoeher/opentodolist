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
  Q_PROPERTY(ParagraphStyle paragraphStyle READ paragraphStyle WRITE setParagraphStyle NOTIFY paragraphStyleChanged)
public:
  
  enum ParagraphStyle {
    UnknownParagraphStyle,
    Default,
    H1,
    H2,
    H3,
    H4,
    H5,
    H6,
    Code
  };
  
  Q_ENUM(ParagraphStyle)
  
  explicit DocumentFormatter(QObject *parent = 0);
  
  QQuickItem *target() const;
  void setTarget(QQuickItem *target);
  
  int selectionStart() const;
  void setSelectionStart(int selectionStart);
  
  int selectionEnd() const;
  void setSelectionEnd(int selectionEnd);
  
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
  
  ParagraphStyle paragraphStyle() const;
  void setParagraphStyle(ParagraphStyle paragraphStyle);
  
signals:
  
  void targetChanged();
  void selectionStartChanged();
  void selectionEndChanged();
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
  void paragraphStyleChanged();
  
public slots:
  
  void reset();
  void increaseIndentation();
  void decreaseIndentation();
  
private:
  
  QQuickItem    *m_target;
  QTextDocument *m_document;
  int            m_selectionStart;
  int            m_selectionEnd;
  ParagraphStyle m_currentParagraphStyle;
  
  QTextCursor cursor() const;
  QTextCursor wordCursor() const;
  QTextCursor blockCursor() const;
  
  void setAlignment(Qt::Alignment alignment);
  
};

#endif // DOCUMENTFORMATTER_H
