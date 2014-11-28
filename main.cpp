#include <QCoreApplication>
#include "TankServer.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    TankServer server;
    server.startServer();

    return app.exec();
}
