import QtQuick 2.0
import QtQuick.Layouts 1.12

Rectangle {
    id: root;

    property int index;
    property string img;
    property string name;

    signal expandButtonClicked;

    RowLayout {
        anchors.fill: parent;
        anchors.leftMargin: 5;

        spacing: 5;

        Text {
            id: pluginName;

            Layout.fillWidth: true;
            Layout.alignment: Qt.AlignVCenter;

            verticalAlignment: Qt.AlignVCenter;
            elide: Text.ElideRight;
            font.pixelSize: 18;
            text: root.name;
        }

        Image {
            id: actionIcon;

            Layout.preferredWidth: 24;
            Layout.preferredHeight: 24;
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
