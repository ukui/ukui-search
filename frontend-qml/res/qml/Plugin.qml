import QtQuick 2.0

Item {
    id: root;

    property int index; //该插件组件的索引
    property string pluginId; //插件id
    property string pluginName; //名称
    property string description;
    property bool isExpand : false; //控制该插件的数据区域 展开或收起
    property var resultModel : resultModelManager.getModel(root.pluginId);
    property string pluginIconName;

    signal itemSelected(int pluginIndex, string pluginId, int index);

    height: pluginLayout.childrenRect.height;

    function getPluginInfo() {
        var ret = {
            height: visible ? height : 0,
            resultIndex: resultList.currentIndex,
            resultCount: resultList.count,
        }

        return ret;
    }

    Column {
        id: pluginLayout;
        anchors.fill: parent;

        spacing: uiConfig.spacing("plugin");

        PluginInfo {
            id: pluginInfo;

            width: parent.width;

            img: "qrc:/icons/ukui-down-symbolic.svg";
            index: root.index;
            name: root.pluginName;

            onExpandButtonClicked: expandAction();
        }

        //插件搜索结果展示
        ListView {
            id: resultList;

            property int currentItemindex: -1;

            width: parent.width;
            height: childrenRect.height; //跟随内容的数量调整视图高度

            spacing: uiConfig.spacing("resultList");
            clip: true;
            interactive: false; //禁用子级滚动

            model: root.resultModel;
            delegate: ResultItem {
                width: ListView.view ? ListView.view.width : 0;

                index: model.index;
                name: model.name;
                icon: model.icon;

                onItemClicked: function(index) {
                    //点击事件用于更新currentIndex
                    if (index !== resultList.currentIndex) {
                        resultList.updateCurrentIndex(index);
                    }
                }

                onItemSelected: function(index) {
                    root.itemSelected(root.index, root.pluginId, index);
                }
            }

            function updateCurrentIndex(index) {
                if (resultList.currentIndex >= 0 && resultList.currentIndex < resultList.count) {
                    //清除处于选中状态的item
                    resultList.currentItem.selectItem(false);
                }

                if (index < 0 || index >= resultList.count) {
                    resultList.currentIndex = -1;
                    return false;

                } else {
                    resultList.currentIndex = index;
                    resultList.currentItem.selectItem(true);
                    return true;
                }
            }

            onCountChanged: {
                //当model中没有数据后，隐藏当前插件视图
                root.visible = (count > 0);
            }
        }
    }

    /**
      *
      * msg {
      *  up: false, //按键方向 上: true, 下:false
      *  border: false, //是上边界或者下边界: true, 不是边界:false
      *  top: false //上边界:true,下边界:false
      * }
      */
    function selectResultItem(msg) {
        if (!root.visible || resultList.count <= 0) {
            return false;
        }

        var index = -1;
        //处于未选中状态
        if (resultList.currentIndex < 0 || resultList.currentIndex >= resultList.count) {
            //重置起始位置
            index = msg.up ? (resultList.count - 1) : 0;

        } else {
            if (msg.up && msg.top && resultList.currentIndex === 0) {
                return true;
            }

            if (!msg.up && msg.bottom && resultList.currentIndex === (resultList.count - 1)) {
                return true;
            }

            index = resultList.currentIndex + (msg.up ? -1 : 1);
        }

        return resultList.updateCurrentIndex(index);
    }

    function clearSelectedItem() {
        if (resultList.currentIndex >= 0 && resultList.currentIndex < resultList.count) {
            resultList.currentItem.selectItem(false);
        }

        resultList.currentIndex = -1;  //重置为无效index
    }

    function expandAction() {
        root.isExpand = !root.isExpand;
        if (root.isExpand) {
            pluginInfo.img = "qrc:/icons/ukui-up-symbolic.svg";
            //展开数据
            resultModelManager.expand(root.pluginId);
        } else {
            pluginInfo.img = "qrc:/icons/ukui-down-symbolic.svg";
            resultModelManager.collapse(root.pluginId);
        }
    }

    function changeVisible(visible) {
        root.visible = visible;
    }

    function changeIconVisible(id, canExpand) {
        if (id === pluginId) {
            //控制展开按钮是否显示
            pluginInfo.actionIconVisable(canExpand);
        }
    }

    function dataCleared(id) {
        if (id === pluginId) {
            resultList.updateCurrentIndex(-1);
        }
    }

    Component.onCompleted: {
        resultList.currentIndex = -1;
        resultModelManager.modelCanExpand.connect(changeIconVisible);
        resultModelManager.modelDataCleared.connect(dataCleared);
        visible = false;
    }
}
