/*
 *  OpenTodoList - A todo and task manager
 *  Copyright (C) 2013  Martin Höher <martin@rpdev.net>
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

#ifndef APPLICATIONVIEWER_H
#define APPLICATIONVIEWER_H

#include "qtquick2applicationviewer/qtquick2applicationviewer.h"

#include <QFileSystemWatcher>
#include <QVariantMap>

class ApplicationViewer : public QtQuick2ApplicationViewer
{
    Q_OBJECT
    Q_PROPERTY( QVariantList styleInfo READ styleInfo CONSTANT )
    Q_PROPERTY( QString basePath READ basePath CONSTANT )
    Q_PROPERTY( QString platformDefaultStyle READ platformDefaultStyle CONSTANT )
    Q_PROPERTY( QString currentStyle READ currentStyle WRITE setCurrentStyle NOTIFY currentStyleChanged )

    
public:
    
    explicit ApplicationViewer(const QString& basePath, QObject *parent = 0);
    virtual ~ApplicationViewer();
    
    void addImportPath( const QString& path );

    QString basePath() const;

    QVariantList styleInfo() const;

    QString platformDefaultStyle() const;

    QString currentStyle() const;
    void setCurrentStyle(const QString &currentStyle);

signals:

    void beforeReload();
    void reloaded();
    void currentStyleChanged();

public slots:

    void show();
    void reload();
    void loadSettings();
    void saveSettings();
    
private:

    QFileSystemWatcher* m_watcher;    
    QString m_basePath;
    QVariantList m_styleInfo;
    QString m_currentStyle;
    bool m_showing;

    static QVariantList loadStyleInfo( const QString& stylesDir );

private slots:
    
};

#endif // APPLICATIONVIEWER_H
