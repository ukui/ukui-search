import QtQuick 2.0
import QtQuick.Controls 2.15

Rectangle {
    id: searchLineEdit
    width: 681
    height: 50
//    radius: 16
    color: "transparent"


    property alias text: textEdit.text
    property alias input: textEdit
    signal textChange(string text)

    TextEdit {
        id: textEdit
        width: 681
        height: 50
        anchors.fill: parent
        textMargin: 14
        focus: true
        onTextChanged:textChangeSlot()
//        color: "transparent"

    }
    function textChangeSlot() {
        searchLineEdit.textChange(textEdit.text)
    }
}

