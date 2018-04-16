pragma Singleton

import QtQuick 2.0

Item {
    function validDate(date) {
        // Invalid dates are unequal to themselves
        return date.getTime() === date.getTime();
    }

    function format(date) {
        if (date) {
            return date.toLocaleDateString();
        } else {
            return "";
        }
    }
}
