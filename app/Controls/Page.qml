import QtQuick 2.10
import QtQuick.Controls 2.3 as QQC2

import "." as C

QQC2.Page {


    /**
     * @brief Save the page.
     *
     * This property holds a function which returns the state of the page. The default
     * value is a function that return null.
     *
     * A concrete page can return any value that is JSON representable. This value will be stored
     * and used when the app is restarted to reconstruct the state of the page stack.
     */
    property var savePage: function () {
        return null
    }


    /**
     * @brief Restrore the page.
     *
     * This function takes the @p state returned by the saveState function and restore the page
     * accordingly. The default implementation does nothing.
     */
    property var restorePage: function (state) {}


    /**
     * @brief A URL used to reconstruct the page.
     *
     * If the page shall be re-constructable, this property must be set to URL from which the
     * page will be created from.
     *
     * If the URL is invalid, the page is not re-constructable.
     */
    property url restoreUrl: ""
}
