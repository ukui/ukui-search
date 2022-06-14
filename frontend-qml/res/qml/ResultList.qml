import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5

Rectangle {
    id: root;

    clip: true;
    color: "#FBFBFB";

    signal showItemDetails(string pluginId, int index);

    ListView {
        id: listView;

        anchors.fill: parent;

//        spacing: 5;
        clip: true;

        //contentHeight: childrenRect.height;
        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AsNeeded;
        }

        model: pluginModel;
        delegate: Plugin {
            width: ListView.view ? ListView.view.width : 0;

            index: model.index;
            pluginId: model.id;
            pluginName: model.name;
            description: model.description;
            pluginIconName: model.iconName;

            onIsExpandChanged: {
                console.log("展开信号", isExpand);
            }

            onItemClicked: (pluginId, index) => {
                showItemDetails(pluginId, index);
            }
        }
    }
}
