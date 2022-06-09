import QtQuick 2.0
import QtQuick.Layouts 1.12

Rectangle {
    id: root;

    radius: 8;
    color: "#74F3F1";

    property string pluginId;
    property int dataIndex : -1;

    ColumnLayout {
        id: layot;

        anchors.fill: parent;

        spacing: 5;

        Rectangle {
            Layout.fillWidth: true;
            Layout.preferredHeight: 200;
        }

        Rectangle {
            Layout.fillWidth: true;
            Layout.preferredHeight: 1;
            Layout.leftMargin: 25;
            Layout.rightMargin: 25;

            color: "#DDDDDD";
        }

        Rectangle {
            Layout.fillWidth: true;
            Layout.preferredHeight: 100;
        }

        Rectangle {
            Layout.fillWidth: true;
            Layout.preferredHeight: 1;
            Layout.leftMargin: 25;
            Layout.rightMargin: 25;

            color: "#DDDDDD";
        }

        Rectangle {
            Layout.fillWidth: true;
            Layout.preferredHeight: 100;
        }
    }

    //预览图
    Component {
        id: image
        Rectangle {
            height: 50;
            width: 50;
        }
    }

    //数据名称
    Component {
        id: info
        Rectangle {
            height: 50;
            width: 50;
        }
    }

    //详细数据
    Component {
        id: data
        Rectangle {
            height: 50;
            width: 50;
        }
    }

    //动作
    Component {
        id: action
        Rectangle {
            height: 50;
            width: 50;
        }
    }

    //分割线
    Component {
        id: dividingLine
        Rectangle {
            height: 50;
            width: 50;
        }
    }

    onPluginIdChanged: {
        console.log("插件内容变化!");
    }

    onDataIndexChanged: {
        console.log("加载数据:", dataIndex);
        if ((dataIndex % 2) === 0) {
            color = "#CCFFFF";
        } else {
            color = "#CC99FF";
        }
    }

    function loadUI() {
        var ui = '{
            "name": "ukui-search-plugin",
            "version": "1.0.0",
            "components": {
              "image": {
                "enable": true

              },
              "info": {
                "enable": true,
                "keys": [
                  "name"
                ]
              },
              "data": {
                "enable": true,
                "keys": [
                  "path",
                  "name"
                ]
              },
              "action": {
                "enable": true
              }
            }
          }';

        var uiObj = JSON.parse(ui);

        console.log(uiObj, "=", uiObj.components.info.enable, uiObj.components.data.keys)

    }

    Component.onCompleted: {
        loadUI();
    }
}
