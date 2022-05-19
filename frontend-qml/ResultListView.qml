import QtQuick 2.0
import QtQuick.Controls 2.15
//import ukui.search 1.0

Rectangle {
    id: viewRect
    width: 681
    height: children.height
    property string pluginId: ""
    Component {
        id: delegate
        Rectangle {
            width: viewRect.width
            height: 50
            Text {
                id: name
                text: displayName
            }
        }
    }

    ListView {
        id: searchResultView
        property string pluginId: viewRect.pluginId
//        SearchResultModel {
//            id: searchResultModel
//            Component.onCompleted: {
//                searchResultModel.pluginId = searchResultView.pluginId
//            }
//        }
        required model
//        model: SearchResultModel
        anchors.top: parent.top
        width: parent.width
//        model: animalsModel
//        delegate: delegate
        delegate: Text {
            required property string displayName
           text: displayName
        }

        function startSearch(string) {
            searchResultModel.startSearch(string)
        }
    }
    function startSearch(string) {
        searchResultView.startSearch(string)
    }
}
