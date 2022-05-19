import QtQuick 2.15
import QtQuick.Controls 2.15
import ukui.search 1.0

Rectangle {
    id: searchResultList
    width: 681
    height: children.height
//    radius: 16
    color: "transparent"
    property string keyword: ""
    Component.onCompleted: {
        debug()
    }
    function debug() {
        console.log(SearchPluginManager.getPluginIds())
    }
    ListModel {
        id: pluginIds
//        ListElement {
//           name: "Note Search"
//        }
//        ListElement {
//           name: "Applications Search"
//        }
//        ListElement {
//           name: "Settings Search"
//        }
//        ListElement {
//           name: "Dir Search"
//        }
        ListElement {
           name: "File Search"
        }
    }

    Column {
        spacing: 2

        Repeater {
            id: repeater
            model: pluginIds
            ResultListView {
//                id: modelData
                width: 681
                height: children.height
                pluginId: pluginIds.get(index).name
                Connections {
                    target: searchResultList
                    function onKeywordChanged(keyword) { startSearch(keyword) }
                }
//                Text {
////                    id: modelData
//                    text: "                                                                                                                                                 " + index
//                }
                Component.onCompleted: {
                    printId()
                }
                function printId() {
                    console.log(pluginIds.get(index).name + "---------")
                }
            }
        }

    }

}
