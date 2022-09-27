#ifndef PACKET_H
#define PACKET_H

#include <QRgb>
#include <cstdint>

struct InitialPacket
{
    //Initial packet with base information about image
    std::uint16_t height, width;
};

struct Packet
{
    //Packet contains coordinate of pixel and it`s color
    std::uint16_t xCoord, yCoord;
    QRgb rgba;
};

#endif // PACKET_H
