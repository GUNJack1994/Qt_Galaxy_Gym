import QtQuick 2.0
import QtQuick.Controls 1.1
import QtGraphicalEffects 1.0

Rectangle {
    color: "#646464"
    //    color: bgdColor
    id: root
    objectName: "root"
    property var lowOpacity: 0.2
    property var photoSource: "qrc:/images/images/Places-user-identity-icon.png"
    property var defMargin: 10
    property var defFontSize: 20
    property string userName: ""
    property string userSurname: ""
    property string userRfid: ""
    property var userDate: ""
    property var userTo: ""
    property bool userActive: false
    property string userTicketTypeDesc: ""
    property int textPointSize: 12
    property double userPrice: 0
    property var glowspeed: 5000
    property var carnetColor: ""
    property string userID: ""
    property bool isKey: false

    function reuse(id, name, surname, rfid, to, desc, price, photo, active, colorString) {
        userID = id
        photoSource = photo
        userName = name
        userSurname = surname
        userRfid = rfid
        userTo = to
        userActive = active
        userTicketTypeDesc = desc
        userPrice = price
        carnetColor = colorString
        isKey = false

        if (active) {
            normalAnimation.stop()
            invalidAnimation.stop()
            validAnimation.restart()
            screenTimer.restart()
        } else {
            normalAnimation.stop()
            validAnimation.stop()
            invalidAnimation.restart()
            screenTimer.restart()
        }
    }

    function showKey(name, surname, rfid) {
        userName = name
        userSurname = surname
        userRfid = rfid
        isKey = true

        normalAnimation.stop()
        invalidAnimation.stop()
        validAnimation.restart()
        screenTimer.restart()
    }

    Timer {
        id: screenTimer
        interval: 10000
        repeat: false
        onTriggered: {
            validAnimation.stop()
            invalidAnimation.stop()
            normalAnimation.restart()
        }
    }

    Image {
        id: glowBlue1
        opacity: 0
        //        anchors.fill: parent
        anchors.centerIn: parent
        source: "qrc:/images/images/glowBlue1.svg"
        width: logo.width
        height: logo.height
    }

    Image {
        id: glowBlue2
        opacity: 0
        //        anchors.fill: parent
        anchors.centerIn: parent
        source: "qrc:/images/images/glowBlue2.svg"
        width: logo.width
        height: logo.height
    }

    Image {
        id: glowGreen
        opacity: 0
        //        anchors.fill: parent
        anchors.centerIn: parent
        source: "qrc:/images/images/glowGreen.svg"
        width: logo.width
        height: logo.height
    }

    Image {
        id: glowRed
        opacity: 0
        //        anchors.fill: parent
        anchors.centerIn: parent
        source: "qrc:/images/images/glowRed.svg"
        width: logo.width
        height: logo.height
    }

    Image {
        id: logo
        anchors.centerIn: parent
        source: "qrc:/images/images/logo.png"
        width: parent.width > sourceSize.width ? sourceSize.width : parent.width
        fillMode: Image.PreserveAspectFit
    }

    Rectangle {
        opacity: 0
        id: mainRect

        color: "transparent"

        anchors.fill: parent

        Rectangle {
            id: cardRect
            clip: true
            anchors.centerIn: parent

            width: parent.width * 0.8 > 500 ? 500 : parent.width * 0.8
            height: parent.height * 0.8 > 300 ? 300 : parent.height * 0.8

            gradient: Gradient {
                GradientStop {position: 0; color: "white"}
                GradientStop {position: 0.5; color: "#E0E6F8"}
            }

            Item {
                property var columnWidh: width / 2
                id: leftSide
                anchors.left: parent.left
                anchors.top: parent.top
                width: parent.width * 0.6
                height: parent.height - bottomRect.height

                Item {
                    id: nameBox
                    anchors.top: parent.top
                    //                    anchors.topMargin: parent.height / 5 * 1
                    anchors.left: parent.left
                    anchors.margins: 10
                    height: nameColumn.height

                    Column {
                        id: nameColumn
                        spacing: 4
                        Text {
                            id: nameLabel
                            width: leftSide.columnWidh
                            text: "Nazwisko i imię:"
                            font.bold: true
                        }

                        Text {
                            text: userSurname + " " + userName
                            font.pointSize: textPointSize
                        }
                    }
                }

                Item {
                    id: idBox
                    anchors.top: parent.top
                    anchors.topMargin: parent.height / 4 * 1
                    anchors.left: parent.left
                    anchors.margins: 10
                    height: idColumn.height
                    visible: !isKey

                    Column {
                        id: idColumn
                        spacing: 4
                        Text {
                            id: idLabel
                            width: leftSide.columnWidh
                            text: "ID:"
                            font.bold: true
                        }

                        Text {
                            text: userID
                            font.pointSize: textPointSize
                        }
                    }
                }

                Item {
                    id: rfidBox
                    //                    anchors.verticalCenter: parent.verticalCenter
                    anchors.top: parent.top
                    anchors.topMargin: parent.height / 4 * 2
                    anchors.left: parent.left
                    anchors.margins: 10
                    height: rfidColumn.height

                    Column {
                        id: rfidColumn
                        spacing: 4
                        Text {
                            id: rfidLabel
                            width: leftSide.columnWidh
                            text: "RFID:"
                            font.bold: true
                        }

                        Text {
                            text: userRfid
                            font.pointSize: textPointSize
                        }
                    }
                }

                Item {
                    id: dateBox
                    //                    anchors.bottom: parent.bottom
                    anchors.top: parent.top
                    anchors.topMargin: parent.height / 4 * 3
                    anchors.left: parent.left
                    anchors.margins: 10
                    height: dateColumn.height
                    visible: !isKey

                    Row {
                        Column {
                            id: dateColumn
                            spacing: 4

                            Text {
                                width: leftSide.columnWidh
                                text: "Ważny do:"
                                font.bold: true
                            }

                            Text {
                                text: userActive ? Qt.formatDateTime(userTo, "yyyy-MM-dd") : "--/--"
                                font.pointSize: textPointSize
                            }
                        }
                    }
                }
            }

            Item {
                id: rightSide
                anchors.right: parent.right
                anchors.top: parent.top
                width: parent.width * 0.6
                height: parent.height- bottomRect.height
                visible: !isKey

                Rectangle {
                    id: photoRect
                    height: parent.height - anchors.margins < parent.width ? parent.height - anchors.margins : parent.width
                    width: height
                    color: "black"
                    anchors.top: parent.top
                    anchors.right: parent.right
                    anchors.margins: 10

                    Image {
                        id: userPhoto
                        source: photoSource
                        anchors.fill: parent
                    }

                    Rectangle {
                        color: "transparent"
                        border.width: 2
                        height: userPhoto.height + border.width
                        width: userPhoto.width + border.width
                        anchors.centerIn: userPhoto
                        border.color: "#E0E6F8"
                        radius: 2
                    }

                }

            }

            Rectangle {
                id: bottomRect
                width: parent.width
                height: 40
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                color: userActive ? carnetColor : "red"
                visible: !isKey

                Text {
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    text: userActive ? userTicketTypeDesc : "Konto nieopłacone"
                    verticalAlignment: Text.AlignVCenter
                    font.pointSize: textPointSize
                    color: "white"

                }



            }



        }

        DropShadow {
            anchors.fill: cardRect
            horizontalOffset: 3
            verticalOffset: 3
            radius: 8.0
            samples: 16
            color: "#80000000"
            source: cardRect
        }

        Rectangle {
            id: cardBorderRect
            color: "transparent";
            border.color: "black"
            border.width: 2
            width: cardRect.width
            height: cardRect.height
            //            anchors.fill: parent
            anchors.centerIn: cardRect
            anchors.margins: 10
            radius: 2
            clip: true
        }

        Rectangle {
            id: userBox
            anchors.left: cardRect.right
            anchors.top: cardRect.bottom
            anchors.margins: -50
            height: 60
            width: 60
            color: "#E0E6F8"
            radius: 4
            border.width: 2
            border.color: "black"
            visible: !isKey

            Image {
                id: userImage
                source: "qrc:/icons/user-identity.png"
                anchors.fill: parent


                MouseArea {
                    id: userBoxArea
                    anchors.fill: parent
                    hoverEnabled: true

                    onClicked: {
                        centerWidget.showUser(userRfid)
                    }
                    onContainsMouseChanged: {
                        userImage.scale = containsMouse ? 0.8 : 1
                    }
                }

            }
        }

        DropShadow {
            anchors.fill: userBox
            horizontalOffset: 4
            verticalOffset: 4
            radius: 8.0
            samples: 16
            color: "#80000000"
            source: userBox
            visible: !isKey
        }
    }

    SequentialAnimation {
        id: normalAnimation
        running: true

        ParallelAnimation {
            PropertyAnimation {
                target: glowGreen
                property: "opacity"
                to: 0
                duration: 200
            }

            PropertyAnimation {
                target: glowRed
                property: "opacity"
                to: 0
                duration: 200
            }

            PropertyAnimation {
                target: glowBlue1
                property: "opacity"
                to: 1
                duration: 200
            }

            PropertyAnimation {
                target: glowBlue2
                property: "opacity"
                to: 0
                duration: 200
            }

            PropertyAnimation {
                target: logo
                property: "opacity"
                to: 1
                duration: 200
            }

            PropertyAnimation {
                target: mainRect
                property: "opacity"
                to: 0
                duration: 200
            }



        }

        SequentialAnimation {
            loops: Animation.Infinite

            ParallelAnimation {
                PropertyAnimation {
                    target: glowBlue1
                    property: "opacity"
                    to: 0
                    duration: 300
                }

                PropertyAnimation {
                    target: glowBlue2
                    property: "opacity"
                    to: 1
                    duration: 300
                }
            }

            ParallelAnimation {
                PropertyAnimation {
                    target: glowBlue1
                    property: "opacity"
                    to: 1
                    duration: 300
                }

                PropertyAnimation {
                    target: glowBlue2
                    property: "opacity"
                    to: 0
                    duration: 300
                }
            }

        }
    }

    SequentialAnimation {
        id: validAnimation
        running: false

        ParallelAnimation {
            PropertyAnimation {
                target: glowGreen
                property: "opacity"
                to: 1
                duration: 200
            }

            PropertyAnimation {
                target: glowRed
                property: "opacity"
                to: 0
                duration: 200
            }

            PropertyAnimation {
                target: glowBlue1
                property: "opacity"
                to: 0
                duration: 200
            }

            PropertyAnimation {
                target: glowBlue2
                property: "opacity"
                to: 0
                duration: 200
            }

            PropertyAnimation {
                target: logo
                property: "opacity"
                to: 0
                duration: 200
            }

            PropertyAnimation {
                target: mainRect
                property: "opacity"
                to: 1
                duration: 200
            }

        }

        PropertyAnimation {
            target: glowGreen
            property: "opacity"
            to: 0
            duration: 200
        }

        SequentialAnimation {
            loops: 4

            PropertyAnimation {
                target: glowGreen
                property: "opacity"
                to: 1
                duration: 200
            }

            PropertyAnimation {
                target: glowGreen
                property: "opacity"
                to: 0
                duration: 200
            }

        }

        PropertyAnimation {
            target: glowBlue2
            property: "opacity"
            to: 1
            from: 0
            duration: glowspeed
        }

        SequentialAnimation {
            loops: Animation.Infinite



            ParallelAnimation {
                PropertyAnimation {
                    target: glowBlue1
                    property: "opacity"
                    from: 0
                    to: 1
                    duration: glowspeed
                }

                PropertyAnimation {
                    target: glowBlue2
                    property: "opacity"
                    to: 0
                    from: 1
                    duration: glowspeed
                }
            }

            ParallelAnimation {
                PropertyAnimation {
                    target: glowBlue1
                    property: "opacity"
                    to: 0
                    from: 1
                    duration: glowspeed
                }

                PropertyAnimation {
                    target: glowBlue2
                    property: "opacity"
                    to: 1
                    from: 0
                    duration: glowspeed
                }
            }

        }
    }

    SequentialAnimation {
        id: invalidAnimation
        running: false

        ParallelAnimation {
            PropertyAnimation {
                target: glowGreen
                property: "opacity"
                to: 0
                duration: 200
            }

            PropertyAnimation {
                target: glowRed
                property: "opacity"
                to: 1
                duration: 200
            }

            PropertyAnimation {
                target: glowBlue1
                property: "opacity"
                to: 0
                duration: 200
            }

            PropertyAnimation {
                target: glowBlue2
                property: "opacity"
                to: 0
                duration: 200
            }

            PropertyAnimation {
                target: logo
                property: "opacity"
                to: 0
                duration: 200
            }

            PropertyAnimation {
                target: mainRect
                property: "opacity"
                to: 1
                duration: 200
            }

        }

        PropertyAnimation {
            target: glowRed
            property: "opacity"
            to: 0
            duration: 200
        }

        SequentialAnimation {
            loops: 4

            PropertyAnimation {
                target: glowRed
                property: "opacity"
                to: 1
                duration: 200
            }

            PropertyAnimation {
                target: glowRed
                property: "opacity"
                to: 0
                duration: 200
            }

        }

        PropertyAnimation {
            target: glowBlue2
            property: "opacity"
            to: 1
            from: 0
            duration: glowspeed
        }

        SequentialAnimation {
            loops: Animation.Infinite

            ParallelAnimation {
                PropertyAnimation {
                    target: glowBlue1
                    property: "opacity"
                    from: 0
                    to: 1
                    duration: glowspeed
                }

                PropertyAnimation {
                    target: glowBlue2
                    property: "opacity"
                    to: 0
                    from: 1
                    duration: glowspeed
                }
            }

            ParallelAnimation {
                PropertyAnimation {
                    target: glowBlue1
                    property: "opacity"
                    to: 0
                    from: 1
                    duration: glowspeed
                }

                PropertyAnimation {
                    target: glowBlue2
                    property: "opacity"
                    to: 1
                    from: 0
                    duration: glowspeed
                }
            }

        }
    }

}


