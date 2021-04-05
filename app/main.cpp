/*
 * Copyright 2020, 2021 Martin Hoeher <martin@rpdev.net>
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

#include "appstartup.h"

int main(int argc, char* argv[])
{
    qWarning() << "Entered main";
#ifdef OPENTODOLIST_FLATPAK
    {
        auto xdgRuntimeDir = qgetenv("XDG_RUNTIME_DIR");
        qputenv("TMPDIR", xdgRuntimeDir + "/app/net.rpdev.OpenTodoList");
    }
#endif
    AppStartup appStartup;
    qWarning() << "Starting app";
    return appStartup.exec(argc, argv);
}
