#include <iostream>
#include <QJsonDocument>
#include <QJsonObject>
#include "TankClient.h"

TankClient::TankClient( QObject *parent ) :
    QObject( parent ),
    m_nextBlockSize( 0 ),
    m_x( 0 ),
    m_y( 0 )
{
}

void TankClient::setSocket( int handle )
{
    m_socket = new QTcpSocket( this );

    m_socket->setSocketDescriptor( handle );
    m_handle = handle;

    connect( m_socket, SIGNAL( disconnected() ),
             this, SLOT( slotDisconnected() ) );
    connect( m_socket, SIGNAL( readyRead() ),
             this, SLOT( slotReadyRead() ) );

    // Send a handle
    QJsonObject id;
    id["name"] = QString( "id" );
    id["id"] = m_handle;
    QJsonDocument doc( id );
    sendDocument( doc );

    // Report
    std::cout << "Client with id = " << m_handle << " was connected" << std::endl;
}

void TankClient::sendDocument( const QJsonDocument &doc )
{
    QByteArray  arrBlock;
    QDataStream out( &arrBlock, QIODevice::WriteOnly );
    out.setVersion( QDataStream::Qt_5_3 );

    out << quint16( 0 ) << doc.toBinaryData();

    // Write a block size
    out.device()->seek( 0 );
    out << quint16( arrBlock.size() - sizeof( quint16 ) );

    // Send data
    m_socket->write( arrBlock );
}

void TankClient::setXY( int x, int y )
{
    m_x = x;
    m_y = y;
}

int TankClient::x() const
{
    return m_x;
}

int TankClient::y() const
{
    return m_y;
}

void TankClient::setId( int id )
{
    m_handle = id;
}

int TankClient::id() const
{
    return m_handle;
}

void TankClient::slotDisconnected()
{
    emit signalErase( m_handle );
    std::cout << "Client with id = " << m_handle << " was disconnected" << std::endl;
}

void TankClient::slotReadyRead()
{
    QDataStream in( m_socket );
    in.setVersion( QDataStream::Qt_5_3 );
    for ( ;; ) {
        if ( !m_nextBlockSize ) {
            if ( m_socket->bytesAvailable() < ( int )sizeof( quint16 ) ) {
                break;
            }
            in >> m_nextBlockSize;
        }

        if ( m_socket->bytesAvailable() < m_nextBlockSize ) {
            break;
        }

        QJsonDocument doc;
        QByteArray data;
        in >> data;
        doc = doc.fromBinaryData( data );

        QJsonObject obj = doc.object();

        m_x = obj["x"].toInt();
        m_y = obj["y"].toInt();
        m_direction = obj["direction"].toInt();

        emit signalCoordinatesWasChanged( doc );

        m_nextBlockSize = 0;
    }

    // parseData
}
