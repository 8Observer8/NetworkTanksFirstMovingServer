#include <iostream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "TankServer.h"
#include "TankClient.h"

TankServer::TankServer( QObject *parent ) :
    QTcpServer( parent ),
    m_nextInitX( -436 ),
    m_nextInitY( -436 )
{
}

void TankServer::startServer()
{
    int portName = 12345;
    if ( listen( QHostAddress::Any, portName ) ) {
        std::cout << "The server is running on" << std::endl;
        std::cout << "Port Name: " << portName << std::endl;
    } else {
        std::cout << "Error: unable to start the server" << std::endl;
    }
}

void TankServer::slotSendInfoAnotherClients( const QJsonDocument &doc )
{
    for ( auto &p : m_clients ) {
        p.second->sendDocument( doc );
    }
}

void TankServer::slotEraseClient( int handle )
{
    m_clients.erase( handle );

    // Send command to delete a tank
    QJsonObject tankObj;
    tankObj["name"] = QString( "DeleteTank" );
    tankObj["id"] = handle;
    QJsonDocument doc;
    doc.setObject( tankObj );
    for( auto &client : m_clients ) {
        client.second->sendDocument( doc );
    }
}

void TankServer::incomingConnection( int handle )
{
    TankClient *client = new TankClient( this );
    client->setSocket( handle );

    m_clients[handle] = client;
    m_clients[handle]->setXY( m_nextInitX, m_nextInitY );

    connect( m_clients[handle], SIGNAL( signalErase( int ) ),
             this, SLOT( slotEraseClient( int ) ) );
    connect( m_clients[handle], SIGNAL( signalCoordinatesWasChanged( QJsonDocument ) ),
             this, SLOT( slotSendInfoAnotherClients( QJsonDocument ) ) );

    // Send init position
    QJsonObject initPositionObj;
    initPositionObj["name"] = QString("InitPosition");
    initPositionObj["x"] = m_nextInitX;
    initPositionObj["y"] = m_nextInitY;
    QJsonDocument initPositionDoc;
    initPositionDoc.setObject( initPositionObj );
    m_clients[handle]->sendDocument( initPositionDoc );
    m_nextInitX += 64;

    // Send coordinates of a new tank
    QJsonObject newTankObj;
    newTankObj["name"] = QString( "AddNewTank" );
    newTankObj["id"] = handle;
    newTankObj["x"] = m_clients[handle]->x();
    newTankObj["y"] = m_clients[handle]->y();
    QJsonDocument doc;
    doc.setObject( newTankObj );
    for( auto &client : m_clients ) {
        client.second->sendDocument( doc );
    }

    // Send coordinates of all tanks
    QJsonObject allTanksObj;
    allTanksObj["name"] = QString( "AllTanks" );
    QJsonArray tankArray;
    for( auto &client : m_clients ) {
        QJsonObject tankObject;
        tankObject["id"] = client.second->id();
        tankObject["x"] = client.second->x();
        tankObject["y"] = client.second->y();
        tankArray.append( tankObject );
    }
    allTanksObj["tanks"] = tankArray;
    QJsonDocument allTanksDocument;
    allTanksDocument.setObject( allTanksObj );
    m_clients[handle]->sendDocument( allTanksDocument );
}
