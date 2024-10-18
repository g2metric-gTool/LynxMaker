import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

FluWindow {

    id:window
    title: qsTr("Crash Occurred")
    width: 350
    height: 400
    fixSize: true
    showMinimize: false

    property string crashFilePath

    Component.onCompleted: {
        window.stayTop = true
    }

    onInitArgument:
        (argument)=>{
            crashFilePath = argument.crashFilePath
        }

    Image{
        width: 540/2
        height: 285/2
        anchors{
            horizontalCenter: parent.horizontalCenter
            top: parent.top
            topMargin: 40
        }
        source: "qrc:/res/images/commun/crash.png"
    }

    FluText{
        id:text_info
        anchors{
            top: parent.top
            topMargin: 240
            left: parent.left
            right: parent.right
            leftMargin: 10
            rightMargin: 10
        }
        wrapMode: Text.WrapAnywhere
        text: qsTr("Oops! An unexpected error has occurred. Please check the details in the report logs for more information, or restart the application. We apologize for any inconvenience caused")
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    RowLayout{
        anchors{
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: 20
        }
        FluButton{
            text: qsTr("Report Logs")
            onClicked: {
                FluTools.showFileInFolder(crashFilePath)
            }
        }
        FluButton{
            text: qsTr("Submit Bug")
            onClicked: {
                var mainRecipient = "Support.Lynx@g2metric.com";
                var ccRecipient = "hamdi.benabdallah@g2metric.com";
                var subject = qsTr("Crash Report") + " - %1".arg(Qt.application.name);
                
                var mailtoUrl = "mailto:"   + mainRecipient +
                                "?cc="      + ccRecipient +
                                "&subject=" + encodeURIComponent(subject);
                                //"&body="    + encodeURIComponent(body);

                Qt.openUrlExternally(mailtoUrl);
            }
        }
        Item{
            width: 30
            height: 1
        }
        FluFilledButton{
            text: qsTr("Restart Program")
            onClicked: {
                FluRouter.exit(931)
            }
        }
    }

}
