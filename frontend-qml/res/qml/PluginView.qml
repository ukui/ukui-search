import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5

Rectangle {
    id: root;

    clip: true;
    color: uiConfig.color("pluginView");
    radius: uiConfig.radius("pluginView");

    signal showItemDetails(string pluginId, int index);

    RowLayout {
        id: pluginViewLayout;

        anchors.fill: parent;
        spacing: uiConfig.spacing("pluginView");

        Item {
            id: pluginContainer;

            Layout.fillWidth: true;
            Layout.fillHeight: true;

            focus: true;

            ListView {
                id: pluginList;

                property int selectedPluginItem: -1;

                anchors.fill: parent;

                clip: true;
                spacing: uiConfig.spacing("pluginList");
                focus: false;

                ScrollBar.vertical: scrollBar;

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

                    onItemSelected: function(pluginIndex, pluginId, index) {
                        pluginList.updateSelectedPlugin(pluginIndex);
                        showItemDetails(pluginId, index);
                    }
                }

                function updateSelectedPlugin(pluginIndex) {
                    //当某个item被选中时，可能是通过点击事件选中的，所以需要将其他插件的结果列表中已经选中的item清除掉
                    if (pluginIndex !== pluginList.selectedPluginItem) {
                        if (pluginList.currentIndex >= 0 && pluginList.currentIndex < pluginList.count) {
//                            var up = (pluginIndex - pluginList.selectedPluginItem) > 0 ? false : true;
                            pluginList.currentItem.clearSelectedItem();
                        }

                        pluginList.selectedPluginItem = pluginIndex;
                        pluginList.currentIndex = pluginIndex;
                    }
                }
            }

            Keys.onUpPressed: function(event) {
                selectPluginItem(true);
                event.accepted = false;
            }
            Keys.onDownPressed: function(event) {
                selectPluginItem(false);
                event.accepted = false;
            }

            function selectPluginItem(up) {
                if (pluginList.count <= 0) {
                    return;
                }

                if (pluginList.selectedPluginItem < 0 || pluginList.selectedPluginItem >= pluginList.count) {
                    //超出范围，重置索引为0
                    pluginList.selectedPluginItem = 0;
                }

                var index = pluginList.selectedPluginItem;
                //对每个插件进行循环，每循环一次，代表遍历一个插件
                while(index >= 0 && index < pluginList.count) {
                    pluginList.currentIndex = index; //定位元素，跟currentItem配对使用
                    pluginList.selectedPluginItem = index;

                    if (pluginList.currentItem.selectResultItem(up)) {
                        break;

                    } else {
                        index += up ? -1 : 1;
                    }
                }

                console.log("Plugin view current selected plugin:", pluginList.selectedPluginItem, "\n");
            }
        }

        ScrollBar {
            id: scrollBar;

            Layout.preferredWidth: uiConfig.width("scrollBar");
            Layout.fillHeight: true;
            visible: pluginList.childrenRect.height > root.height;

            size: root.height / pluginList.childrenRect.height;
            policy: ScrollBar.AsNeeded;
        }

        Component.onCompleted: {
            pluginList.currentIndex = -1;
        }
    }
}
