import QtQuick 2.0

Item {
    id: root;
    signal textChanged(string keyword);

    onTextChanged: (keyword) => {
        console.log("received signal:", keyword);
    }

    Rectangle {
        id: container;
        anchors.fill: parent;
        anchors.margins: 5;
        color: "transparent";

        TextInput {
            id: keywordInput;
            font.pixelSize: 22;
            anchors.fill: parent;
            verticalAlignment: TextInput.AlignVCenter;
            //text: "";
            clip: true;
            selectByMouse: true;
            focus: true;
            maximumLength: 100;

            onTextChanged: {
                console.log(keywordInput.text);
                root.textChanged(keywordInput.text);
            }
        }
    }
}
