#ifndef TANKSERVER_H
#define TANKSERVER_H

#include <map>
#include <QTcpServer>
#include <QJsonDocument>
#include "TankClient.h"

class TankServer : public QTcpServer
{
    Q_OBJECT

public:
    explicit TankServer( QObject *parent = 0 );
    void startServer();

private slots:
    void slotSendInfoAnotherClients( const QJsonDocument &doc );
    void slotEraseClient( int handle );

protected:
    void incomingConnection( int handle );

private:
    std::map<int, TankClient*> m_clients;
    int m_nextInitX;
    int m_nextInitY;
};

#endif // TANKSERVER_H
