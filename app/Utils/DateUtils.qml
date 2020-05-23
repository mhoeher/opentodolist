pragma Singleton

import QtQuick 2.0

Item {
    function validDate(date) {
        // Invalid dates are unequal to themselves
        return date !== null && date.getTime() === date.getTime();
    }

    function format(date) {
        if (date) {
            return date.toLocaleDateString();
        } else {
            return "";
        }
    }

    function today() {
        var now = new Date();
        var result = new Date(now.getFullYear(),
                             now.getMonth(),
                             now.getDate());
        return result;
    }

    function tomorrow() {
        var now = new Date();
        var result = new Date(now.getFullYear(),
                             now.getMonth(),
                             now.getDate() + 1);
        return result;
    }

    function endOfThisWeek() {
        var now = new Date();
        var locale = Qt.locale();
        var lastDayOfWeek = (locale.firstDayOfWeek + 6) % 7;
        var result = new Date(now.getFullYear(),
                              now.getMonth(),
                              now.getDate());
        while (result.getDay() !== lastDayOfWeek) {
            result = new Date(result.getFullYear(), result.getMonth(), result.getDate() + 1);
        }
        return result;
    }

    function endOfNextWeek() {
        var date = endOfThisWeek();
        return new Date(date.getFullYear(), date.getMonth(), date.getDate() + 7);
    }
}
