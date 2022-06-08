import QtQuick 2.0
import QtQuick.Layouts 1.12
import org.ukui.search.items 1.0

Rectangle {
    id: root;
    property int index;
    property string name;
    property string iconName;
    property string pluginIconName;
    property string description;

    signal itemClicked(int index);

    RowLayout {
        id: layout;
        anchors.fill: parent;
        spacing: 5;

//        Image {
//            id: itemIcon;

//            Layout.alignment: Qt.AlignCenter;
//            Layout.minimumHeight: 24;
//            Layout.maximumHeight: 24;
//            Layout.minimumWidth: 24;
//            Layout.maximumWidth: 24;

//            fillMode: Image.PreserveAspectFit;
//            source: "qrc:/icons/desktop.png";
//        }

        IconItem {
            id: itemIcon;

            backup: root.pluginIconName;
            name: root.iconName;

            Layout.alignment: Qt.AlignCenter;
            Layout.minimumHeight: 24;
            Layout.maximumHeight: 24;
            Layout.minimumWidth: 24;
            Layout.maximumWidth: 24;

        }

        Text {
            id: itemText;

            Layout.alignment: Qt.AlignVCenter;
            Layout.fillWidth: true;

            verticalAlignment: Qt.AlignVCenter;
            elide: Text.ElideRight;
            wrapMode: Text.NoWrap;

            text: root.index + " : " + root.name;
        }
    }

    MouseArea {
        anchors.fill: parent;
        onClicked: (mouseEvent) => {
            console.log("点击：", root.index, root.name);
            root.itemClicked(index);
        }
    }

    Component.onCompleted: {
        //组件加载完成后执行
        console.log("===qml icon:", root.iconName, root.pluginIconName);
    }
}

