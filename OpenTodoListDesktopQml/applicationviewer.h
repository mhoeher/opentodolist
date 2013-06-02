/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2013  Martin Höher <martin@rpdev.net>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef APPLICATIONVIEWER_H
#define APPLICATIONVIEWER_H

#include "qtquick2applicationviewer/qtquick2applicationviewer.h"

#include <QFileSystemWatcher>

class ApplicationViewer : public QtQuick2ApplicationViewer
{
    Q_OBJECT
    
public:
    
    explicit ApplicationViewer(QWindow *parent = 0);
    
    void addImportPath( const QString& path );
    void setMainFile( const QString& mainFile );
    
private:

    QFileSystemWatcher* m_watcher;    
    QString m_mainFile;
    
private slots:
    
    void reload();
    
};

#endif // APPLICATIONVIEWER_H
