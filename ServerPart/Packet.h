#ifndef PACKET_H
#define PACKET_H

#include <QRgb>
#include <cstdint>

/**
\brief Struct contains information for image initialization
*/
struct InitialPacket
{
    std::uint16_t height,///< Image height
	width;/// < Image width
};

/**
\brief Struct contains information of image pixel
*/
struct Packet
{
    //Packet contains coordinate of pixel and it`s color
    std::uint16_t xCoord,///< pixel position on the x-axis
	yCoord;///< pixel position on the y-axis
    QRgb rgba;///< pixel color + transparency
};

#endif // PACKET_H
