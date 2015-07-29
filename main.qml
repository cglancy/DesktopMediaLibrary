import QtQuick 2.3
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1

ApplicationWindow {
    visible: true
    width: 1024
    height: 768
    title: qsTr("Desktop Media Library")

    //    menuBar: MenuBar {
    //        Menu {
    //            title: qsTr("File")
    //            MenuItem {
    //                text: qsTr("&Open")
    //                onTriggered: console.log("Open action triggered");
    //            }
    //            MenuItem {
    //                text: qsTr("Exit")
    //                onTriggered: Qt.quit();
    //            }
    //        }
    //    }

    toolBar: ToolBar {

        style: ToolBarStyle {
            background: Rectangle {
                //border.color: "#313131"
                gradient: Gradient {
                    GradientStop { position: 0 ; color: "#828282" }
                    GradientStop { position: 1 ; color: "#404040" }
                }
            }
        }

        RowLayout {
            anchors.fill: parent

            ToolButton {
                id: gearButton
                text: "Options"
                tooltip: "Change application options"
                iconSource: "gear.png"
                onClicked: console.log("options")
            }

            ToolButton {
                id: exportButton
                checkable: true
                text: "Export"
                tooltip: "Exports video files"
                iconSource: "export.png"
                onClicked: {
                    if (exportButton.checked)
                        exportColumn.visible = true
                    else
                        exportColumn.visible = false
                }
            }
            Item {
                Layout.fillWidth: true
            }
            TextField {
                id: searchField
                Layout.minimumWidth: 200
                placeholderText: "Search"
                onTextChanged: controller.search(text)
            }
        }
    }

    Component {
        id: checkBoxDelegate
        Item {
            CheckBox {
                id: checkBox
                anchors.fill: parent
                checkedState: styleData.value
                //partiallyCheckedEnabled: true
                onClicked: {
                    controller.setCategoryExport(styleData.index, checkBox.checkedState)
                }
            }
        }
    }


    SplitView {
        id: splitView
        width: parent.width
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: exportPanel.top

        orientation: Qt.Horizontal

        TreeView {
            id: treeView

            TableViewColumn {
                title: "Category"
                role: "name"
                width: 300
            }
            TableViewColumn {
                id: exportColumn
                visible: false
                title: "Export"
                role: "export"
                width: 50
                delegate: checkBoxDelegate
            }

            Layout.fillWidth: true
            alternatingRowColors: false
            model: treeModel
            onActivated: {
                searchField.text = ""
                controller.setCategory(index)
            }
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
                                imageSource: onlineOnly ? "" : highQualityFileProgressImage
                                onClicked: controller.clickVideoButton(highQualityFileUrl)
                            }
                            VideoFileButton {
                                qualityText: onlineOnly ? "" : mediumQualityFileResolution
                                sizeText: mediumQualityFileSize
                                url: mediumQualityFileUrl
                                imageSource: mediumQualityFileProgressImage
                                onClicked: controller.clickVideoButton(mediumQualityFileUrl)
                            }
                            VideoFileButton {
                                qualityText: onlineOnly ? "" : lowQualityFileResolution
                                sizeText: lowQualityFileSize
                                url: lowQualityFileUrl
                                imageSource: lowQualityFileProgressImage
                                onClicked: controller.clickVideoButton(lowQualityFileUrl)
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

    ExportPanel {
        id: exportPanel

        width: parent.width
        height: 0
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }

}
