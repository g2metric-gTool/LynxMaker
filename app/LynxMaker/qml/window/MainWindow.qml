import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQml
import Qt.labs.platform
import "../global"

FluWindow {
    id:window
    title: Qt.application.name
    width: 500
    height: 400
    autoDestroy: true
    fixSize: true
    launchMode: FluWindowType.SingleTask
    flags: Qt.Window | Qt.WindowTitleHint | Qt.WindowSystemMenuHint | Qt.WindowMinMaxButtonsHint

    fitsAppBarWindows: SettingsHelper.fitsAppBarWindows()
    appBar: FluAppBar {
        width: window.width
        height: 30
        showDark: true
        darkClickListener:(button)=>handleDarkChanged(button)
        closeClickListener: ()=>{dialog_close.open()}
        z:7
    }

    onFitsAppBarWindowsChanged: {
        if(window.fitsAppBarWindows !== SettingsHelper.fitsAppBarWindows())
            SettingsHelper.setFitsAppBarWindows(window.fitsAppBarWindows)
    }

    onLazyLoad: {
        //tour.open()
    }

    Component.onDestruction: {
        FluRouter.exit()
    }

    FluContentDialog {
        id: dialog_close
        title: qsTr("Quit")
        message: qsTr("Are you sure you want to exit the program?")
        buttonFlags: FluContentDialogType.NegativeButton | FluContentDialogType.PositiveButton

        positiveText: qsTr("Quit")
        negativeText: qsTr("Cancel")
        onPositiveClicked: {
            FluRouter.exit(0)
        }
    }

    Component {
        id: nav_item_right_menu
        FluMenu{
            width: 186
            FluMenuItem {
                text: qsTr("Open in Separate Window")
                font: FluTextStyle.Caption
                onClicked: {
                    FluRouter.navigate("/pageWindow",{title:modelData.title,url:modelData.url})
                }
            }
        }
    }
    
    Item {
        id:page_front
        anchors.fill: parent
        FluNavigationView {
            id:nav_view
            width: parent.width
            height: parent.height
            z:999
            pageMode: FluNavigationViewType.NoStack
            items: ItemsOriginal
            topPadding:{
                if(window.useSystemAppBar){
                    return 0
                }
                return FluTools.isMacos() ? 20 : 0
            }
            displayMode: GlobalModel.displayMode
            logo: "qrc:/res/images/commun/x.svg"
            title:Qt.application.name
            onLogoClicked: {
            }
            Component.onCompleted: {
                ItemsOriginal.navigationView = nav_view
                ItemsOriginal.paneItemMenu = nav_item_right_menu
                window.setHitTestVisible(nav_view.buttonMenu)
                window.setHitTestVisible(nav_view.buttonBack)
                window.setHitTestVisible(nav_view.imageLogo)
                setCurrentIndex(0)
            }
        }
    }

    Component {
        id: com_reveal
        CircularReveal {
            id: reveal
            target: window.contentItem
            anchors.fill: parent
            darkToLight: FluTheme.dark
            onAnimationFinished: {
                loader_reveal.sourceComponent = undefined
            }
            onImageChanged: {
                changeDark()
            }
        }
    }

    FluLoader {
        id:loader_reveal
        anchors.fill: parent
    }

    function distance(x1,y1,x2,y2) {
        return Math.sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2))
    }

    function handleDarkChanged(button) {
        if(FluTools.isMacos() || !FluTheme.animationEnabled) {
            changeDark()
        } else {
            loader_reveal.sourceComponent = com_reveal
            var target = window.containerItem()
            var pos = button.mapToItem(target,0,0)
            var mouseX = pos.x + button.width / 2
            var mouseY = pos.y + button.height / 2
            var radius = Math.max(distance(mouseX,mouseY,0,0),distance(mouseX,mouseY,target.width,0),distance(mouseX,mouseY,0,target.height),distance(mouseX,mouseY,target.width,target.height))
            var reveal = loader_reveal.item
            reveal.start(reveal.width*Screen.devicePixelRatio,reveal.height*Screen.devicePixelRatio,Qt.point(mouseX,mouseY),radius)
        }
    }

    function changeDark() {
        if(FluTheme.dark) {
            FluTheme.darkMode = FluThemeType.Light
        } else {
            FluTheme.darkMode = FluThemeType.Dark
        }
    }

    Shortcut {
        sequence: "F1"
        context: Qt.WindowShortcut
        onActivated: {
            tour.open()
        }
    }

    FluTour{
        id: tour
        finishText: qsTr("Finish")
        nextText: qsTr("Next")
        previousText: qsTr("Previous")
        steps:{
            var data = []
            if(!window.useSystemAppBar) {
                data.push({title:qsTr("Dark Mode"),description: qsTr("Here you can switch to night mode."),target:()=>appBar.buttonDark})
            }
            //data.push({title:qsTr("Hide Easter eggs"),description: qsTr("Try a few more clicks!!"),target:()=>nav_view.imageLogo})
            return data
        }
    }

    FpsItem {
        id:fps_item
    }

    FluText {
        text: "fps %1".arg(fps_item.fps)
        opacity: 0.3
        anchors {
            bottom: parent.bottom
            right: parent.right
            bottomMargin: 5
            rightMargin: 5
        }
    }

    Shortcut {
        sequence: "Ctrl+M,Ctrl+O"
        context: Qt.WindowShortcut
        onActivated: {
            ItemsOriginal.navigationView.collapseAll()
        }
    }
}
