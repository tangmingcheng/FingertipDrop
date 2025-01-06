

/*
This is a UI file (.ui.qml) that is intended to be edited in Qt Design Studio only.
It is supposed to be strictly declarative and only uses a subset of QML. If you edit
this file manually, you might introduce QML code that is not supported by Qt Design Studio.
Check out https://doc.qt.io/qtcreator/creator-quick-ui-forms.html for details on .ui.qml files.
*/
import QtQuick
import QtQuick.Controls
import QtQuick.Timeline
import FingertipDrop 1.0

Item {
    id: root
    width: Constants.width
    height: Constants.height
    clip: true

    property string httpAddress: "192.168.0.1"
    property real httpPort: 8080
    property real buttonoutlneRotation

    signal readytoconnect

    Flatbackground {
        id: backgroundfull
        anchors.fill: parent
    }

    Image {
        id: buttonoutlne
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        source: "assets/buttonoutlne.png"

        MouseArea {
            id: readyMA
            visible: false
            anchors.fill: parent

            Connections {
                target: readyMA
                onClicked: {
                    console.log("readyMA clicked")
                }
            }
        }
    }
    Text {
        id: text1
        visible: true
        font.family: "Maven Pro"
        color: "#e544a1"
        text: qsTr("Ready!")
        font.pixelSize: 36
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Text {
        id: httpstatuslabel
        y: 235
        color: "#B8B8B8"
        text: "IP: " + httpAddress + " Port: " + httpPort
        font.pixelSize: 12
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors.horizontalCenterOffset: 0
        anchors.horizontalCenter: parent.horizontalCenter
        font.family: "Maven Pro"
    }
    Text {
        id: connectnumlabel
        y: 251
        color: "#B8B8B8"
        text: "Connect number: " + "0"
        font.pixelSize: 12
        horizontalAlignment: Text.AlignHCenter
        anchors.horizontalCenterOffset: 0
        anchors.horizontalCenter: parent.horizontalCenter
        font.family: "Maven Pro"
    }

    SequentialAnimation {
        id: sequentialAnimationRotion
        running: false
        loops: 1
        NumberAnimation {
            id:numberAnimation
            target: buttonoutlne
            property: "rotation"
            loops: Animation.Infinite
            from: 0
            to: 360
        }
    }
    Timeline {
        id: rotationAnimation
        animations: [
            TimelineAnimation {
                id: rotationTimeline
                from: 0
                to: 600
                duration: 5000 // 动画时长 5 秒
                loops: 1
                running: false
            }
        ]
        startFrame: 0
        endFrame: 600
        currentFrame: 0
        enabled: false

        keyframeGroups: [
            KeyframeGroup {
                target: buttonoutlne
                property: "rotation"

                Keyframe {
                    value: 0
                    frame: 600
                }
                Keyframe {
                    value: -360
                    frame: 300
                }
                Keyframe {
                    value: 360
                    frame: 200
                }
                Keyframe {
                    value: -360
                    frame: 50
                }
                Keyframe {
                    value: buttonoutlneRotation
                    frame: 0
                }
            },
            KeyframeGroup {
                target: readyMA
                property: "visible"
                Keyframe {
                    value: false
                    frame: 0
                }
                Keyframe {
                    value: true
                    frame: 600
                }
            }
        ]
    }

    Timeline {
        id: timeline

        animations: [
            TimelineAnimation {
                id: timelineanimation
                running: false
                loops: 1
                from: 0
                to: 120
                duration: 500
            }
        ]
        enabled: false
        currentFrame: 0
        startFrame: 0
        endFrame: 120
        keyframeGroups: [
            KeyframeGroup {
                target: buttonoutlne
                property: "scale"
                Keyframe {
                    value: 2
                    frame: 0
                }
                Keyframe {
                    value: 1
                    frame: 120
                }
            }
        ]
    }

    Timeline {
        id: readyToconnecttimline
        animations: [
            TimelineAnimation {
                id: readyToconnecttimelineanimation
                running: false
                loops: 1
                from: 0
                to: 240
                duration: 500
            }
        ]
        enabled: false
        currentFrame: 0
        startFrame: 0
        endFrame: 240
        keyframeGroups: [
            KeyframeGroup {
                target: buttonoutlne
                property: "scale"
                Keyframe {
                    value: 1
                    frame: 0
                }
                Keyframe {
                    value: 1.5
                    frame: 60
                }
                Keyframe {
                    value: 1.5
                    frame: 100
                }
                Keyframe {
                    value: 0
                    frame: 240
                }
            }
        ]
    }

    Connections {
        target: timelineanimation
        onFinished: {
            sequentialAnimationRotion.running = false
            buttonoutlneRotation = buttonoutlne.rotation
            rotationAnimation.enabled = true
            rotationTimeline.running = true

        }
    }

    Connections {
        target: rotationTimeline
        onFinished: {
            rotationAnimation.enabled = true
            rotationTimeline.running = false
            root.state = "ready"
        }
    }

    Connections {
        id: connections
        target: readyToconnecttimelineanimation
        onFinished: {
            readytoconnect()
        }
    }

    states: [
        State {
            name: "startToReady"
            PropertyChanges {
                target: timeline
                currentFrame: 0
                enabled: true
            }
            PropertyChanges {
                target: timelineanimation
                running: true
            }
            PropertyChanges {
                target: sequentialAnimationRotion
                running: true
            }
        },
        State {
            name: "ready"
            PropertyChanges {
                target: timeline
                currentFrame: 120
            }

            PropertyChanges {
                target: text1
                font.pixelSize: 36
            }

            PropertyChanges {
                target: httpstatuslabel
                y: 235
                font.pixelSize: 12
                anchors.horizontalCenterOffset: 0
            }

            PropertyChanges {
                target: connectnumlabel
                y: 251
                font.pixelSize: 12
                anchors.horizontalCenterOffset: 0
            }
        },
        State {
            name: "readyToConnect"
            PropertyChanges {
                target: timeline
                currentFrame: 120
            }
            PropertyChanges {
                target: readyToconnecttimline
                currentFrame: 0
                enabled: true
            }
            PropertyChanges {
                target: readyToconnecttimelineanimation
                running: true
            }
        }
    ]
}
