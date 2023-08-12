#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>


int main(int argc, char *argv[])
{



//
//    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, last);
//    if (hProcess != NULL) {
//        std::cout<<"kill"<<std::endl;
//        TerminateProcess(hProcess, 0);
//        std::cout<<"kill2"<<std::endl;
//        CloseHandle(hProcess);
//    }

    // Закриття об'єкту процесів


    //std::cout<<"count process: "<<count<<std::endl;

    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "taskmanager_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    MainWindow w;
    w.show();
    return a.exec();

}
