import QtQuick 2.0
import QtQuick.Layouts 1.12
import org.ukui.search.items 1.0

Rectangle {
    id: root;
    property int index;
    property string name;
    property var icon;

    signal itemClicked(int index);

    RowLayout {
        id: layout;
        anchors.fill: parent;
        anchors.leftMargin: 5;

        spacing: 5;

        IconItem {
            id: itemIcon;

            icon: root.icon;

            Layout.alignment: Qt.AlignCenter;
            Layout.preferredHeight: 24;
            Layout.preferredWidth: 24;
        }

        Text {
            id: itemText;

            Layout.alignment: Qt.AlignVCenter;
            Layout.fillWidth: true;

            verticalAlignment: Qt.AlignVCenter;
            elide: Text.ElideRight;
            wrapMode: Text.NoWrap;

            text: root.name;
        }
    }

    MouseArea {
        anchors.fill: parent;
        onClicked: (mouseEvent) => {
            root.itemClicked(index);
        }
    }

    Component.onCompleted: {
        //组件加载完成后执行
    }
}

