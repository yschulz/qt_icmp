#ifndef ICMP_PACKET_HPP
#define ICMP_PACKET_HPP

#include <netinet/ip_icmp.h>
#include <QtCore/QtGlobal>

class ICMPPacket {
public:
    ICMPPacket(quint16 id, quint16 sequence);

    const struct icmphdr* header() const;
    size_t size() const;

private:
    struct icmphdr icmp_hdr_;

    // Calculates the checksum for the ICMP packet
    unsigned short calculateChecksum();
};

#endif // ICMP_PACKET_HPP
