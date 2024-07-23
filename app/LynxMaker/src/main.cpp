// QT
#include <QProcess>
#include <QQmlContext>
#include <QQuickWindow>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml/qqmlextensionplugin.h>

// LynxHorizon
#include <xUI.h>
#include "version.h"
#include "component/fpsItem.h"
#include "component/circularReveal.h"

// gLib
#include "log/gLog.h"
#include "log/listener/gStdStreamLog.h"

const QString company = COMPANY_NAME;
const QString appname = APPLICATION_NAME;
const QString version = APPLICATION_VERSION;
const QString domaine = ORGANIZATION_DOMAINE;

const int major = 1;
const int minor = 0;

// Function prototypes
void initializeQmlContext(QQmlApplicationEngine& engine, const char* uri);

int main(int argc, char* argv[])
{
	QGuiApplication app(argc, argv);

	//gLog::listener::CoutLog coutLog;
	//gLog::LogManager::instance().addLogListener(coutLog);

#ifdef WIN32
	qputenv("QT_QPA_PLATFORM", "windows:darkmode=2");
#endif
	qputenv("DESIGN_KIT_PREFIX", "Flu");

	QGuiApplication::setApplicationName(appname);
	QGuiApplication::setOrganizationName(company);
	QGuiApplication::setOrganizationDomain(domaine);
	QGuiApplication::setApplicationVersion(version);
	QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);

	QQmlApplicationEngine engine;

	const char* uri = APPLICATION_NAME;

	initializeQmlContext(engine, uri);

	FluentUI::getInstance()->registerTypes("XFluUi");

	const QUrl url(QStringLiteral("qrc:/qml/app.qml"));
	QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app, [url](QObject* obj, const QUrl& objUrl)
		{
			if (!obj && url == objUrl)
				QCoreApplication::exit(-1);
		}, Qt::QueuedConnection);
	engine.load(url);
	const int exec = QGuiApplication::exec();
	if (exec == 931) {
		QProcess::startDetached(qApp->applicationFilePath(), qApp->arguments());
	}
	return exec;
}

void initializeQmlContext(QQmlApplicationEngine& engine, const char* uri)
{
	qmlRegisterType<FpsItem>(uri, major, minor, "FpsItem");
	qmlRegisterType<CircularReveal>(uri, major, minor, "CircularReveal");
}
