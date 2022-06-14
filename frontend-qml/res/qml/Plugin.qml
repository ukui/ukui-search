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

    signal itemClicked(string pluginId, int index);

    height: layout.childrenRect.height;

    Column {
        id: layout;
        anchors.fill: parent;
//        spacing: 5;

        PluginInfo {
            id: pluginInfo;
            width: parent.width;
            height: 30;

            img: "qrc:/icons/ukui-down-symbolic.svg";
            color: "#E3E3E3";
            radius: 5;

            index: root.index;
            name: root.pluginName;

            onExpandButtonClicked: expandAction();
        }

        //插件搜索结果展示
        ListView {
            id: resultView;

            width: parent.width;
            height: childrenRect.height; //跟随内容的数量调整视图高度

//            spacing: 1;
            clip: true;
            interactive: false; //禁用子级滚动

            model: root.resultModel;
            delegate: ResultItem {
                id: delegate;
                width: ListView.view ? ListView.view.width : 0;
                height: 40;

                color: "#F1F1F1";
                radius: 5;

                index: model.index;
                name: model.name;
                icon: model.icon;

                onItemClicked: (index) => {
                    root.itemClicked(root.pluginId, index);
                }
            }
        }
    }

    Component.onCompleted: {
        //组件加载完成后，链接信号
//        root.resultModel.onDataLoaded.connect(getSig);
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
}
