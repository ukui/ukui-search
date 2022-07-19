import QtQuick 2.0
import QtQuick.Controls 2.5

Item {
    id: root;

    property ScrollBar scrollerBar;
    signal pluginItemSelected(string pluginId, int index);

    focus: true;
    clip: true;

    Flickable {
        id: scrollerArea; //主体滚动区域

        property int stepHeight: uiConfig.height("resultItem");

        anchors.fill: parent;

        ScrollBar.vertical: root.scrollerBar;
        contentHeight: pluginList.height; //上下滚动
        clip: true;

        onContentHeightChanged: {
            //当数据的高度低于界面时,不显示滚动条
            root.scrollerBar.visible = (contentHeight > height);
        }

        Column {
            id: pluginList;

            property int selectedIndex: -1;

            width: parent.width;
            height: childrenRect.height; //动态调整高度

            spacing: uiConfig.spacing("pluginList");

            Repeater {
                id: pluginRepeater; //用于定位每一个 Plugin{} 元素。

                model: pluginModel;
                delegate: Plugin {
                    width: parent.width;

                    index: model.index;
                    pluginId: model.id;
                    pluginName: model.name;
                    description: model.description;
                    pluginIconName: model.iconName;

                    //visible: model.rowCount > 0;

                    onIsExpandChanged: {
                        console.log("展开信号", isExpand);
                    }

                    onVisibleChanged: {
                        pluginList.forceLayout();
                    }

                    onItemSelected: function(pluginIndex, pluginId, index) {
                        pluginList.updateSelectedPlugin(pluginIndex);
                        pluginList.updateContentY(pluginIndex);
                        root.pluginItemSelected(pluginId, index);
                    }
                }
            }

            function updateSelectedPlugin(pluginIndex) {
                //当某个item被选中时，可能是通过点击事件选中的，所以需要将其他插件的结果列表中已经选中的item清除掉
                if (pluginIndex !== pluginList.selectedIndex) {
                    if (pluginList.selectedIndex >= 0 && pluginList.selectedIndex < pluginRepeater.count) {
                        pluginRepeater.itemAt(pluginList.selectedIndex).clearSelectedItem();
                    }

                    pluginList.selectedIndex = pluginIndex;
                }
            }

            function updateContentY(pluginIndex) {
                var height = 0;
                for (var index = 0; index <= pluginIndex; ++index) {
                    var plugin = pluginRepeater.itemAt(index);
                    if (plugin === undefined) {
                        continue;
                    }

                    var ret = plugin.getPluginInfo();
                    if (index < pluginIndex) {
                        height += ret.height;

                    } else {
                        //取出当前index所在contentHeight中的位置
                        var tmp = (ret.resultCount - ret.resultIndex - 1) * scrollerArea.stepHeight;
                        height += (ret.height - tmp);
                    }
                }

                scrollerArea.scrollToY(height);
            }
        }

        function scrollToY(indexY) {
            var border = indexY;
            if (indexY <= (contentY + stepHeight)) {
                //contentY + stepHeight 滚动区域的上边界
                border = contentY + stepHeight;

            } else if (indexY >= (height + contentY)) {
                //height + contentY 滚动区域的下边界
                border = height + contentY - stepHeight;
            }

            scrollContentArea(Math.floor(indexY - border));
        }

        function scrollOneStep(up) {
            scrollContentArea(up ? -stepHeight : stepHeight);
        }

        function scrollContentArea(y) {
            if (y === 0 || scrollerArea.contentHeight <= scrollerArea.height) {
                return;
            }

            var newContentY = scrollerArea.contentY + y;
            if (newContentY < 0) {
                newContentY = 0;

            } else if(newContentY > (scrollerArea.contentHeight - scrollerArea.height)) {
                newContentY = scrollerArea.contentHeight - scrollerArea.height;
            }

            scrollerArea.contentY = newContentY;
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

    Keys.onLeftPressed: function() {
        scrollerArea.scrollOneStep(true);
    }

    Keys.onRightPressed: function() {
        scrollerArea.scrollOneStep(false);
    }

    function selectPluginItem(up) {
        if (pluginRepeater.count <= 0) {
            return;
        }

        var border = pluginListBorder();
        if (border.top === -1) {
            return;
        }

        if (pluginList.selectedIndex < 0 || pluginList.selectedIndex >= pluginRepeater.count) {
            //超出范围，重置索引为0
            pluginList.selectedIndex = 0;
        }

        var index = pluginList.selectedIndex;

        var msg = {
            up: up,
            border: false,
            top: false,
            bottom: false
        }

        //对每个插件进行循环，每循环一次，代表遍历一个插件
        while(index >= 0 && index < pluginRepeater.count) {
            msg.top = (index === border.top);
            msg.bottom = (index === border.bottom);

            pluginList.selectedIndex = index;
            var plugin = pluginRepeater.itemAt(index);
            var ret = plugin.selectResultItem(msg);

            if (ret) {
                break;
            }

            index += up ? -1 : 1;
        }

        console.log("Plugin view current selected plugin:", pluginList.selectedIndex, "\n");
    }

    function pluginListBorder() {
        //插件列表的上下边界元素
        var border = {
            top: -1,
            bottom: -1
        }

        var plugin = null;
        for (var i = 0; i < pluginRepeater.count; ++i) {
            plugin = pluginRepeater.itemAt(i);
            if(plugin.visible) {
                if (border.top === -1) {
                    //初始上下边界一样
                    border.top = i;
                    border.bottom = i;
                } else {
                    border.bottom = i;
                }
            }
        }

        return border;
    }

    function dataCleared(id) {
        //从头开始索引插件数据
        pluginList.selectedIndex = -1;
        scrollerArea.contentY = 0;
    }

    Component.onCompleted: {
        resultModelManager.modelDataCleared.connect(dataCleared);
    }
}
