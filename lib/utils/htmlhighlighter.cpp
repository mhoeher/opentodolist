/*
    Copyright (C) 2016 Volker Krause <vkrause@kde.org>
    Copyright (C) 2018 Christoph Cullmann <cullmann@kde.org>
    Copyright (C) 2021 Martin Hoeher <martin@rpdev.net>

    Permission is hereby granted, free of charge, to any person obtaining
    a copy of this software and associated documentation files (the
    "Software"), to deal in the Software without restriction, including
    without limitation the rights to use, copy, modify, merge, publish,
    distribute, sublicense, and/or sell copies of the Software, and to
    permit persons to whom the Software is furnished to do so, subject to
    the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "htmlhighlighter.h"

#ifdef OPENTODOLIST_USE_SYSTEM_KF_SYNTAX_HIGHLIGHTING
#    include <KSyntaxHighlighting/definition.h>
#    include <KSyntaxHighlighting/format.h>
#    include <KSyntaxHighlighting/state.h>
#    include <KSyntaxHighlighting/theme.h>
#else
#    include "../3rdparty/KDE/syntax-highlighting/src/lib/definition.h"
#    include "../3rdparty/KDE/syntax-highlighting/src/lib/format.h"
#    include "../3rdparty/KDE/syntax-highlighting/src/lib/state.h"
#    include "../3rdparty/KDE/syntax-highlighting/src/lib/theme.h"
#endif

#include <QBuffer>
#include <QIODevice>
#include <QTextStream>
#include <QVarLengthArray>

using namespace KSyntaxHighlighting;

class HtmlHighlighterPrivate
{
public:
    std::unique_ptr<QTextStream> out;
    std::unique_ptr<QBuffer> buffer;
    QString currentLine;
};

HtmlHighlighter::HtmlHighlighter() : d(new HtmlHighlighterPrivate())
{
    d->buffer.reset(new QBuffer());
    d->buffer->open(QIODevice::WriteOnly);
    d->out.reset(new QTextStream(d->buffer.get()));
    d->out->setCodec("UTF-8");
}

HtmlHighlighter::~HtmlHighlighter() {}

QString HtmlHighlighter::highlightData(QIODevice* dev, const QString& title)
{
    State state;
    *d->out << "<div ";
    if (theme().textColor(Theme::Normal))
        *d->out << " style=\"color:" << QColor(theme().textColor(Theme::Normal)).name() << "\"";
    *d->out << "><pre>\n";

    QTextStream in(dev);
    in.setCodec("UTF-8");
    while (!in.atEnd()) {
        d->currentLine = in.readLine();
        state = highlightLine(d->currentLine, state);
        *d->out << "\n";
    }

    *d->out << "</pre></div>\n";
    d->out->flush();

    d->out.reset();
    return d->buffer->buffer();
}

void HtmlHighlighter::applyFormat(int offset, int length, const Format& format)
{
    if (length == 0)
        return;

    // collect potential output, cheaper than thinking about "is there any?"
    QVarLengthArray<QString, 16> formatOutput;
    if (format.hasTextColor(theme()))
        formatOutput << QStringLiteral("color:") << format.textColor(theme()).name()
                     << QStringLiteral(";");
    if (format.hasBackgroundColor(theme()))
        formatOutput << QStringLiteral("background-color:")
                     << format.backgroundColor(theme()).name() << QStringLiteral(";");
    if (format.isBold(theme()))
        formatOutput << QStringLiteral("font-weight:bold;");
    if (format.isItalic(theme()))
        formatOutput << QStringLiteral("font-style:italic;");
    if (format.isUnderline(theme()))
        formatOutput << QStringLiteral("text-decoration:underline;");
    if (format.isStrikeThrough(theme()))
        formatOutput << QStringLiteral("text-decoration:line-through;");

    if (!formatOutput.isEmpty()) {
        *d->out << "<span style=\"";
        for (const auto& out : qAsConst(formatOutput)) {
            *d->out << out;
        }
        *d->out << "\">";
    }

    *d->out << d->currentLine.mid(offset, length).toHtmlEscaped();

    if (!formatOutput.isEmpty()) {
        *d->out << "</span>";
    }
}
