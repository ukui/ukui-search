import QtQuick 2.0
import QtQuick.Layouts 1.12
import org.ukui.search.items 1.0

Item {
    id: root;

    property var icon;
    property string backup;
    property int imageWidth;
    property int imageHeight;

    RowLayout {
        id: layout;
        anchors.fill: parent;

        IconItem {
            id: detailsImage;

            icon: root.icon;

            Layout.preferredWidth: root.imageWidth;
            Layout.preferredHeight: root.imageHeight;

            Layout.alignment: Qt.AlignCenter;
        }
    }
}
