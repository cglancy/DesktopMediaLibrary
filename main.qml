import QtQuick 2.3
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1

ApplicationWindow {
    visible: true
    width: 640
    height: 480
    title: qsTr("Desktop Media Library")

    menuBar: MenuBar {
        Menu {
            title: qsTr("File")
            MenuItem {
                text: qsTr("&Open")
                onTriggered: console.log("Open action triggered");
            }
            MenuItem {
                text: qsTr("Exit")
                onTriggered: Qt.quit();
            }
        }
    }

    toolBar: ToolBar {
        RowLayout {
            anchors.fill: parent
            Item { Layout.fillWidth: true }
            TextField {
                id: searchField
                onTextChanged: controller.search(text)
            }
        }
    }

    SplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal

        TreeView {
            TableViewColumn {
                title: "Category"
                role: "display"
                width: 300
            }

            alternatingRowColors: false
            model: treeModel
            onClicked: {
                searchField.text = ""
                controller.setCategory(index)
            }
        }
        ScrollView {
            Layout.minimumWidth: 300
            Layout.fillWidth: true
            Layout.fillHeight: true
            GridView {
                anchors.fill: parent
                anchors.margins: 10
                model: listModel
                cellHeight: 200
                cellWidth: 210
                delegate: Column {
                    Text {
                        text: title
                        width: 192
                        height: 30
                        wrapMode: Text.WordWrap
                        elide: Text.ElideRight
                        verticalAlignment: Text.AlignBottom
                        renderType: Text.NativeRendering
                    }
                    Item {
                        height: 2; width: 192
                    }
                    Image {
                        source: thumbnailUrl
                        width: 192; height: 108
                        fillMode: Image.PreserveAspectCrop
                    }
                    Row {
                        spacing: 0
                        VideoFileButton {
                            text: highQualityFileResolution
                            url: highQualityFileUrl
                        }
                        VideoFileButton {
                            text: mediumQualityFileResolution
                            url: mediumQualityFileUrl
                        }
                        VideoFileButton {
                            text: lowQualityFileResolution
                            url: lowQualityFileUrl
                        }
                    }
                }
            }
        }
    }
}
