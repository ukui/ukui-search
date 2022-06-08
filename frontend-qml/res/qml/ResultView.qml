import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5

Item {
    id: root;
    focus: false;
    property bool showDetails : false;

    RowLayout {
        id: mainLayout;
        anchors.fill: parent;
        anchors.margins: 5;
        spacing: 5;

        ListView {
            id: listView;

            Layout.preferredWidth: showDetails ? Math.round(parent.width * 0.382) : parent.width;
            Layout.fillHeight: true;

            spacing: 5;
            clip: true;

//            contentHeight: childrenRect.height;
            ScrollBar.vertical: ScrollBar {
                policy: ScrollBar.AsNeeded;
            }

            model: pluginModel;
            delegate: Plugin {
                width: listView.width;

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

            onContentHeightChanged: {
                console.log("主界面内容高度变化:", contentHeight, listView.height)
            }
        }

        Details {
            id: details;

            Layout.fillWidth: true;
            Layout.fillHeight: true;

            radius: 8;
            color: "#74F3F1";
        }
    }

    onWidthChanged: {
        console.log("输出:", listView.width, details.width)
    }

    Component.onCompleted: {
        //listView.width = showDetails ? Math.round(parent.width * 0.382) : parent.width;
    }

    function showItemDetails(pluginId, index) {
        showDetails = true;
        details.pluginId = pluginId;
        details.dataIndex = index;
    }
}
