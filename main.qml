import QtQuick 2.3
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.1

ApplicationWindow {
    visible: true
    width: 1024
    height: 768
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
                width: 300
                placeholderText: "Search"
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

            Layout.fillWidth: true
            alternatingRowColors: false
            model: treeModel
            onClicked: {
                searchField.text = ""
                controller.setCategory(index)
            }
        }
        ScrollView {
            Layout.minimumWidth: 640

            Layout.fillHeight: true
            ListView {
                anchors.fill: parent
                anchors.margins: 10
                model: listModel
                spacing: 10
                contentHeight: 138
                contentWidth: 600
                delegate: Row {
                    Column {
                        Image {
                            source: thumbnailUrl
                            width: 192; height: 108
                            fillMode: Image.PreserveAspectCrop
                        }
                        Row {
                            spacing: 0
                            VideoFileButton {
                                qualityText: highQualityFileResolution
                                sizeText: highQualityFileSize
                                url: highQualityFileUrl
                                imageSource: onlineOnly ? "" : "images/download_24x24.png"
                            }
                            VideoFileButton {
                                qualityText: onlineOnly ? "" : mediumQualityFileResolution
                                sizeText: mediumQualityFileSize
                                url: mediumQualityFileUrl
                                imageSource: mediumQualityFileUrl ? "images/download_24x24.png" : ""
                            }
                            VideoFileButton {
                                qualityText: onlineOnly ? "" : lowQualityFileResolution
                                sizeText: lowQualityFileSize
                                url: lowQualityFileUrl
                                imageSource: lowQualityFileUrl ? "images/download_24x24.png" : ""
                            }
                        }
                    }
                    Column {
                        width: 4
                        height: 1
                    }
                    Column {
                        Text {
                            text: title
                            width: 408
                            wrapMode: Text.WordWrap
                            //elide: Text.ElideRight
                            renderType: Text.NativeRendering
                            font.pointSize: 10
                            font.family: "Helvetica"
                            font.bold: true
                        }
                        Item {
                            height: 2; width: 408
                        }
                        Text {
                            text: summary
                            width: 408
                            wrapMode: Text.WordWrap
                            renderType: Text.NativeRendering
                            font.pointSize: 8
                            font.family: "Helvetica"
                        }
                        Item {
                            height: 2; width: 408
                        }
                        Text {
                            text: "Length: " + length
                            width: 408
                            wrapMode: Text.WordWrap
                            renderType: Text.NativeRendering
                            font.pointSize: 8
                            font.family: "Helvetica"
                        }
                    }
                }
            }
        }
    }
}
