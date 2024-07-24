import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import LynxMaker

FluWindow {
    id:window
    title: Qt.application.name
    width: 500
    height: 400
    minimumWidth: 350
    minimumHeight: 350
    autoVisible: true
    launchMode: FluWindowType.SingleTask
    fitsAppBarWindows: true

    appBar: FluAppBar {
        width: window.width
        height: 30
        showDark: true
        darkClickListener:(button)=>handleDarkChanged(button)
        closeClickListener: ()=>{window.close()}
        z:7
    }

    FluText{
        id: cameraInfo
        anchors {top: parent.top; left: parent.left; right: parent.horizontalCenter; topMargin: 50; leftMargin: 25; rightMargin:15}
        text:"Camera Info"
    }

    FluText{
        id: promotionInfo
        anchors {top: parent.top; left: parent.horizontalCenter; right: parent.right; topMargin: 50; rightMargin:25; leftMargin: 15}
        text: "Promotion Info"
    }

    FluComboBox{
        id: camera
        anchors {top: cameraInfo.bottom; left: parent.left; right: parent.horizontalCenter; topMargin: 10; leftMargin: 25; rightMargin:15}
        model: LynxConfigure.cameraList
        onCurrentTextChanged:(deviceInfo.text = LynxConfigure.configInfo(currentText))
    }
    FluComboBox{
        id: promotion
        anchors {top: promotionInfo.bottom; left: parent.horizontalCenter; right: parent.right; topMargin: 10; rightMargin:25; leftMargin: 15}
        model: LynxConfigure.promotionList
        onCurrentTextChanged:(promotionUid.text = "Current promotion: " + LynxConfigure.promotionUid(currentText))
    }

    FluText{
        anchors {bottom: serialNumber.top; horizontalCenter: parent.horizontalCenter}
        text: "SERIAL NUMBER"
    }
    FluTextBox{
        id: serialNumber
        anchors {top: promotion.bottom; horizontalCenter: parent.horizontalCenter; topMargin: 50}
    }

    FluButton{
        text:"Configure"
        enabled: (serialNumber.text !== "" && promotion.currentText !== "" && camera.currentText !== "")
        anchors {top: serialNumber.bottom; left: parent.left; right: parent.horizontalCenter; topMargin: 50; leftMargin: 25; rightMargin:15}
        onClicked: {
            showSuccess("Configure")
            LynxConfigure.configure(camera.currentText, promotion.currentText, serialNumber.text)
        }
    }

    FluButton{
        id: btnRefresh
        text:"Refresh"
        anchors {top: serialNumber.bottom; left: parent.horizontalCenter; right: parent.right; topMargin: 50; rightMargin:25; leftMargin: 15}
        onClicked: {
            showSuccess("Refresh")
            LynxConfigure.updateList();
            deviceInfo.text = LynxConfigure.configInfo(camera.currentText)
        }
    }

    FluText{
        id: deviceInfo
        anchors {top: btnRefresh.bottom ; horizontalCenter: parent.horizontalCenter; topMargin: 30}
        text: ""
        color: "green"
    }

    
    FluText{
        id: promotionUid
        anchors {top: deviceInfo.bottom; horizontalCenter: parent.horizontalCenter; leftMargin: 15}
        text:""
    }

    Component{
        id: com_reveal
        CircularReveal{
            id: reveal
            target: window.contentItem
            anchors.fill: parent
            onAnimationFinished:{
                loader_reveal.sourceComponent = undefined
            }
            onImageChanged: {
                changeDark()
            }
        }
    }

    FluLoader{
        id:loader_reveal
        anchors.fill: parent
    }

    function distance(x1,y1,x2,y2){
        return Math.sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2))
    }

    function handleDarkChanged(button){
        if(!FluTheme.animationEnabled || window.fitsAppBarWindows === false){
            changeDark()
        }else{
            if(loader_reveal.sourceComponent){
                return
            }
            loader_reveal.sourceComponent = com_reveal
            var target = window.contentItem
            var pos = button.mapToItem(target,0,0)
            var mouseX = pos.x
            var mouseY = pos.y
            var radius = Math.max(distance(mouseX,mouseY,0,0),distance(mouseX,mouseY,target.width,0),distance(mouseX,mouseY,0,target.height),distance(mouseX,mouseY,target.width,target.height))
            var reveal = loader_reveal.item
            reveal.start(reveal.width*Screen.devicePixelRatio,reveal.height*Screen.devicePixelRatio,Qt.point(mouseX,mouseY),radius)
        }
    }

    function changeDark(){
        if(FluTheme.dark){
            FluTheme.darkMode = FluThemeType.Light
        }else{
            FluTheme.darkMode = FluThemeType.Dark
        }
    }

    FpsItem{
        id:fps_item
    }

    FluText{
        text: "fps %1".arg(fps_item.fps)
        opacity: 0.3
        anchors{
            bottom: parent.bottom
            right: parent.right
            bottomMargin: 5
            rightMargin: 5
        }
    }

}
