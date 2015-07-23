import QtQuick 2.0

Rectangle {
    property string text
    property string url
    width: 64
    height: 30
    color: "black"

    Text {
        color: "white"
        anchors.fill: parent
        anchors.leftMargin: 4
        verticalAlignment: Text.AlignVCenter
        text: parent.text
        renderType: Text.NativeRendering
        font.family: "Helvetica"
        font.pixelSize: 10
    }
    Image {
        anchors.right: parent.right
        anchors.rightMargin: 2
        anchors.verticalCenter: parent.verticalCenter
        source: "images/download.png"
    }

    MouseArea {
        anchors.fill: parent
        onClicked: console.log("Url = " + url)
    }
}
