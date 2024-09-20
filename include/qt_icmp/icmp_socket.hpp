#ifndef ICMP_SOCKET_HPP
#define ICMP_SOCKET_HPP

#include <QString>
#include <QObject>
#include <QMutex>
#include <QWaitCondition>

class ICMPSocket : public QObject {
    Q_OBJECT
public:
    ICMPSocket();
    ~ICMPSocket();

    // Sends an ICMP Echo request (ping) to the specified IP address
    void sendPing(const QString& ipAddress, quint16 id, quint16 sequence);

public slots:
    void listenForPackets();
    void stopListening();

signals:
    void pongReceived(const QString& ipAddress);

private:
    int sockfd_;

private:
    bool stopRequested;
    QMutex mutex;
};

#endif // ICMP_SOCKET_HPP
