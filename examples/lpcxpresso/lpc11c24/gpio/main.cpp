/**
 * \brief	Minimal example for LPC11C24 on LPCXpresso board.
 *
 * The LED2 is lit when the device boots.
 * If the GPIO P3_2 (lower right corner) is tied low
 * the LED2 is switched off.
 *
 * Tested in hardware 2012-08-08 by strongly-typed.
 * Tested again 2014-08-01.
 * Tested again 2017-10-26.
*/

#include <xpcc/architecture.hpp>

using Button = GpioInput3_2;

int
main()
{
	Board::initialize();

	// Set LED port pin to output
	Button::setInput(Gpio::InputType::PullUp);

	while (true)
	{
		// Each time we wake up...
		Board::LedRed::set(Button::read());
		// Go to sleep to save power between timer interrupts
		__WFI();
	}

/*
	// Use all other defines once to verify that it is compilable
	But::setInput();
	But::setInput(xpcc::lpc::FLOATING);
	But::setInput(xpcc::lpc::PULLUP);
	But::setInput(xpcc::lpc::PULLDOWN);
	But::setInput(xpcc::lpc::REPEATER);

	Led::setOutput();
	Led::setOutput(true);
	Led::setOutput(false);
	Led::setOutput(xpcc::lpc::PUSH_PULL);
	Led::setOutput(xpcc::lpc::OPEN_DRAIN);
	Led::set();
	Led::reset();
	Led::toggle();

	GPIO__IO(Io, 0, 3);
	Io::setOutput();
	Io::setOutput(true);
	Io::setOutput(false);
	Io::setOutput(xpcc::lpc::PUSH_PULL);
	Io::setOutput(xpcc::lpc::OPEN_DRAIN);
	Io::setInput();
	Io::setInput(xpcc::lpc::FLOATING);
	Io::setInput(xpcc::lpc::PULLUP);
	Io::setInput(xpcc::lpc::PULLDOWN);
	Io::setInput(xpcc::lpc::REPEATER);
	Io::set();
	Io::reset();
	Io::toggle();
	Io::set(true);
	Io::set(false);
	Led::set(Io::read());
*/
}
