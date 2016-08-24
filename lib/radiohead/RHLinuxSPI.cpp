// RHLinuxSPI.h
// Author: Mike McCauley (mikem@airspayce.com)
// Copyright (C) 2011 Mike McCauley
// Contributed by Joanna Rutkowska
// $Id: RHLinuxSPI.cpp,v 1.9 2014/06/24 02:40:12 mikem Exp $

#include <RHLinuxSPI.h>

//#define RH_LINUX_SPI_DEBUG 1

#define RH_SPI_WRITE_MASK 0x80

// Declare a single default instance of the hardware SPI interface class
RHLinuxSPI hardware_spi;

RHLinuxSPI::RHLinuxSPI(Frequency frequency, BitOrder bitOrder, DataMode dataMode)
    :
    RHGenericSPI(frequency, bitOrder, dataMode)
{
	_bits = 8;
}

bool RHLinuxSPI::init(const char* pDevice)
{
	if( (_fd = ::open(pDevice, O_RDWR)) < 0) {
		perror("Unable to open SPI device");
		return false;
	}
	usleep(100000);

	return true;
}

uint8_t RHLinuxSPI::transfer(uint8_t data)
{
	_tx_buffer[0] = data;
	exchange(2);

#ifdef RH_LINUX_SPI_DEBUG
	printf("RHLinuxSPI::transfer() : 0x%2.2X 0x%2.2X\n", data, _rx_buffer[1]);
#endif

    return _rx_buffer[1];
}

void RHLinuxSPI::attachInterrupt(uint8_t gpio, void (*function)(void), int mode)
{
#ifdef RH_LINUX_SPI_DEBUG
	printf("RHLinuxSPI::attachInterrupt() : gpio=%d mode=%d\n", gpio, mode);
#endif
	pinMode(gpio, INPUT);
	if( wiringPiISR(gpio, mode, function) < 0 ){
		printf("RHLinuxSPI::attachInterrupt() : wiringPiIsr isr failed!\n");
	}
}

void RHLinuxSPI::detachInterrupt(uint8_t gpio, void (*function)(void))
{
#ifdef RH_LINUX_SPI_DEBUG
	printf("RHLinuxSPI::detachInterrupt()\n");
#endif
}

uint8_t RHLinuxSPI::spiRead(uint8_t reg)
{
	_tx_buffer[0] = reg & ~RH_SPI_WRITE_MASK;
	exchange(2);

	return _rx_buffer[1];
}

uint8_t RHLinuxSPI::spiWrite(uint8_t reg, uint8_t val)
{
    uint8_t status = 0;

	_tx_buffer[0] = reg | RH_SPI_WRITE_MASK;
	_tx_buffer[1] = val;
	status = exchange(2);

	return status;
}

uint8_t RHLinuxSPI::spiBurstRead(uint8_t reg, const uint8_t* dest, uint8_t len)
{
	int ret;

	_tx_buffer[0] = reg & ~RH_SPI_WRITE_MASK;
	if( (ret = exchange(len+1)) < 0){
		return 0;
	}
	memcpy((void*)dest, (void*)&_rx_buffer[1], len);

	return ret;
}

uint8_t RHLinuxSPI::spiBurstWrite(uint8_t reg, const uint8_t* src, uint8_t len)
{
   int ret;

	_tx_buffer[0] = reg | RH_SPI_WRITE_MASK;
	memcpy((void*)&_tx_buffer[1], (void*)src, len);
	if( (ret = exchange(len+1)) < 0){
		return 0;
	}

	return ret;
}

int RHLinuxSPI::exchange(uint32_t length)
{
	int ret;
   struct spi_ioc_transfer tr = {0};

   tr.tx_buf = (unsigned long)_tx_buffer;	//tx and rx MUST be the same length!
   tr.rx_buf = (unsigned long)_rx_buffer;
   tr.len = length;
   tr.delay_usecs = 0;
   tr.speed_hz = _speed;
   tr.bits_per_word = _bits;
   tr.pad=0;

	if( (ret = ioctl(_fd, SPI_IOC_MESSAGE(1), &tr)) < 0)
		perror("RHLinuxSPI::exchange() : failed");
#ifdef RH_LINUX_SPI_DEBUG
	printf("RHLinuxSPI::exchange() : length=%d ret=%d\n", length,  ret);
#endif

	return ret;
}

void RHLinuxSPI::begin(char* pDevice)
{
#ifdef RH_LINUX_SPI_DEBUG
	printf("RHLinuxSPI::begin()\n");
#endif
	/* Bits size */
	if ( ioctl(_fd, SPI_IOC_WR_BITS_PER_WORD, &_bits) < 0 )
		perror("RHLinuxSPI::begin() : Unable to set bits");
	if ( ioctl(_fd, SPI_IOC_RD_BITS_PER_WORD, &_bits) < 0 )
		perror("RHLinuxSPI::begin() : Unable to get bits");

	/* Frequency configuration */
	switch (_frequency) {
		case Frequency1MHz:
		default:
		    _speed = 1000000;
		    break;

		case Frequency2MHz:
		    _speed = 2000000;
		    break;

		case Frequency4MHz:
		    _speed = 4000000;
		    break;

		case Frequency8MHz:
		    _speed = 8000000;
		    break;

		case Frequency16MHz:
		    _speed = 16000000;
		    break;
	}
	if ( ioctl(_fd, SPI_IOC_WR_MAX_SPEED_HZ, &_speed) < 0 )
		perror("RHLinuxSPI::begin() : Unable to set SPI speed");
	if ( ioctl(_fd, SPI_IOC_RD_MAX_SPEED_HZ, &_speed) < 0)
		perror("RHLinuxSPI::begin() : Unable to get SPI speed");

	/* SPI mode */
	switch(RHGenericSPI::_dataMode){
		case DataMode0:
		default:
			_dataMode = 0;
			break;
		case DataMode1:
			_dataMode = 1;
			break;
		case DataMode2:
			_dataMode = 2;
			break;
		case DataMode3:
			_dataMode = 3;
			break;
	}
	if ( ioctl(_fd, SPI_IOC_WR_MODE, &_dataMode) < 0 )
		perror("RHLinuxSPI::begin() : Unable to set SPI mode");
	if ( ioctl(_fd, SPI_IOC_RD_MODE, &_dataMode) < 0 )
		perror("Unable to get SPI mode");
}

void RHLinuxSPI::end()
{
	::close(_fd);
}
