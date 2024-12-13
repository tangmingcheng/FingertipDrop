

/*
This is a UI file (.ui.qml) that is intended to be edited in Qt Design Studio only.
It is supposed to be strictly declarative and only uses a subset of QML. If you edit
this file manually, you might introduce QML code that is not supported by Qt Design Studio.
Check out https://doc.qt.io/qtcreator/creator-quick-ui-forms.html for details on .ui.qml files.
*/
import QtQuick
import QtQuick.Controls
import FingertipDrop
import Backend

Item {
    id: root
    width: Constants.width
    height: Constants.height
    state: "start"

    Backend {
        id: backend
    }

    StartScrenn {
        id: startscreen
        anchors.fill: parent
        visible: true
    }

    ReadyScreen {
        id: readyscreen
        anchors.fill: parent
        visible: false
        httpAddress: backend.httpServerAddress
        httpPort: backend.httpServerPort
    }

    ConnectDeviceScreen {
        id: connectscreen
        anchors.fill: parent
        visible: false
        devicemodel: backend.listModel

    }

    Connections {
        target: startscreen
        onStartCliked: {
            backend.startButtonClicked()
            root.state = "ready"
        }
    }

    Connections {
        target: connectscreen
        onFileAccepted: {
            backend.sendFile(filePath)
        }
    }

    Connections {
        target: backend
        onUpdateClientStatus:function(connectActive) {
            console.log("connect", connectActive ? "连接中" : "无连接")
            root.state = connectActive ? "connect" : "disconnect"
        }
    }



    states: [
        State {
            name: "start"
            PropertyChanges {
                target: startscreen
                visible: true
            }
            PropertyChanges {
                target: readyscreen
                visible: false
            }
            PropertyChanges {
                target: connectscreen
                visible: false
            }
        },
        State {
            name: "ready"
            PropertyChanges {
                target: startscreen
                visible: false
            }
            PropertyChanges {
                target: readyscreen
                visible: true
                state: "startToReady"
            }
            PropertyChanges {
                target: connectscreen
                visible: false
            }
        },
        State {
            name: "connect"
            PropertyChanges {
                target: startscreen
                visible: false
            }
            PropertyChanges {
                target: readyscreen
                visible: false
            }
            PropertyChanges {
                target: connectscreen
                visible: true
            }
        }
    ]
}
