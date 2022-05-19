import QtQuick 2.0

Rectangle {
    id: searchResultPage
    width: 681
    height: 532
    property string keyword: ""

    onKeywordChanged: keywordChange()
//    radius: 16
    color: "transparent"
    SearchResultList {
        id: searchResultList
        keyword: searchResultPage.keyword
    }

    function keywordChange() {
        console.log(keyword)
        heightAnima.start();
        searchResultList.keyword = searchResultPage.keyword
    }
    PropertyAnimation on height{
        id: heightAnima;
        target: searchResultPage;
        property: "height";
        easing.type: Easing.InQuint
        from: 0;
        to: 532;
        duration: 100
    }


}
