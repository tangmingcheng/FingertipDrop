/*
This is a UI file (.ui.qml) that is intended to be edited in Qt Design Studio only.
It is supposed to be strictly declarative and only uses a subset of QML. If you edit
this file manually, you might introduce QML code that is not supported by Qt Design Studio.
Check out https://doc.qt.io/qtcreator/creator-quick-ui-forms.html for details on .ui.qml files.
*/

import QtQuick
import QtQuick.Controls
import FingertipDrop 1.0

Item {
    id: _item
    width: Constants.width
    height: Constants.height

    Flatbackground {
        id: backgroundfull
        anchors.fill: parent
    }

    DeviceList {
        id: deviceList
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
    }



}
