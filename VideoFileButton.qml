import QtQuick 2.0

Rectangle {
    id: videoFileButton
    property alias qualityText: qualityTextField.text
    property alias sizeText: sizeTextField.text
    property string url
    property string imageSource
    signal clicked

    width: 64
    height: 30
    color: "transparent"

    Column {
        anchors.right: parent.right
        anchors.rightMargin: 30
        Item {
            width: 1
            height: 4
        }
        Text {
            id: qualityTextField
            text: parent.text
            renderType: Text.NativeRendering
            font.family: "Helvetica"
            font.pixelSize: 8
        }
        Text {
            id: sizeTextField
            text: parent.sizeText
            renderType: Text.NativeRendering
            font.family: "Helvetica"
            font.pixelSize: 8
        }
    }
    Image {
        anchors.right: parent.right
        anchors.rightMargin: 2
        anchors.verticalCenter: parent.verticalCenter
        source: parent.imageSource
    }

    MouseArea {
        anchors.fill: parent
        onClicked: videoFileButton.clicked()
    }
}
