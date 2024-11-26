

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
    id: root
    width: 300
    height: 70
    color: "#333333"

    clip: true
    property real slideOffset: -0
    property string connectstatus: "Connected"
    property alias deviceName: devicename.text
    property alias deviceIP: ip.text

    Behavior on scale {

        NumberAnimation {
            duration: 200
            easing.type: Easing.InOutQuad
        }
    }
    Behavior on slideOffset {

        NumberAnimation {
            duration: 500
            easing.type: Easing.InOutQuad
        }
    }

    Rectangle {
        id: maincontent
        x: slideOffset
        width: 380
        height: 70
        color: "#00ffffff"
        RowLayout {
            anchors.fill: parent
            antialiasing: true
            spacing: 0
            Rectangle {
                id: messagecontent
                width: 300
                height: root.height
                color: "#00ffffff"
                RowLayout {
                    anchors.fill: parent
                    antialiasing: true
                    spacing: 3

                    // 连接状态（红绿圆形指示灯）
                    Rectangle {
                        id: statusled
                        width: 10
                        height: 10
                        radius: width / 2
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        color: connectstatus === "Connected" ? "green" : "red"
                    }

                    // 设备名称
                    Text {
                        id: devicename
                        font.family: "Maven Pro"
                        color: "#e544a1"
                        text: qsTr("iphone")
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        font.bold: true
                        font.pointSize: 8
                    }

                    // IP 地址
                    Text {
                        id: ip
                        font.family: "Maven Pro"
                        text: "192.168.0.1"
                        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                        color: "#eaeaea"
                        font.pointSize: 8
                    }
                }
                MouseArea {
                    id: mousearea
                    anchors.fill: parent
                    propagateComposedEvents: true
                    preventStealing: true
                    hoverEnabled: true
                }
            }

            Button {
                id: disconnectbutton
                font.family: "Maven Pro"
                font.pointSize: 8
                text: qsTr("Disconnect")
                width: 80
                height: 70
                hoverEnabled: true
                y: 0
            }
        }
    }

    Connections {
        target: mousearea
        onEntered: {
            root.scale = 1.2
        }
        onExited: {
            // 将点击坐标映射到按钮的局部坐标
            var localPos = disconnectbutton.mapFromItem(mousearea,
                                                        mousearea.mouseX,
                                                        mousearea.mouseY)

            console.log(localPos)

            if (localPos.x >= -2 && localPos.x <= disconnectbutton.width
                    && localPos.y >= 0
                    && localPos.y <= disconnectbutton.height) {

                return
            }
            root.scale = 1
            if (slideOffset !== 0)
                slideOffset = 0
        }
        onClicked: {
            if (slideOffset === 0)
                slideOffset = -disconnectbutton.width
            else
                slideOffset = 0
        }
    }
    Connections {
        target: disconnectbutton
        onClicked: {
            console.log("disconnect clicked")
        }
    }
}
