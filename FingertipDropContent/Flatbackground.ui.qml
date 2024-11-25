/*
This is a UI file (.ui.qml) that is intended to be edited in Qt Design Studio only.
It is supposed to be strictly declarative and only uses a subset of QML. If you edit
this file manually, you might introduce QML code that is not supported by Qt Design Studio.
Check out https://doc.qt.io/qtcreator/creator-quick-ui-forms.html for details on .ui.qml files.
*/

import QtQuick
import QtQuick.Controls
import FingertipDrop
import QtQuick.Studio.DesignEffects

Item {
    id: flatbackground
    width: Constants.width
    height: Constants.height

    Rectangle {
        id:background
        color: "#333333"
        anchors.fill: parent
        antialiasing: true
    }
}
