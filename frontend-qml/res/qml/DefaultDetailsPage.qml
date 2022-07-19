import QtQuick 2.0

Item {
    id: root;

    height: detailsLayout.height;

    signal requestOpenAction(int actionId);

    function update(detailedData) {
        image.icon = detailedData.icon;
        info.name = detailedData.name;
        info.type = detailedData.type;

        data.dataModel = detailedData.descriptions;
        dividingA.visible = (detailedData.descriptions.length > 0);

        actions.actionModel = detailedData.actions;
        dividingB.visible = (detailedData.actions.length > 0);
    }

    Column {
        id: detailsLayout;

        anchors.fill: parent;

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

            onActionClicked: (actionKey) => {root.requestOpenAction(actionKey)};
        }
    }
}
