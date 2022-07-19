import QtQuick 2.0

Item {
    id: root;

    property var actionModel;
    property int spacing: 10;

    height: listView.height;

    signal actionClicked(int actionId);

    ListView {
        id: listView;

//        anchors.fill: parent;
        width: parent.width;
        height: childrenRect.height;

        clip: true;
        interactive: false; //禁用子级滚动
        spacing: root.spacing;

        model: root.actionModel;
        delegate: Rectangle {
            id: actionDelegate

            width: ListView.view ? ListView.view.width : 0;
            height: 25;

            color: "transparent";

            Text {
                id: action;
                anchors.fill: parent;

                color: "#008DFF";

                text: modelData[1];
                font.pixelSize: 16
                verticalAlignment: Qt.AlignVCenter;
                horizontalAlignment: Qt.AlignLeft;
                elide: Text.ElideRight;
                wrapMode: Text.NoWrap;

                MouseArea {
                    anchors.fill: parent;

                    onClicked: {
                        root.actionClicked(modelData[0]);
                    }
                }
            }
        }
    }
}
