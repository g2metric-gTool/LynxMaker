pragma Singleton

import QtQuick
import QtQuick.Controls

FluObject{

    property var navigationView
    property var paneItemMenu
    
    FluPaneItem{
        id:item_home
        title: qsTr("Home")
        menuDelegate: paneItemMenu
        icon: FluentIcons.Home
        url: "qrc:/qml/pages/P_Home.qml"
        onTap: {
            if(navigationView.getCurrentUrl()){
                item_home.count = 0
            }
            navigationView.push(url)
        }
    }
}
