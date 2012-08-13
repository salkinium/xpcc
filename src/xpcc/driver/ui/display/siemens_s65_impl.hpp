// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// ----------------------------------------------------------------------------
/*
 * Code from:
 * http://www.watterott.net/projects/s65-shield
 * http://www.superkranz.de/christian/S65_Display/DisplaySoftware.html
 *
 * Pinout from:
 * http://www.juras-projects.org/eng/lcd.php
 */

#ifndef XPCC__SIEMENS_S65_HPP
#error	"Don't include this file directly, use 'siemens_s65.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename SPI, typename CS, typename RS, typename Reset>
void
xpcc::SiemensS65<SPI, CS, RS, Reset>::initialize()
{
	// CS pin
	CS::setOutput(true);

	// RS pin
	RS::setOutput(false);

	// Reset pin
	Reset::setOutput(false);

	lcdSettings();

	this->clear();
	//	this->update();
}

template <typename SPI, typename CS, typename RS, typename Reset>
void
xpcc::SiemensS65<SPI, CS, RS, Reset>::writeCmd(uint8_t reg, uint16_t param)
{
	writeReg(reg);
	writeData(param);
}

template <typename SPI, typename CS, typename RS, typename Reset>
void
xpcc::SiemensS65<SPI, CS, RS, Reset>::writeReg(uint8_t reg)
{
	CS::reset();
	SPI::write(0x74); // start byte, RS = 0, R/W = 0, receive index register
	SPI::write(0x00);
	SPI::write(reg);

//	while(SPI::isBusy()); // wait until SPI has emptied its buffer before setting CS.
	CS::set();
}

template <typename SPI, typename CS, typename RS, typename Reset>
void
xpcc::SiemensS65<SPI, CS, RS, Reset>::writeData(uint16_t data)
{
	CS::reset();
	SPI::write(0x76);	// start byte, RS = 1, R/W, receive instruction or RAM data
	SPI::write(data>>8);
	SPI::write(data);
	CS::set();
}

template <typename SPI, typename CS, typename RS, typename Reset>
void
xpcc::SiemensS65<SPI, CS, RS, Reset>::lcdSettings() {
	// Hardware reset is low from initialize
	xpcc::delay_ms(50);
	Reset::set();
	xpcc::delay_ms(50);

	writeCmd(0x07, 0x0000); //display off
	xpcc::delay_ms(10);

	//power on sequence
	writeCmd(0x02, 0x0400); //lcd drive control
	writeCmd(0x0C, 0x0001); //power control 3: VC        //step 1
	writeCmd(0x0D, 0x0006); //power control 4: VRH
	writeCmd(0x04, 0x0000); //power control 2: CAD
	writeCmd(0x0D, 0x0616); //power control 4: VRL
	writeCmd(0x0E, 0x0010); //power control 5: VCM
	writeCmd(0x0E, 0x1010); //power control 5: VDV
	writeCmd(0x03, 0x0000); //power control 1: BT        //step 2
	writeCmd(0x03, 0x0000); //power control 1: DC
	writeCmd(0x03, 0x000C); //power control 1: AP
	xpcc::delay_ms(40);
	writeCmd(0x0E, 0x2D1F); //power control 5: VCOMG     //step 3
	xpcc::delay_ms(40);
	writeCmd(0x0D, 0x0616); //power control 4: PON       //step 4
	xpcc::delay_ms(100);

	//display options
#if defined(S65_MIRROR)
	writeCmd(0x05, 0x0008); //Entry mode --
#else
	writeCmd(0x05, 0x0038); //Entry mode ++
#endif
	//setArea(0, 0, (S65_WIDTH-1), (S65_HEIGHT-1));
	//  writeCmd(0x16, 176<<8); //set y
	//  writeCmd(0x17, 132<<8); //set x

	//display on sequence (bit2 = reversed colors)
	writeCmd(0x07, 0x0005); //display control: D0
	writeCmd(0x07, 0x0025); //display control: GON
	writeCmd(0x07, 0x0027); //display control: D1
	writeCmd(0x07, 0x0037); //display control: DTE
	xpcc::delay_ms(10);

	lcdCls(0x03e0);
}

template <typename SPI, typename CS, typename RS, typename Reset>
void
xpcc::SiemensS65<SPI, CS, RS, Reset>::lcdCls(uint16_t colour) {
	writeReg(0x22); // ?? write data when CS is high does not make sense.
	CS::reset();
	SPI::write(0x76);

	// start data transmission
	uint8_t c1 = colour >> 8;
	uint8_t c2 = colour & 0xff;
	for (uint16_t i = 0; i < 132*176; i++)
	{
		SPI::write(c1);
		SPI::write(c2);
	}

	CS::set();
}

GPIO__OUTPUT(Dbg, 1, 11);
GPIO__OUTPUT(Dbg2, 1, 10);

template <typename SPI, typename CS, typename RS, typename Reset>
void
xpcc::SiemensS65<SPI, CS, RS, Reset>::update() {

	// WRITE MEMORY
	CS::reset();
	SPI::write(0x76);	// start byte

	const uint16_t maskBlank  = 0x0000; // RRRR RGGG GGGB BBBB
	const uint16_t maskFilled = 0x37e0; // RRRR RGGG GGGB BBBB

	const uint8_t width = this->getWidth();
	const uint8_t height = this->getHeight()/8;

#ifdef LPC_ACCELERATED
	/**
	 * This is an extremely optimised version of the copy routine for LPC
	 * microcontrollers. It will work with any SPI block that support 16-bit
	 * transfer and a FIFO with a size of at least 8 frames.
	 *
	 * The SPI is switched to 16-bit mode to reduce the
	 * numbers of bus accesses and use the trick that then 8 pixels of each
	 * 16 bits fit into the FIFO.
	 *
	 * This reduces the copy time to 17.6 msec when running at 48 MHz. This
	 * is a peak transfer rate of 2.6 MiByte/sec
	 */

	// switch to 16 bit mode, wait for empty FIFO before
	while (!(LPC_SSP0->SR & SPI_SRn_TFE));
	LPC_SSP0->CR0 |= 0x0f;

	for (uint8_t x = width - 1; x != 0xff; --x)
	{
		for (uint8_t y = 0; y < height; ++y)
		{
			// group of 8 black-and-white pixels
			uint8_t group = this->buffer[x][y];

			// 8 pixels of 16 bits fit in the Tx FIFO if it is empty.
			while (!(LPC_SSP0->SR & SPI_SRn_TFE));

			for (uint8_t pix = 0; pix < 8; ++pix, group >>= 1) {
				LPC_SSP0->DR = (group & 1) ? maskFilled : maskBlank;
			} // pix
		} // y
	} // x

	// switch back to 8 bit transfer, wait for empty FIFO before.
	while (!(LPC_SSP0->SR & SPI_SRn_TFE));
	LPC_SSP0->CR0 &= ~0xff;
	LPC_SSP0->CR0 |=  0x07;

#else
	// ----- Normal version with SPI buffer
	const uint8_t fill_h = maskFilled >> 8;
	const uint8_t fill_l = maskFilled & 0xff;

	const uint8_t blank_h = maskBlank >> 8;
	const uint8_t blank_l = maskBlank & 0xff;

	for (uint8_t x = width - 1; x != 0xff; --x)
	{
		for (uint8_t y = 0; y < height; ++y)
		{
			// group of 8 black-and-white pixels
			uint8_t group = this->buffer[x][y];
			uint8_t spiBuffer[16];
			uint8_t spiIdx = 0;

			for (uint8_t pix = 0; pix < 8; ++pix, group >>= 1) {
				if (group & 1)
				{
					spiBuffer[spiIdx++] = fill_h;
					spiBuffer[spiIdx++] = fill_l;
				}
				else
				{
					spiBuffer[spiIdx++] = blank_h;
					spiBuffer[spiIdx++] = blank_l;
				}
			} // pix

			// use transfer() of SPI to transfer spiBuffer
			SPI::setBuffer(16, spiBuffer);
			SPI::transfer(SPI::TRANSFER_SEND_BUFFER_DISCARD_RECEIVE);
		} // y
	} // x
#endif

	CS::set();
}
