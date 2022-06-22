import QtQuick 2.0
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.5

Rectangle {
    id: root;

    property bool showDetails : false;

    clip: true;
    color: uiConfig.color("resultView");

    RowLayout {
        id: mainLayout;

        anchors.fill: parent;
        anchors.margins: uiConfig.margin("resultView", 0);
        spacing: uiConfig.spacing("resultView");

        PluginView {
            id: pluginView;

            Layout.preferredWidth: showDetails ? Math.round(parent.width * 0.45) : parent.width;
            Layout.fillHeight: true;

            onShowItemDetails: (pluginId, index) => {
                root.expandDetails(pluginId, index);
            }
        }


        Details {
            id: details;

            Layout.fillWidth: true;
            Layout.fillHeight: true;
        }
    }

    function expandDetails(pluginId, index) {
        showDetails = true;

        details.loadUI(pluginId, index);
    }
}
