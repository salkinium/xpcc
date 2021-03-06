
#include <xpcc/debug/logger.hpp>
#include <xpcc/architecture.hpp>

#include <xpcc/architecture/interface/can.hpp>
#include <xpcc/architecture/platform/driver/can/canusb/canusb.hpp>

/**
 * Simple example that listens to a CAN bus connected by a CAN2USB.
 *
 * How to use:
 * - Connect a CAN2USB to a CAN bus with traffic.
 * - Connect this CAN2USB by USB to your computer.
 * - Make sure the user can access the /dev/ttyUSB0 device.
 *   Brute force method is: #sudo chmod 777 /dev/ttyUSB0
 *   A wiser method is to add the user the appropriate group
 * - Adjust the baud rate of the CAN bus in this example.
 * - Do
 *   scons run
 * - All CAN messages on the bus should appear on the screen.
 */

static constexpr xpcc::Can::Bitrate canBusBitRate = xpcc::Can::kBps125;

xpcc::hosted::SerialInterface port("/dev/ttyUSB0", 115200);
xpcc::hosted::CanUsb<xpcc::hosted::SerialInterface> canUsb(port);

int
main()
{
	if (not canUsb.open(canBusBitRate)) {
		XPCC_LOG_ERROR << "Could not connect to CAN2USB port" << xpcc::endl;
		exit(EXIT_FAILURE);
	}

	while(true)
	{
		if (canUsb.isMessageAvailable())
		{
			xpcc::can::Message message;
			canUsb.getMessage(message);
			
			XPCC_LOG_DEBUG << message << xpcc::endl;
		}
	}

	canUsb.close();
}
