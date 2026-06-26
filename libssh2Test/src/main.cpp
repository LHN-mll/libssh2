#include <QApplication>
#include "mainwindow.h"
#include "sshworker.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("SSH Remote File Manager");
    app.setApplicationVersion("1.0");

    // Register custom types for cross-thread queued signal/slot
    qRegisterMetaType<RemoteFileInfo>("RemoteFileInfo");
    qRegisterMetaType<QList<RemoteFileInfo>>("QList<RemoteFileInfo");

    MainWindow w;
    w.resize(960, 640);
    w.show();

    return app.exec();
}
