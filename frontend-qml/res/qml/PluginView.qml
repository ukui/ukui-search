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

        PluginList {
            //PluginList和PluginView是一对一配对使用的
            id: pluginContainer;

            Layout.fillWidth: true;
            Layout.fillHeight: true;

            scrollerBar: scrollBar;

            onPluginItemSelected: function(pluginId, index) {
                root.showItemDetails(pluginId, index);
            }
        }

        ScrollBar {
            id: scrollBar;

            Layout.preferredWidth: uiConfig.width("scrollBar");
            Layout.fillHeight: true;

            policy: ScrollBar.AsNeeded;
        }
    }
}
