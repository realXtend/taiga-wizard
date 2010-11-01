#include <QtGui/QApplication>
#include "dialog.h"
#include <QFont>
#include <QString>
#include <QDir>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString styleSheet;
    if(QDir::separator()=='/') // for checking if were in windows of linux
        styleSheet = "QLabel{font: \"SansSerif\"; font-size: 10px } QGroupBox{font: bold \"SansSerif\"; font-size: 14px  }";
    else
        styleSheet = "QLabel{font: \"SansSerif\"; font-size: 12px } QGroupBox{font: bold \"SansSerif\"; font-size: 14px  }";
    a.setStyleSheet(styleSheet);
    Dialog w;
    w.show();
    return a.exec();
}
