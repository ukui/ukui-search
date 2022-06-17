import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5
import org.ukui.search.utils 1.0

Rectangle {
    id: root;

    color: "#FBFBFB";
    clip: true;

    property string pluginId;
    property int dataIndex : -1;

    property var itemObjSave: null;
    property var actionObjSave: null;

    DetailsUtils {
        id: detailsUtils;
    }

    Column {
        id: layout;

        anchors.fill: parent;
        anchors.margins: 10;

        spacing: 10;

        DetailsImage {
            id: image;

            width: layout.width;
            height: 150;

            icon: null;
            imageWidth: 150;
            imageHeight: 150;
        }

        DetailsInfo {
            id: info;

            width: layout.width;
            height: 50;
        }

        Rectangle {
            id: dividingA;

            width: layout.width;
            height: 1;

            color: "#dbdbdb";
        }

        DetailsData {
            id: data;

            width: layout.width;

            spacing: layout.spacing;
        }

        Rectangle {
            id: dividingB;

            width: layout.width;
            height: 1;

            color: "#dbdbdb";
        }

        DetailsAction {
            id: actions;

            width: layout.width;

            spacing: layout.spacing;

            onActionClicked: (actionKey) => {root.openAction(actionKey)};
        }
    }

    function getJsonObj(jsonStr) {
        console.log("处理Json字符串:", jsonStr);
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
