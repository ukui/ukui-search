import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5
import org.ukui.search.utils 1.0

Rectangle {
    id: root;

    property string pluginId;
    property int dataIndex: -1;
    property var itemObjSave: null;
    property var actionObjSave: null;

    clip: true;
    color: uiConfig.color("details");
    radius: uiConfig.radius("details");
    
    DetailsUtils {
        id: detailsUtils;
    }

    Column {
        id: detailsLayout;

        anchors.fill: parent;
        anchors.leftMargin: uiConfig.margin("details", 0);
        anchors.topMargin: uiConfig.margin("details", 1);
        anchors.rightMargin: uiConfig.margin("details", 2);
        anchors.bottomMargin: uiConfig.margin("details", 3);

        spacing: uiConfig.spacing("details");

        DetailsImage {
            id: image;

            width: detailsLayout.width;
            height: uiConfig.height("detailsImage");

            icon: null;
            imageWidth: image.height;
            imageHeight: image.height;
        }

        DetailsInfo {
            id: info;

            width: detailsLayout.width;
            height: 50;
        }

        Rectangle {
            id: dividingA;

            width: detailsLayout.width;
            height: 1;

            color: "#dbdbdb";
        }

        DetailsData {
            id: data;

            width: detailsLayout.width;

            spacing: detailsLayout.spacing;
        }

        Rectangle {
            id: dividingB;

            width: detailsLayout.width;
            height: 1;

            color: "#dbdbdb";
        }

        DetailsAction {
            id: actions;

            width: detailsLayout.width;

            spacing: detailsLayout.spacing;

            onActionClicked: (actionKey) => {root.openAction(actionKey)};
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
        var item = detailsUtils.getResultData(pluginId, dataIndex, "description");
        var itemObj = getJsonObj(item);
        if (itemObj === undefined) {
            return;
        }

        var action = detailsUtils.getPluginActions(pluginId, itemObj.type);
        var actionObj = getJsonObj(action);
        if (actionObj === undefined) {
            return;
        }

        image.icon = detailsUtils.getResultData(pluginId, dataIndex, "icon");

        info.name = detailsUtils.getResultData(pluginId, dataIndex, "name");
        info.type = detailsUtils.getPluginData(pluginId, "name");

        data.dataModel = itemObj.keys;
        dividingA.visible = (itemObj.keys.length > 0);

        actions.actionModel = actionObj.keys;
        dividingB.visible = (actionObj.keys.length > 0);

        root.itemObjSave = itemObj;
        root.actionObjSave = actionObj;

        console.log("load action data:", itemObj.keys);
        console.log("load action data:", actionObj.keys);
    }

    function loadUI(pluginId, dataIndex) {
        root.pluginId = pluginId;
        root.dataIndex = dataIndex;

        parseUI();
    }

    function openAction(actionKey) {
        detailsUtils.openAction(root.pluginId, actionKey, root.itemObjSave.actionKey, root.itemObjSave.type);
    }

    Component.onCompleted: {

    }
}
