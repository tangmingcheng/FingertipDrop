

/*
This is a UI file (.ui.qml) that is intended to be edited in Qt Design Studio only.
It is supposed to be strictly declarative and only uses a subset of QML. If you edit
this file manually, you might introduce QML code that is not supported by Qt Design Studio.
Check out https://doc.qt.io/qtcreator/creator-quick-ui-forms.html for details on .ui.qml files.
*/
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: rectangle
    width: 350
    height: 420
    color: "#333333"

    // 模拟数据模型
    ListModel {
        id: deviceModel
        ListElement {
            name: "Device 1"
            ip: "192.168.0.1"
            status: "Connected"
        }
        ListElement {
            name: "Device 2"
            ip: "192.168.0.2"
            status: "Disconnected"
        }
        ListElement {
            name: "Device 3"
            ip: "192.168.0.3"
            status: "Connected"
        }
    }

    // 自定义列表
    ListView {
        id: listView
        anchors.fill: parent
        model: deviceModel

        spacing: 10

        delegate: DeviceListItem {
            anchors.horizontalCenter: parent.horizontalCenter

            deviceName: model.name
            deviceIP: model.ip
            connectstatus: model.status
        }

        // 空内容提示
        ScrollBar.vertical: ScrollBar {}
    }

    // 测试动态更新的工具按钮
    Button {
        id: addbutton
        text: "Add Device"
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        Connections {
            target: addbutton
            onClicked: {
                deviceModel.append({
                                       "name": "Device " + (deviceModel.count + 1),
                                       "ip": "192.168.0." + (deviceModel.count + 1),
                                       "status": "Disconnected"
                                   })
            }
        }
    }
}
