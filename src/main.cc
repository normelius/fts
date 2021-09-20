

#include <QApplication>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QFontDatabase>
#include "src/mainwindow.h"


int main(int argc, char *argv[]) {

    QApplication app(argc, argv);

    // Add custom fonts
    int id = QFontDatabase::addApplicationFont(":/src/fonts/Roboto/Roboto-Regular.ttf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    QFont monospace(family);

    /*
    QStringList families = QFontDatabase().families();
    for (auto family : families)
        qDebug() << family;
    */

    //QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    app.setFont(family);
    MainWindow window;
    window.setWindowState(Qt::WindowMaximized);
    window.show();
    return app.exec();
}