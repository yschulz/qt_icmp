#include "qt_icmp/icmp_socket.hpp"
#include "qt_icmp/icmp_packet.hpp"

#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>
#include <regex>
#include <QThread>
#include <QDebug>
#include <QMutex>
#include <fcntl.h> // Add this line
#include <errno.h> 

ICMPSocket::ICMPSocket() {
    sockfd_ = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd_ < 0) {
        throw std::runtime_error("Failed to create raw socket");
    }

    int flags = fcntl(sockfd_, F_GETFL, 0);
    fcntl(sockfd_, F_SETFL, flags | O_NONBLOCK);
}

ICMPSocket::~ICMPSocket() {
    if (sockfd_ >= 0) {
        close(sockfd_);
    }
}

void ICMPSocket::sendPing(const QString& ipAddress, quint16 id, quint16 sequence) {
    ICMPPacket packet(id, sequence);

    qDebug() << "sending the ping from socket";

    struct sockaddr_in dest_addr = {};
    dest_addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, ipAddress.toStdString().c_str(), &dest_addr.sin_addr) <= 0) {
        throw std::runtime_error("Invalid IP address");
    }

    if (sendto(sockfd_, packet.header(), packet.size(), 0,
               reinterpret_cast<struct sockaddr*>(&dest_addr), sizeof(dest_addr)) < 0) {
        throw std::runtime_error("Failed to send ICMP packet");
    }
}

void ICMPSocket::listenForPackets() {
    while (!stopRequested) {
        char buffer[1024];
        struct sockaddr_in sender_addr;
        socklen_t addrlen = sizeof(sender_addr);

        ssize_t length = recvfrom(sockfd_, buffer, sizeof(buffer), 0,
                                  reinterpret_cast<struct sockaddr*>(&sender_addr), &addrlen);
         
        if (length >= 0) {  // Data received
            QString receivedIpAddress = inet_ntoa(sender_addr.sin_addr);
            qDebug() << "Received packet from" << receivedIpAddress;
            emit pongReceived(receivedIpAddress);
        } else if (errno != EAGAIN && errno != EWOULDBLOCK) {
            qWarning() << "recvfrom error:" << strerror(errno);
        }

        // Check stopRequested flag periodically
        if (stopRequested) break;

        QThread::msleep(10);
    }
}

void ICMPSocket::stopListening() {
    QMutexLocker locker(&mutex);
    stopRequested = true;
    qDebug() << "Stop requested, waking up thread...";
}