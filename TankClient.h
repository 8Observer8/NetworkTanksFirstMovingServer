#ifndef TANKCLIENT_H
#define TANKCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonDocument>

class TankClient : public QObject
{
    Q_OBJECT

public:
    explicit TankClient( QObject *parent = 0 );
    void setSocket( int handle );
    void sendDocument( const QJsonDocument &doc );
    void setXY( int x, int y );
    int x() const;
    int y() const;
    void setId( int id );
    int id() const;

signals:
    void signalCoordinatesWasChanged( const QJsonDocument &doc );
    void signalErase( int handle );

private slots:
    void slotDisconnected();
    void slotReadyRead();

private:
    QTcpSocket *m_socket;
    quint16 m_nextBlockSize;
    int m_handle;
    int m_x;
    int m_y;
    int m_direction;
};

#endif // TANKCLIENT_H
