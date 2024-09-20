#ifndef ICMP_WRAPPER_HPP
#define ICMP_WRAPPER_HPP

#include <QObject>
#include <QString>
#include <QFuture>
#include <QFutureWatcher>

class ICMPWrapper : public QObject {
    Q_OBJECT
public:
    explicit ICMPWrapper(QObject *parent = nullptr);

    // Sends a ping to the specified IP address
    bool pingHost(const quint32& ipAddress);
    bool pingHost(const QString& ipAddress);


private:
    // Helper method to validate IP
    bool isValidIPAddress(const QString& ipAddress);

    QString convertIntToIPAddress(quint32 ipAddressInt);

    // Asynchronous task to wait for pong response
    void handlePongResponse(const QString& ipAddress);

    // Asynchronous task to wait for pong response
    QFuture<bool> pingAsync(const QString& ipAddress);
    QFutureWatcher<bool> watcher;
};

#endif // ICMP_WRAPPER_HPP
