#include "qt_icmp/icmp_wrapper.hpp"
#include "qt_icmp/icmp_socket.hpp"

#include <QDebug>
#include <QCoreApplication>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <stdexcept>
#include <regex>

#include <QThread>
#include <QEventLoop>
#include <QTimer>

#include <QTextStream>
#include <cstdio>

ICMPWrapper::ICMPWrapper(QObject *parent) : QObject(parent) {}

// Overloaded method that accepts an integer IP address
bool ICMPWrapper::pingHost(const quint32& ipAddressInt) {
    QString ipAddress = convertIntToIPAddress(ipAddressInt);
    return pingHost(ipAddress);
}

bool ICMPWrapper::pingHost(const QString& ipAddress) {
    if (!isValidIPAddress(ipAddress)) {
        qWarning() << "Invalid IP address format:" << ipAddress;
        return false;
    }

    try {
        ICMPSocket socket;
        quint16 id = static_cast<quint16>(QCoreApplication::applicationPid());
        quint16 sequence = 1;  // Increment this for subsequent pings

        // Create an event loop and timer for timeout handling
        QEventLoop loop;
        QTimer timeoutTimer;

        // Variable to store the pong response status
        bool pongReceived = false;

        // Connect the pongReceived signal to update the status and quit the loop
        connect(&socket, &ICMPSocket::pongReceived, [&loop, &pongReceived]() {
            pongReceived = true;
            qDebug() << "Pong received";
            loop.quit();  // Quit the event loop when pong is received
        });

        // Connect the timeout timer to quit the loop on timeout
        connect(&timeoutTimer, &QTimer::timeout, [&loop]() {
            qWarning() << "Ping timeout reached";
            loop.quit();  // Quit the event loop on timeout
        });

        // Start the timeout timer
        timeoutTimer.setSingleShot(true);
        timeoutTimer.start(2000);  // Timeout after 2000 ms (2 seconds)

        // Start the packet listening in a separate thread
        QThread packetThread;
        connect(&packetThread, &QThread::started, [&socket]() {
            socket.listenForPackets();  // This method should keep running
        });

        packetThread.start();

        // Send the ping
        socket.sendPing(ipAddress, id, sequence);
        qDebug() << "Ping sent to" << ipAddress;

        // Wait for response or timeout
        loop.exec();

        // Clean up
        socket.stopListening();  // Request the thread to stop
        packetThread.quit();
        packetThread.wait();
 
        // Return the result based on pong received status
        return pongReceived;
    } catch (const std::exception& e) {
        qWarning() << "Error while pinging:" << e.what();
        return false;
    }
}

// Helper method to convert an int to an IP address string
QString ICMPWrapper::convertIntToIPAddress(quint32 ipAddressInt) {
    struct in_addr ipAddrStruct;
    ipAddrStruct.s_addr = htonl(ipAddressInt);  // Ensure correct byte order
    return QString(inet_ntoa(ipAddrStruct));    // Convert to QString
}

// Helper method to validate IP address string using regex
bool ICMPWrapper::isValidIPAddress(const QString& ipAddress) {
    const std::regex ipPattern(
        R"(^(?:[0-9]{1,3}\.){3}[0-9]{1,3}$)"
    );
    return std::regex_match(ipAddress.toStdString(), ipPattern);
}
