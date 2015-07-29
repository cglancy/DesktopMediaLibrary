import QtQuick 2.3
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1

Rectangle {

    Row {
        Column {
            Button {
                text: "Browse..."
            }
            Button {
                text: "Export"
            }
        }
    }

    gradient: Gradient {
        GradientStop { position: 0 ; color: "#828282" }
        GradientStop { position: 1 ; color: "#404040" }
    }

    states: [
        State {
            name: "export"
            when: exportButton.checked == true
            PropertyChanges {
                target: exportPanel
                height: 100
            }
        }
    ]

    transitions: Transition {
        NumberAnimation {
            property: "height"
            duration: 200
            easing.type: Easing.InOutQuad
        }
    }
}

