pragma Singleton

import QtQuick 2.0

Item {
    id: fonts

    readonly property FontLoader fontAwesomeRegular: FontLoader {
        source: "./fontawesome-free-5.0.9/fa-regular-400.ttf"
        Component.onCompleted: {
            console.debug("Loaded font " + name)
        }
    }
    readonly property FontLoader fontAwesomeSolid: FontLoader {
        source: "./fontawesome-free-5.0.9/fa-solid-900.ttf"
        Component.onCompleted: {
            console.debug("Loaded font " + name)
        }
    }
    readonly property FontLoader fontAwesomeBrands: FontLoader {
        source: "./fontawesome-free-5.0.9/fa-brands-400.ttf"
        Component.onCompleted: {
            console.debug("Loaded font " + name)
        }
    }

    readonly property string icons: fonts.fontAwesomeRegular.name
    readonly property string solidIcons: fonts.fontAwesomeSolid.name
    readonly property string brands: fonts.fontAwesomeBrands.name
}
