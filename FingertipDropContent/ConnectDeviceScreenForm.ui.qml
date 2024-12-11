

/*
This is a UI file (.ui.qml) that is intended to be edited in Qt Design Studio only.
It is supposed to be strictly declarative and only uses a subset of QML. If you edit
this file manually, you might introduce QML code that is not supported by Qt Design Studio.
Check out https://doc.qt.io/qtcreator/creator-quick-ui-forms.html for details on .ui.qml files.
*/
import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import FingertipDrop 1.0

Item {
    id: _item
    width: Constants.width
    height: Constants.height
    property alias devicemodel: deviceList.devicelistModel
    signal fileAccepted(string filePath)

    Flatbackground {
        id: backgroundfull
        anchors.fill: parent
    }

    DeviceList {
        id: deviceList
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 20
    }

    Text {
        id: filename
        x: 14
        y: 167
        color: "#b34cdc"
        font.pixelSize: 12
    }

    Button {
        id: button
        x: 8
        y: 201
        text: qsTr("Choose File")

        Connections {
            target: button
            onClicked: filedialog.open()
        }
    }

    FileDialog {
        id: filedialog
        title: "选择文件"
        nameFilters: ["所有文件 (*)"]
        currentFolder: Qt.platform.os
                       === "windows" ? "file:///C:/" : Qt.platform.os
                                       === "macos" ? "file:///Users/" : "file:///home/"
    }

    Connections {
        target: filedialog
        onAccepted: {
            var filePath = String(filedialog.selectedFile)
            var fileName = filePath.substring(filePath.lastIndexOf("/") + 1)
            filename.text = fileName
            fileAccepted(filedialog.selectedFile)
        }
    }
}
