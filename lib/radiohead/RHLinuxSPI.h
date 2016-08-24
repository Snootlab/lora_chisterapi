// RHLinuxSPI.h
// Author: Mike McCauley (mikem@airspayce.com)
// Copyright (C) 2011 Mike McCauley
// Contributed by Joanna Rutkowska
// $Id: RHLinuxSPI.h,v 1.7 2014/04/14 08:37:11 mikem Exp $

#ifndef RHLinuxSPI_h
#define RHLinuxSPI_h

#include <RHGenericSPI.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#include <linux/types.h>
#include <linux/spi/spidev.h>

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

/////////////////////////////////////////////////////////////////////
/// \class RHLinuxSPI RHLinuxSPI.h <RHLinuxSPI.h>
/// \brief Encapsulate a hardware SPI bus interface
///
/// This concrete subclass of GenericSPIClass encapsulates the standard Arduino hardware and other
/// hardware SPI interfaces.
class RHLinuxSPI : public RHGenericSPI
{
public:
    /// Constructor
    /// Creates an instance of a hardware SPI interface, using whatever SPI hardware is available on
    /// your processor platform. On Arduino and Uno32, uses SPI. On Maple, uses HardwareSPI.
    /// \param[in] frequency One of RHGenericSPI::Frequency to select the SPI bus frequency. The frequency
    /// is mapped to the closest available bus frequency on the platform.
    /// \param[in] bitOrder Select the SPI bus bit order, one of RHGenericSPI::BitOrderMSBFirst or
    /// RHGenericSPI::BitOrderLSBFirst.
    /// \param[in] dataMode Selects the SPI bus data mode. One of RHGenericSPI::DataMode
    RHLinuxSPI(Frequency frequency = Frequency8MHz, BitOrder bitOrder = BitOrderMSBFirst, DataMode dataMode = DataMode0);

	 bool init(const char* pDevice);

    /// Transfer a single octet to and from the SPI interface
    /// \param[in] data The octet to send
    /// \return The octet read from SPI while the data octet was sent
    uint8_t transfer(uint8_t data);

	 uint8_t spiRead(uint8_t reg);
	 uint8_t spiWrite(uint8_t reg, uint8_t val);
    uint8_t spiBurstRead(uint8_t reg, const uint8_t* dest, uint8_t len);
    uint8_t spiBurstWrite(uint8_t reg, const uint8_t* dest, uint8_t len);

    // SPI Configuration methods
    /// Enable SPI interrupts
    /// This can be used in an SPI slave to indicate when an SPI message has been received
    /// It will cause the SPI_STC_vect interrupt vectr to be executed
    void attachInterrupt(uint8_t, void (*function)(void), int);

    /// Disable SPI interrupts
    /// This can be used to diable the SPI interrupt in slaves where that is supported.
    void detachInterrupt(uint8_t, void (*function)(void));

    /// Initialise the SPI library
    /// Call this after configuring the SPI interface and before using it to transfer data.
    /// Initializes the SPI bus by setting SCK, MOSI, and SS to outputs, pulling SCK and MOSI low, and SS high.
    void begin(char*);

    /// Disables the SPI bus (leaving pin modes unchanged).
    /// Call this after you have finished using the SPI interface.
    void end();

private:
	int exchange(uint32_t length);
	//SPI _spi;
	int _fd;
	uint32_t _speed;
	uint8_t _dataMode;
	uint8_t _bits;
	uint8_t _rx_buffer[255+1];
	uint8_t _tx_buffer[255+1];
};

#endif
