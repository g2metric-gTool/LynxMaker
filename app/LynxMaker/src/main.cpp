#include "version.h"
#include "application.h"

#include <windows.h>

void enableAnsiSupport() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hConsole, &mode);
    SetConsoleMode(hConsole, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}

int main(int argc, char* argv[])
{
    enableAnsiSupport();

    Application app(argc, argv);
    if (app.qmlEngine()->rootObjects().isEmpty())
        return 1;
    return app.run();
}