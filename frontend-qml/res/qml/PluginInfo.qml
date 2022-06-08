import QtQuick 2.0
import QtQuick.Layouts 1.12

Rectangle {
    id: root;

    property string img;
    property int index;
    property string name;

    signal expandButtonClicked;

    RowLayout {
        anchors.fill: parent;
        spacing: 5;

        Text {
            id: pluginName;

            Layout.fillWidth: true;
            Layout.alignment: Qt.AlignVCenter;

            verticalAlignment: Qt.AlignVCenter;
            elide: Text.ElideRight;
            font.family: "Ubuntu";
            font.pixelSize: 18;
            text:  root.index + " : " + root.name;
        }

        Image {
            id: actionIcon;

            Layout.minimumWidth: 24;
            Layout.minimumHeight: 24;
            Layout.alignment: Qt.AlignCenter;

            source: root.img;
            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    expandButtonClicked();
                }
            }
        }
    }
}
