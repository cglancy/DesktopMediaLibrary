import QtQuick 2.0

Rectangle {
    property string text
    property string url
    property string imageSource
    width: 64
    height: 30
    color: "transparent"

    Text {
        anchors.right: parent.right
        anchors.rightMargin: 30
        anchors.verticalCenter: parent.verticalCenter
        text: parent.text
        renderType: Text.NativeRendering
        font.family: "Helvetica"
        font.pixelSize: 10
    }
    Image {
        anchors.right: parent.right
        anchors.rightMargin: 2
        anchors.verticalCenter: parent.verticalCenter
        source: parent.imageSource
    }

    MouseArea {
        anchors.fill: parent
        onClicked: console.log("Url = " + url)
    }
}
