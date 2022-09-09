import QtQuick 2.6
import QtQuick.Window 2.2
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import org.ukui.appwidget 1.0
import org.ukui.qqc2style.private 1.0 as StylePrivate

AppWidget {
    width: 640
    height: 100
    visible: true
    id: window
    appname: "search"
    username: parent.username

    function parseJson(jsonStr) {
        try {
            var obj = JSON.parse(jsonStr);
            return obj;
        } catch(e) {
            console.log("json解析失败");
        }
        return undefined;
    }

    function updateSearchBarColor() {
        var data = window.parseJson(window.datevalue);
        searchBar.color = Qt.rgba(data.red, data.green, data.blue, data.alpha);
        placeholderText.color = data.placeHolderTextColor;
    }

    onDatevalueChanged: updateSearchBarColor();

    Rectangle {
        id:searchBar
        anchors.centerIn: parent
        width:448
        height: 64
        radius: searchBar.height / 2

        Component.onCompleted: {
            window.updateSearchBarColor();
        }

        RowLayout {
            id: layout
            anchors.fill: parent
            spacing: 0

            Image {
                id: image_search
                Layout.leftMargin: 16
                Layout.rightMargin: 16
                Layout.preferredWidth: 32;
                Layout.preferredHeight: 32;

                Layout.alignment: Qt.AlignVCenter
                source: "file:///usr/share/appwidget/search/ukui-search.svg"
            }

            Item {
                clip: true;
                Layout.fillWidth: true
                Layout.fillHeight: true;
                Layout.rightMargin: 32;

                TextInput {
                    id: searchtext
                    anchors.fill: parent;

                    Keys.enabled: true
                    Keys.onEnterPressed: {
                        window.qmldefineeventchangedsignal("enter", "search", searchtext.text);
                        searchtext.remove(0, searchtext.length);
                    }
                    Keys.onReturnPressed: {
                        window.qmldefineeventchangedsignal("return", "search", searchtext.text);
                        searchtext.remove(0, searchtext.length);
                    }

                    focus: true
                    color: StylePrivate.StyleHelper.windowtextcolorrole
                    maximumLength: 100
                    selectByMouse: true
                    verticalAlignment: Qt.AlignVCenter
                    font.pixelSize: 21

                    property string placeholderText: /*qsTr("search")*/"全局搜索"

                    Text {
                        id: placeholderText
                        text: searchtext.placeholderText
                        font.pixelSize: 21

                        visible: !searchtext.text
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
            }
        }
    }
}


