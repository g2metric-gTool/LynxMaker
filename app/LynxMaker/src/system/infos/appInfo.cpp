#include <QDebug>
#include <QQmlContext>
#include <QGuiApplication>

#include "system/infos/appInfo.h"
#include "../res/version.h"

AppInfo::AppInfo(QObject *parent) : QObject(parent)
{
    version(APPLICATION_VERSION);
    appFullVersion(APPLICATION_FULL_VERSION);

    commitHash(COMMIT_HASH);
    commitAuthor(COMMIT_AUTHOR);
    commitDate(COMMIT_DATE);
    gitBranch(CURRENT_BRANCH);

    compilationAuthor(COMPILATION_AUTHOR);
    compilationDate(COMPILATION_DATE);
    complierName(COMPILER_NAME);
}
