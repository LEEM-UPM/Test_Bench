#include "GyroNick_WT.h"
#include "Wire.h"

VRU::VRU(HardwareSerial *Ser)
{
	I2C_add = 0;
	Serie = Ser;
	Serie->begin(115200);
}

VRU::VRU(uint8_t address = 0x50)
{
	Serie = NULL;
	I2C_add = address;
	Wire.begin();
}

bool VRU::readTTL()
{
	memset(&buffer, 0x00, BUFFSIZE);

	if (Serie->available() > 0)
	{
		short nbr = Serie->available();
		Serie->readBytes(buffer, nbr);

		for (uint8_t i = 0; i < nbr; i++)
		{
			if (buffer[i] == 0x55)
			{
				//PARSE
				return;
			}
		}
	}
}

void VRU::readI2C()
{
	getI2Cdata(AX, 6);
	acc[0] = float(short(buffer[1] << 8 | buffer[0])) * 16.0 * 9.8 / 32768;
	acc[1] = float(short(buffer[3] << 8 | buffer[2])) * 16.0 * 9.8 / 32768;
	acc[2] = float(short(buffer[5] << 8 | buffer[4])) * 16.0 * 9.8 / 32768;

	getI2Cdata(Roll, 6);
	angle[0] = float(uint16_t(buffer[1] << 8 | buffer[0])) * 180 / 32768;
	angle[1] = float(uint16_t(buffer[3] << 8 | buffer[2])) * 180 / 32768;
	angle[2] = float(uint16_t(buffer[5] << 8 | buffer[4])) * 180 / 32768;

	getI2Cdata(TEMP, 2);
	temp = float(uint16_t(buffer[1] << 8 | buffer[0])) / 100;

}

void VRU::getI2Cdata(uint8_t addressToRead, uint8_t bytesToRead)
{
	memset(&buffer, 0x00, BUFFSIZE);

	Wire.beginTransmission(I2C_add);
	Wire.write(addressToRead);
	Wire.endTransmission(false); //endTransmission but keep the connection active

	Wire.requestFrom(I2C_add, bytesToRead); //Ask for bytes, once done, bus is released by default

	while (Wire.available() < bytesToRead);
	{
		for (int x = 0; x < bytesToRead; x++)
		{
			buffer[x] = Wire.read();
		}
	}
}

void VRU::flush()
{
	memset(&buffer, 0x00, BUFFSIZE);
	if (Serie->available() > 0)
	{
		short nbr = Serie->available();
		Serie->readBytes(&buffer[0], nbr);
	}
}