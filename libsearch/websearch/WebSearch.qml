import QtQuick 2.0
import QtQuick.Layouts 1.12
import org.ukui.search.items 1.0

Item {
    id: root;

    height: 300;

    function update(detailsData) {
        icon.icon = detailsData.icon;
//        name.text = detailsData.name;

        repeater.model = detailsData.actions;
    }

    ColumnLayout {
        id: layout;

        anchors.fill: parent;

        spacing: 5;

        IconItem {
            id: icon;

            Layout.preferredWidth: 128;
            Layout.preferredHeight: 128;

            Layout.alignment: Qt.AlignHCenter;
        }

//        Text {
//            id: name;

//            clip: true;
//            elide: Text.ElideRight;
//            wrapMode: Text.NoWrap;

//            Layout.alignment: Qt.AlignHCenter;
//        }

        Column {
            id: actions;

            spacing: 5;

            Layout.fillWidth: true;
            Layout.fillHeight: true;

            Repeater {
                id: repeater;

                delegate: Item {
                    width: parent.width;
                    height: 50;

                    Text {
                        anchors.fill: parent;

                        clip: true;
                        color: "blue";
                        elide: Text.ElideRight;
                        wrapMode: Text.NoWrap;
                        verticalAlignment: Qt.AlignVCenter;
                        horizontalAlignment: Qt.AlignHCenter;

                        text: modelData[1];
                    }
                }
            }
        }
    }
}
