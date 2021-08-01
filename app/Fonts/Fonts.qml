pragma Singleton

import QtQuick 2.0

Item {
    id: fonts

    readonly property string icons: materialDesignIcons.name

    FontLoader {
        id: materialDesignIcons
        source: "./material-design-icons/font/MaterialIconsOutlined-Regular.otf"
    }
}
