

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
    id: _item
    width: Constants.width
    height: Constants.height

    signal startCliked
    signal positionChanged(real x, real y)
    property alias startbuttomRotation: startbutton.rotation
    property int startbuttonWidth: 300
    property int startbuttonHeight: 300
    property alias rotationAnimSpeed: rotationAnimation.duration
    property real currentButtonScale

    Flatbackground {
        id: backgroundfull
        anchors.fill: parent
    }
    Text {
        id: startlabel
        color: "#eaeaea"
        text: qsTr("Start")
        anchors.verticalCenter: parent.verticalCenter
        font.pixelSize: 40
        font.family: "Maven Pro"
        style: Text.Outline
        anchors.horizontalCenter: parent.horizontalCenter
    }

    Image {
        id: startbutton
        width: 300
        height: 300
        anchors.verticalCenter: parent.verticalCenter
        source: "assets/drumcopy_temp.png"
        anchors.horizontalCenter: parent.horizontalCenter
        fillMode: Image.PreserveAspectFit

        MouseArea {
            id: startmouseArea
            anchors.fill: parent
            hoverEnabled: true
        }
    }

    SequentialAnimation {
        id: sequentialAnimationRotion
        paused: true
        running: true
        loops: Animation.Infinite

        ParallelAnimation {
            NumberAnimation {
                id: rotationAnimation
                target: startbutton
                property: "rotation"
                from: 0
                to: 360
            }
        }
    }

    SequentialAnimation {
        id: sequentialAnimationScale
        paused: true
        running: true
        loops: Animation.Infinite
        NumberAnimation {
            target: startbutton
            property: "scale"
            from: 1
            to: 0.9
            duration: 1000
            easing.type: Easing.InOutQuad
        }
        NumberAnimation {
            target: startbutton
            property: "scale"
            from: 0.9
            to: 1
            duration: 1000
            easing.type: Easing.InOutQuad
        }
    }

    Timeline {
        id: clickAnimation
        animations: [
            TimelineAnimation {
                id: timeanimation
                from: 0
                to: 120
                duration: 500
                loops: 1
                running: false
            }
        ]
        currentFrame: 0
        startFrame: 0
        endFrame: 120
        enabled: false

        keyframeGroups: [
            KeyframeGroup {
                target: startbutton
                property: "scale"
                Keyframe {
                    value: 2
                    frame: 120
                }
                Keyframe {
                    value: currentButtonScale
                    frame: 0
                }
            }
        ]
    }

    Connections {
        target: startmouseArea
        onClicked: {
            console.log("clicked")
            sequentialAnimationScale.running = false
        }
        onPositionChanged: {
            _item.positionChanged(startmouseArea.mouseX, startmouseArea.mouseY)
        }
        onEntered: {
            sequentialAnimationRotion.paused = false
        }
        onExited: {
            sequentialAnimationRotion.paused = true
        }
    }

    Connections {
        target: startmouseArea
        onEntered: {
            sequentialAnimationScale.paused = false
        }
        onExited: {
            sequentialAnimationScale.paused = true
        }
    }

    Connections {
        target: sequentialAnimationScale
        onFinished: {
            console.log("finished")
            currentButtonScale = startbutton.scale
            clickAnimation.enabled = true
            timeanimation.running = true
        }
    }

    Connections {
        target: timeanimation
        onFinished: {
            console.log("time animation finshed")
            startCliked()
        }
    }
}
