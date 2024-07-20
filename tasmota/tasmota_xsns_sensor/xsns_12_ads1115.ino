/*
  xsns_12_ads1115_ada.ino - ADS1115 A/D Converter support for Tasmota

  Copyright (C) 2021  Syssi, stefanbode

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifdef USE_I2C
#ifdef USE_ADS1115
/*********************************************************************************************\
 * ADS1115 - 4 channel 16BIT A/D converter
 *
 * Required library: none but based on Adafruit Industries ADS1015 library
 *
 * I2C Address: 0x48, 0x49, 0x4A or 0x4B
 *
 * The ADC input range (or gain) can be changed via the following
 * defines, but be careful never to exceed VDD +0.3V max, or to
 * exceed the upper and lower limits if you adjust the input range!
 * Setting these values incorrectly may destroy your ADC!
 *                                                                 ADS1115
 *                                                                 -------
 * ADS1115_REG_CONFIG_PGA_6_144V  // 2/3x gain +/- 6.144V  1 bit = 0.1875mV (default)
 * ADS1115_REG_CONFIG_PGA_4_096V  // 1x gain   +/- 4.096V  1 bit = 0.125mV
 * ADS1115_REG_CONFIG_PGA_2_048V  // 2x gain   +/- 2.048V  1 bit = 0.0625mV
 * ADS1115_REG_CONFIG_PGA_1_024V  // 4x gain   +/- 1.024V  1 bit = 0.03125mV
 * ADS1115_REG_CONFIG_PGA_0_512V  // 8x gain   +/- 0.512V  1 bit = 0.015625mV
 * ADS1115_REG_CONFIG_PGA_0_256V  // 16x gain  +/- 0.256V  1 bit = 0.0078125mV
\*********************************************************************************************/

#define XSNS_12                         12
#define XI2C_13                         13        // See I2CDEVICES.md

#define ADS1115_ADDRESS_ADDR_GND        0x48      // address pin low (GND)
#define ADS1115_ADDRESS_ADDR_VDD        0x49      // address pin high (VCC)
#define ADS1115_ADDRESS_ADDR_SDA        0x4A      // address pin tied to SDA pin
#define ADS1115_ADDRESS_ADDR_SCL        0x4B      // address pin tied to SCL pin

#define ADS1115_CONVERSIONDELAY         (8)       // CONVERSION DELAY (in mS)

#define ADS1115_SINGLE_CHANNELS         (4)
#define ADS1115_DIFFERENTIAL_CHANNELS   (2)

/*======================================================================
POINTER REGISTER
-----------------------------------------------------------------------*/
#define ADS1115_REG_POINTER_MASK        (0x03)
#define ADS1115_REG_POINTER_CONVERT     (0x00)
#define ADS1115_REG_POINTER_CONFIG      (0x01)
#define ADS1115_REG_POINTER_LOWTHRESH   (0x02)
#define ADS1115_REG_POINTER_HITHRESH    (0x03)

/*======================================================================
CONFIG REGISTER
-----------------------------------------------------------------------*/
#define ADS1115_REG_CONFIG_OS_MASK      (0x8000)
#define ADS1115_REG_CONFIG_OS_SINGLE    (0x8000)  // Write: Set to start a single-conversion
#define ADS1115_REG_CONFIG_OS_BUSY      (0x0000)  // Read: Bit = 0 when conversion is in progress
#define ADS1115_REG_CONFIG_OS_NOTBUSY   (0x8000)  // Read: Bit = 1 when device is not performing a conversion

#define ADS1115_REG_CONFIG_MUX_MASK     (0x7000)
#define ADS1115_REG_CONFIG_MUX_DIFF_0_1 (0x0000)  // Differential P = AIN0, N = AIN1 (default)
#define ADS1115_REG_CONFIG_MUX_DIFF_0_3 (0x1000)  // Differential P = AIN0, N = AIN3
#define ADS1115_REG_CONFIG_MUX_DIFF_1_3 (0x2000)  // Differential P = AIN1, N = AIN3
#define ADS1115_REG_CONFIG_MUX_DIFF_2_3 (0x3000)  // Differential P = AIN2, N = AIN3
#define ADS1115_REG_CONFIG_MUX_SINGLE_0 (0x4000)  // Single-ended AIN0
#define ADS1115_REG_CONFIG_MUX_SINGLE_1 (0x5000)  // Single-ended AIN1
#define ADS1115_REG_CONFIG_MUX_SINGLE_2 (0x6000)  // Single-ended AIN2
#define ADS1115_REG_CONFIG_MUX_SINGLE_3 (0x7000)  // Single-ended AIN3

#define ADS1115_REG_CONFIG_PGA_MASK     (0x0E00)
#define ADS1115_REG_CONFIG_PGA_6_144V   (0x0000)  // +/-6.144V range = Gain 2/3 (default)
#define ADS1115_REG_CONFIG_PGA_4_096V   (0x0200)  // +/-4.096V range = Gain 1
#define ADS1115_REG_CONFIG_PGA_2_048V   (0x0400)  // +/-2.048V range = Gain 2
#define ADS1115_REG_CONFIG_PGA_1_024V   (0x0600)  // +/-1.024V range = Gain 4
#define ADS1115_REG_CONFIG_PGA_0_512V   (0x0800)  // +/-0.512V range = Gain 8
#define ADS1115_REG_CONFIG_PGA_0_256V   (0x0A00)  // +/-0.256V range = Gain 16

#define ADS1115_REG_CONFIG_MODE_MASK    (0x0100)
#define ADS1115_REG_CONFIG_MODE_CONTIN  (0x0000)  // Continuous conversion mode
#define ADS1115_REG_CONFIG_MODE_SINGLE  (0x0100)  // Power-down single-shot mode (default)

#define ADS1115_REG_CONFIG_DR_MASK      (0x00E0)
#define ADS1115_REG_CONFIG_DR_128SPS    (0x0000)  // 128 samples per second
#define ADS1115_REG_CONFIG_DR_250SPS    (0x0020)  // 250 samples per second
#define ADS1115_REG_CONFIG_DR_490SPS    (0x0040)  // 490 samples per second
#define ADS1115_REG_CONFIG_DR_920SPS    (0x0060)  // 920 samples per second
#define ADS1115_REG_CONFIG_DR_1600SPS   (0x0080)  // 1600 samples per second (default)
#define ADS1115_REG_CONFIG_DR_2400SPS   (0x00A0)  // 2400 samples per second
#define ADS1115_REG_CONFIG_DR_3300SPS   (0x00C0)  // 3300 samples per second
#define ADS1115_REG_CONFIG_DR_6000SPS   (0x00E0)  // 6000 samples per second

#define ADS1115_REG_CONFIG_CMODE_MASK   (0x0010)
#define ADS1115_REG_CONFIG_CMODE_TRAD   (0x0000)  // Traditional comparator with hysteresis (default)
#define ADS1115_REG_CONFIG_CMODE_WINDOW (0x0010)  // Window comparator

#define ADS1115_REG_CONFIG_CPOL_MASK    (0x0008)
#define ADS1115_REG_CONFIG_CPOL_ACTVLOW (0x0000)  // ALERT/RDY pin is low when active (default)
#define ADS1115_REG_CONFIG_CPOL_ACTVHI  (0x0008)  // ALERT/RDY pin is high when active

#define ADS1115_REG_CONFIG_CLAT_MASK    (0x0004)  // Determines if ALERT/RDY pin latches once asserted
#define ADS1115_REG_CONFIG_CLAT_NONLAT  (0x0000)  // Non-latching comparator (default)
#define ADS1115_REG_CONFIG_CLAT_LATCH   (0x0004)  // Latching comparator

#define ADS1115_REG_CONFIG_CQUE_MASK    (0x0003)
#define ADS1115_REG_CONFIG_CQUE_1CONV   (0x0000)  // Assert ALERT/RDY after one conversions
#define ADS1115_REG_CONFIG_CQUE_2CONV   (0x0001)  // Assert ALERT/RDY after two conversions
#define ADS1115_REG_CONFIG_CQUE_4CONV   (0x0002)  // Assert ALERT/RDY after four conversions
#define ADS1115_REG_CONFIG_CQUE_NONE    (0x0003)  // Disable the comparator and put ALERT/RDY in high state (default)

uint16_t ads1115_ranges[] = { ADS1115_REG_CONFIG_PGA_6_144V, ADS1115_REG_CONFIG_PGA_4_096V, ADS1115_REG_CONFIG_PGA_2_048V, ADS1115_REG_CONFIG_PGA_1_024V, ADS1115_REG_CONFIG_PGA_0_512V, ADS1115_REG_CONFIG_PGA_0_256V };
uint8_t ads1115_addresses[] = { ADS1115_ADDRESS_ADDR_GND, ADS1115_ADDRESS_ADDR_VDD, ADS1115_ADDRESS_ADDR_SDA, ADS1115_ADDRESS_ADDR_SCL };
uint8_t ads1115_count = 0;
uint16_t ads1115_range;
uint8_t ads1115_channels;

struct ADS1115 {
  int16_t last_values[4] = { 0,0,0,0 };
  uint8_t address; 
  uint8_t bus;
} Ads1115[4];

//Ads1115StartComparator(channel, ADS1115_REG_CONFIG_MODE_SINGLE);
//Ads1115StartComparator(channel, ADS1115_REG_CONFIG_MODE_CONTIN);
void Ads1115StartComparator(uint32_t device, uint8_t channel, uint16_t mode) {
  // Start with default values
  uint16_t config = mode |
                    ADS1115_REG_CONFIG_CQUE_NONE    | // Comparator enabled and asserts on 1 match
                    ADS1115_REG_CONFIG_CLAT_NONLAT  | // Non Latching mode
                    ads1115_range                   | // ADC Input voltage range (Gain)
                    ADS1115_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
                    ADS1115_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
                    ADS1115_REG_CONFIG_DR_6000SPS;    // 6000 samples per second

  // Set single-ended or differential input channel
  if (ads1115_channels == ADS1115_SINGLE_CHANNELS) {
    config |= (ADS1115_REG_CONFIG_MUX_SINGLE_0 + (0x1000 * channel));
  } else {
    config |= (ADS1115_REG_CONFIG_MUX_DIFF_0_1 + (0x3000 * channel));
  }

  // Write config register to the ADC
  I2cWrite16(Ads1115[device].address, ADS1115_REG_POINTER_CONFIG, config, Ads1115[device].bus);
}

int16_t Ads1115GetConversion(uint32_t device, uint8_t channel) {
  device = 0;
  channel = 0;
  // Ads1115StartComparator(device, channel, ADS1115_REG_CONFIG_MODE_SINGLE);
  // // Wait for the conversion to complete
  // delay(ADS1115_CONVERSIONDELAY);
  // // Read the conversion results
  // I2cRead16(Ads1115[device].address, ADS1115_REG_POINTER_CONVERT, Ads1115[device].bus);

  // Ads1115StartComparator(device, channel, ADS1115_REG_CONFIG_MODE_CONTIN);
  // delay(ADS1115_CONVERSIONDELAY);
  // Read the conversion results
  uint16_t res = I2cRead16(Ads1115[device].address, ADS1115_REG_POINTER_CONVERT, Ads1115[device].bus);
  return (int16_t)res;
}

int16_t Ads1115GetQuickConversion() {
  uint16_t res = I2cRead16(Ads1115[0].address, ADS1115_REG_POINTER_CONVERT, Ads1115[0].bus);
  return (int16_t)res;
}

/********************************************************************************************/

void Ads1115Detect(void) {
  // Set default mode and range
  ads1115_channels = ADS1115_SINGLE_CHANNELS;
  ads1115_range = ADS1115_REG_CONFIG_PGA_6_144V;
  for (uint32_t bus = 0; bus < 2; bus++) {
    for (uint32_t i = 0; i < sizeof(ads1115_addresses); i++) {
      if (!I2cSetDevice(ads1115_addresses[i], bus)) { continue; }
      uint16_t buffer;
      if (I2cValidRead16(&buffer, ads1115_addresses[i], ADS1115_REG_POINTER_CONVERT, bus) &&
          I2cValidRead16(&buffer, ads1115_addresses[i], ADS1115_REG_POINTER_CONFIG, bus)) {
        Ads1115[ads1115_count].address = ads1115_addresses[i];
        Ads1115[ads1115_count].bus = bus;
        Ads1115StartComparator(ads1115_count, 0, ADS1115_REG_CONFIG_MODE_CONTIN);
        I2cSetActiveFound(Ads1115[ads1115_count].address, "ADS1115", Ads1115[ads1115_count].bus);
        ads1115_count++;
        if (4 == ads1115_count) { return; }
      }
    }
  }
}

#ifdef USE_RULES
// Check every 250ms if there are relevant changes in any of the analog inputs
// and if so then trigger a message
void AdsEvery250ms(void) {
  int16_t value;

  for (uint32_t t = 0; t < ads1115_count; t++) {
    // collect first wich addresses have changed. We can save on rule processing this way
    uint32_t changed = 0;
    for (uint32_t i = 0; i < ads1115_channels; i++) {
      // value = Ads1115GetConversion(t, i);

      // Check if value has changed more than 1 percent from last stored value
      // we assume that gain is set up correctly, and we could use the whole 16bit result space
      if (value >= Ads1115[t].last_values[i] + 327 || value <= Ads1115[t].last_values[i] - 327) {
        Ads1115[t].last_values[i] = value;
        bitSet(changed, i);
      }
    }
  }
}
#endif  // USE_RULES

void Ads1115Show(bool json) {
  // Do nothing -- we don't need these raw values
}

bool ADS1115_Command(void) {
  // Sensor12 D2
  // Sensor12 S0
  if (XdrvMailbox.data_len > 1) {
    UpperCase(XdrvMailbox.data, XdrvMailbox.data);
    switch (XdrvMailbox.data[0]) {
      case 'D':
        ads1115_channels = ADS1115_DIFFERENTIAL_CHANNELS;
        break;
      case 'S':
        ads1115_channels = ADS1115_SINGLE_CHANNELS;
    }
//    uint32_t range_index = atoi((const char*)XdrvMailbox.data[1]);    
    uint32_t range_index = atoi((const char*)XdrvMailbox.data +1);    
    if ((range_index >= 0) && (range_index <= 5)) {
      ads1115_range = ads1115_ranges[range_index];
    }
  }
  const char ds[2][13] = { "Differential", "Single ended" };
  const uint16_t r[6] = { 6144, 4096, 2048, 1024, 512, 256 };
  Response_P("{\"ADS1115\":{\"Settings\":\"%c%u\",\"Mode\":\"%s\",\"Range\":%u,\"Unit\":\"mV\"}}",
    ds[(ads1115_channels>>1)-1][0], ads1115_range>>9, ds[(ads1115_channels>>1)-1], r[ads1115_range>>9]);
  return true;
}

/*********************************************************************************************\
 * Interface
\*********************************************************************************************/

bool Xsns12(uint32_t function)
{
  if (!I2cEnabled(XI2C_13)) { return false; }

  bool result = false;

  if (FUNC_INIT == function) {
    Ads1115Detect();
  }
  else if (ads1115_count) {
    switch (function) {
#ifdef USE_RULES
      case FUNC_EVERY_250_MSECOND:
        AdsEvery250ms();
        break;
#endif  // USE_RULES
      case FUNC_JSON_APPEND:
        Ads1115Show(1);
        break;
#ifdef USE_WEBSERVER
      case FUNC_WEB_SENSOR:
        Ads1115Show(0);
        break;
#endif  // USE_WEBSERVER
      case FUNC_COMMAND_SENSOR:
        if (XSNS_12 == XdrvMailbox.index) {
          result = ADS1115_Command();
        }
        break;
    }
  }
  return result;
}

#endif  // USE_ADS1115
#endif  // USE_I2C
