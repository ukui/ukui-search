import QtQuick 2.0
import QtQuick.Layouts 1.12

Rectangle {
    id: root;

    property string name;
    property string type;

    clip: true;
    color: "transparent";

    RowLayout {
        id: infoLayout;

        anchors.fill: parent;
        spacing: 10;

        Text {
            id: infoName;
            text: root.name;

//            font.family: "Ubuntu";
            font.pixelSize: 20;
            verticalAlignment: Qt.AlignVCenter;
            elide: Text.ElideRight;
            wrapMode: Text.NoWrap;

            Layout.fillWidth: true;
            Layout.preferredHeight: root.height;
            Layout.alignment: Qt.AlignLeft;
        }

        Text {
            id: infoType;
            text: root.type;

            color: "gray";
//            font.family: "Ubuntu";
            font.pixelSize: 14;
            verticalAlignment: Qt.AlignVCenter;
            horizontalAlignment: Qt.AlignRight;
            elide: Text.ElideRight;
            wrapMode: Text.NoWrap;

            Layout.minimumWidth: 50;
            Layout.maximumWidth: 150;
            Layout.preferredHeight: root.height;
            Layout.alignment: Qt.AlignRight;
        }
    }
}
