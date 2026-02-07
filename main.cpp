#include "mainwindow.h"

#include <QApplication>

#ifdef _WIN32
#include <windows.h>
#endif

int main(int argc, char *argv[])
{
#ifdef _WIN32
    // Если приложение запущено из консоли — включаем UTF-8 для корректного вывода в std::cout
    if (GetConsoleWindow() != nullptr) {
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    }
#endif

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
