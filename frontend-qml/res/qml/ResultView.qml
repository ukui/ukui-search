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

        ResultList {
            id: resultList;

            Layout.preferredWidth: showDetails ? Math.round(parent.width * 0.382) : parent.width;
            Layout.fillHeight: true;

            onShowItemDetails: (pluginId, index) => {
                root.expandDetails(pluginId, index);
            }
        }


        Details {
            id: details;

            Layout.fillWidth: true;
            Layout.fillHeight: true;

            radius: 5;
        }
    }

    function expandDetails(pluginId, index) {
        showDetails = true;

        details.loadUI(pluginId, index);
    }
}
