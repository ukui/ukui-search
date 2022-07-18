import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5
import org.ukui.search.utils 1.0

Rectangle {
    id: root;

    property string pluginId;
    property int dataIndex: -1;
    property var currentData: null; //当前显示内容的数据备份
    property var currentType: null; //当前显示的搜索结果的类型
    property var currentActionKey: null; //当前显示的搜索结果的名称关键字

    clip: true;
    color: uiConfig.color("details");
    radius: uiConfig.radius("details");
    
    DetailsUtils {
        id: detailsUtils;
    }

    Flickable {
        id: scrollerArea; //主体滚动区域

        anchors.fill: parent;
        anchors.leftMargin: uiConfig.margin("details", 0);
        anchors.topMargin: uiConfig.margin("details", 1);
        anchors.rightMargin: uiConfig.margin("details", 2);
        anchors.bottomMargin: uiConfig.margin("details", 3);

        contentHeight: loader.height;

        Loader {
            id: loader;

            width: parent.width;
            height: loader.item.height;

            onLoaded: {
                loader.item.update(root.currentData);
            }
        }
    }

    function getJsonObj(jsonStr) {
        if (jsonStr === undefined || jsonStr === "") {
            return undefined;
        }

        try {
            var obj = JSON.parse(jsonStr);
            return obj;
        } catch (e) {
            console.log("处理json字符串失败，请检查数据");
        }

        return undefined;
    }

    function parseUI() {
        var source = "";
        var pluginObj = getJsonObj(detailsUtils.getPluginDesc(pluginId)); //获取每个插件的详情页描述文件

        if (pluginObj === undefined) {
            console.log("use defaule page; ", pluginId);
            source = "qrc:/qml/DefaultDetailsPage.qml";

        } else {
            var mode = pluginObj.mode;

            if (mode.includes("all")) {
                // TODO 修改plugin.json,简单判断从写的类型
                source = pluginObj.modules["all"];

            } else {
                //部分重写
                source = undefined;
            }
        }

        console.log("==初始化界面==", loader.source, source);
        //
        if (source !== undefined) {
            if (source === loader.source) {
                //直接加载数据
                loader.item.update(root.currentData);

            } else {
                loader.setSource(source);
            }

        } else {
            //部分加载
            if (loader.source === "qrc:/qml/DefaultDetailsPage.qml") {
                //手动加载数据
                loader.item.update(root.currentData);

            } else {
                loader.setSource("qrc:/qml/DefaultDetailsPage.qml");
            }
        }
    }

    function loadData(pluginId, dataIndex) {
        if (!loader.visible) {
            return false;
        }

        var item = detailsUtils.getResultData(pluginId, dataIndex, "description");
        var itemObj = getJsonObj(item);
        if (itemObj === undefined) {
            return false;
        }

        var action = detailsUtils.getPluginActions(pluginId, itemObj.type);
        var actionObj = getJsonObj(action);
        if (actionObj === undefined) {
            return false;
        }

        /*
         * 详情页数据对象(datails)的结构如下：
            {
                icon: null,
                name: "",
                type: "",
                descriptions: [],
                actions: []
            }
        */

        var detailedData = {}; //发送给详情页的详细信息数据

        detailedData.icon = detailsUtils.getResultData(pluginId, dataIndex, "icon");
        detailedData.name = detailsUtils.getResultData(pluginId, dataIndex, "name");
        detailedData.type = detailsUtils.getPluginData(pluginId, "name");

        detailedData.descriptions = [...itemObj.keys]; //深拷贝数组(ES6)
        detailedData.actions = [...actionObj.keys];

        root.currentData = detailedData;
        root.currentType = itemObj.type;
        root.currentActionKey = itemObj.actionKey;

        return true;
    }

    function loadUI(pluginId, dataIndex) {
        //先加载数据，再刷新界面
        if (loadData(pluginId, dataIndex)) {
            root.pluginId = pluginId;
            root.dataIndex = dataIndex;

            parseUI();
        }
    }

    // actionId: action的id号
    function openAction(actionId) {
        if (root.currentActionKey === null || root.currentType === null) {
            console.log("error: actionkey is null or type is null");
            return;
        }
        console.log("requestOpenAction:", actionId, root.currentActionKey, root.currentType);

        detailsUtils.openAction(root.pluginId, actionId, root.currentActionKey, root.currentType);
    }

    Component.onCompleted: {

    }
}
