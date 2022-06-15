import QtQuick 2.0
import QtQuick.Layouts 1.12

Item {
    id: root;
    property var dataModel;
    property int spacing: 10;

    height: listView.height;

    ListView {
        id: listView;

        width: parent.width;
        height: childrenRect.height;

        clip: true;
        interactive: false; //禁用子级滚动
        spacing: root.spacing;

        model: root.dataModel;
        delegate: Rectangle {
            id: infoDelegate;
            property int index: model.index;
            property var modelData: model.modelData;

            width: ListView.view ? ListView.view.width : 0;
            height: 25;

            color: "transparent";

            RowLayout {
                anchors.fill: parent;
                Text {
                    id: key;
                    text: infoDelegate.modelData.k;

//                    font.family: "Ubuntu";
                    font.pixelSize: 16;
                    verticalAlignment: Qt.AlignVCenter;
                    elide: Text.ElideRight;
                    wrapMode: Text.NoWrap;

                    Layout.minimumWidth: 100;
                    Layout.maximumWidth: 150;
                    Layout.fillHeight: true;
                    Layout.alignment: Qt.AlignLeft;
                }

                Text {
                    id: value;
                    text: infoDelegate.modelData.v;

                    color: "gray";
//                    font.family: "Ubuntu";
                    font.pixelSize: 16;
                    verticalAlignment: Qt.AlignVCenter;
                    horizontalAlignment: Qt.AlignRight;
                    elide: Text.ElideRight;
                    wrapMode: Text.NoWrap;

                    Layout.fillWidth: true;
                    Layout.fillHeight: true;
                    Layout.alignment: Qt.AlignRight;
                }
            }
        }
    }
}
