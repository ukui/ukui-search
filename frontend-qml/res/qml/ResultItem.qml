import QtQuick 2.0
import QtQuick.Layouts 1.12
import org.ukui.search.items 1.0

Rectangle {
    id: root;

    property int index;
    property string name;
    property var icon;
    property bool selected: false;

    signal itemClicked(int index);
    signal itemSelected(int index);

    height: uiConfig.height("resultItem");
    radius: uiConfig.radius("resultItem");

    RowLayout {
        id: resultItemLayout;
        anchors.fill: parent;
        anchors.leftMargin: uiConfig.margin("resultItem", 0);

        spacing: uiConfig.spacing("resultItem");

        IconItem {
            id: itemIcon;

            icon: root.icon;

            Layout.alignment: Qt.AlignCenter;
            Layout.preferredHeight: uiConfig.width("itemIcon");
            Layout.preferredWidth: uiConfig.height("itemIcon");
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

        hoverEnabled: true;
        onClicked: {
            root.itemClicked(root.index);
        }
        onEntered: {
            if (!root.selected) {
                root.color = uiConfig.color("hover");
            }
        }
        onExited: {
            if (!root.selected) {
                root.color = "transparent";
            }
        }
    }

    function selectItem(selected) {
        if (root.selected !== selected) {
            root.selected = selected;
            root.color = root.selected ? uiConfig.color("select") : "transparent";
            if (root.selected) {
                root.itemSelected(root.index);
            }
        }
    }

    Component.onCompleted: {
        //组件加载完成后执行
    }
}

