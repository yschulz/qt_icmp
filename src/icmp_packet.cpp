#include "qt_icmp/icmp_packet.hpp"

#include <cstring>

ICMPPacket::ICMPPacket(quint16 id, quint16 sequence) {
    memset(&icmp_hdr_, 0, sizeof(icmp_hdr_));
    icmp_hdr_.type = ICMP_ECHO;
    icmp_hdr_.un.echo.id = id;
    icmp_hdr_.un.echo.sequence = sequence;
    icmp_hdr_.checksum = calculateChecksum();
}

const struct icmphdr* ICMPPacket::header() const {
    return &icmp_hdr_;
}

size_t ICMPPacket::size() const {
    return sizeof(icmp_hdr_);
}

unsigned short ICMPPacket::calculateChecksum() {
    unsigned short *buf = reinterpret_cast<unsigned short*>(&icmp_hdr_);
    unsigned int sum = 0;
    for (int len = sizeof(icmp_hdr_); len > 1; len -= 2) {
        sum += *buf++;
    }
    if (sizeof(icmp_hdr_) % 2 == 1) {
        sum += *(reinterpret_cast<unsigned char*>(buf));
    }
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    return static_cast<unsigned short>(~sum);
}
