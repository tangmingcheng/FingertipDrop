

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
    state: "normal"

    property string httpAddress: "192.168.0.1"
    property real httpPort: 8080
    property real buttonoutlneRotation
    signal readyClicked

    Flatbackground {
        id: backgroundfull
        anchors.fill: parent
    }

    Image {
        id: backgroundcout
        y: -2
        anchors.horizontalCenterOffset: -2
        source: "assets/backgroundcutout.png"
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Image {
        id: buttonoutlne
        width: 300
        height: 300
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
                    readyClicked()
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
        font.pixelSize: 40
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Text {
        id: httpstatuslabel
        y: 366
        color: "#B8B8B8"
        text: "IP: " + httpAddress + " Port: " + httpPort
        font.pixelSize: 21
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        anchors.horizontalCenterOffset: 0
        anchors.horizontalCenter: parent.horizontalCenter
        font.family: "Maven Pro"
    }
    Text {
        id: connectnumlabel
        y: 397
        color: "#B8B8B8"
        text: "Connect number: " + "0"
        font.pixelSize: 18
        horizontalAlignment: Text.AlignHCenter
        anchors.horizontalCenterOffset: 0
        anchors.horizontalCenter: parent.horizontalCenter
        font.family: "Maven Pro"
    }

    SequentialAnimation {
        id: sequentialAnimationRotion
        running: false
        loops: Animation.Infinite

        NumberAnimation {
            target: buttonoutlne
            property: "rotation"
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
        enabled: true
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
            },
            KeyframeGroup {
                target: sequentialAnimationRotion
                property: "running"
                Keyframe {
                    value: true
                    frame: 0
                }
            }
        ]
    }

    Connections {
        target: timelineanimation
        onFinished: {
            root.state = "ready"
            sequentialAnimationRotion.running = false
        }
    }
    Connections {
        target: sequentialAnimationRotion
        onFinished: {
            buttonoutlneRotation = buttonoutlne.rotation
            rotationAnimation.enabled = true
            rotationTimeline.running = true
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
        },
        State {
            name: "ready"
            PropertyChanges {
                target: timeline
                currentFrame: 120
            }
        }
    ]
}
