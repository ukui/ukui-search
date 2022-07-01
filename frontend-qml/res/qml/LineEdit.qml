import QtQuick 2.12
import QtQuick.Layouts 1.12
import org.ukui.search.items 1.0

Rectangle {
    id: root;

    signal textChanged(string keyword);

    onTextChanged: (keyword) => {
        console.log("received signal:", keyword);
    }

    color: uiConfig.color("keywordBody");

    RowLayout {
        id: lineditLayout;

        anchors.fill: parent;
        anchors.leftMargin: uiConfig.margin("keywordBody", 0);
        anchors.topMargin: uiConfig.margin("keywordBody", 1);
        anchors.rightMargin: uiConfig.margin("keywordBody", 2);
        anchors.bottomMargin: uiConfig.margin("keywordBody", 3);

        spacing: uiConfig.spacing("keywordBody");

        IconItem {
            id: keywordIcon;

            Layout.preferredWidth: uiConfig.width("keywordIcon");
            Layout.preferredHeight: uiConfig.height("keywordIcon");
            Layout.alignment: Qt.AlignVCenter;

            name: "system-search-symbolic";
        }

        TextInput {
            id: keywordInput;

            Layout.fillWidth: true;
            Layout.fillHeight: true;
            Layout.alignment: Qt.AlignVCenter;

            clip: true;
            focus: true;
            maximumLength: 100;
            selectByMouse: true;
            verticalAlignment: Qt.AlignVCenter;

            selectionColor: uiConfig.color("select");
            font.pointSize: uiConfig.height("keywordFont");

            onTextChanged: {
                console.log(keywordInput.text);
                root.textChanged(keywordInput.text);
            }
        }
    }
}
