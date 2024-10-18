import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import "global"
FluLauncher {
    id: app
    Connections{
        target: FluTheme
        function onDarkModeChanged(){
            SettingsHelper.setDarkMode(FluTheme.darkMode)
        }
        function onAccentColorChanged(){
            SettingsHelper.setAccentColor(FluTheme.accentColor.normal)
        }
        function onAnimationEnabledChanged(){
            SettingsHelper.setAnimationEnabled(FluTheme.animationEnabled)
        }
    }
    Connections{
        target: FluApp
        function onUseSystemAppBarChanged(){
            SettingsHelper.setUseSystemAppBar(FluApp.useSystemAppBar)
        }
    }
    Component.onCompleted: {
        FluApp.init(app,Qt.locale(SettingsHelper.language()))
        FluApp.windowIcon = "qrc:/res/images/commun/x.png"
        FluApp.useSystemAppBar = SettingsHelper.useSystemAppBar()
        FluTheme.darkMode = SettingsHelper.darkMode()
        FluTheme.animationEnabled = SettingsHelper.animationEnabled()
        FluTheme.accentColor = FluColors.createAccentColor(SettingsHelper.accentColor())
        FluRouter.routes = {
            "/":"qrc:/qml/window/MainWindow.qml",
            "/crash":"qrc:/qml/window/CrashWindow.qml"
        }
        var args = Qt.application.arguments
        if(args.length>=2 && args[1].startsWith("-crashed=")){
            FluRouter.navigate("/crash",{crashFilePath:args[1].replace("-crashed=","")})
        }else{
            FluRouter.navigate("/")
        }
    }
}