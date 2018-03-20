#include <LSM303D.h>
#include <Wire.h>
//  #include <math.h>

// Defines ////////////////////////////////////////////////////////////////

// The Arduino two-wire interface uses a 7-bit number for the address,
// and sets the last bit correctly based on reads and writes
#define D_SA0_HIGH_ADDRESS                0b0011101
#define D_SA0_LOW_ADDRESS                 0b0011110
// #define DLM_DLH_ACC_SA0_LOW_ADDRESS       0b0011000

#define TEST_REG_ERROR -1

#define D_WHO_ID    0x49
// #define DLM_WHO_ID  0x3C

// Constructors ////////////////////////////////////////////////////////////////

LSM303::LSM303(void)
{
  /*
  These values lead to an assumed magnetometer bias of 0.
  Use the Calibrate example program to determine appropriate values
  for your particular unit. The Heading example demonstrates how to
  adjust these values in your own sketch.
  */
//  m_min = (LSM303::vector<int16_t>){-32767, -32767, -32767};
//  m_max = (LSM303::vector<int16_t>){+32767, +32767, +32767};

  _device = device_auto;

  io_timeout = 0;  // 0 = no timeout
  did_timeout = false;
}

// Public Methods //////////////////////////////////////////////////////////////

// Did a timeout occur in readAcc(), readMag(), or read() since the last call to timeoutOccurred()?
bool LSM303::timeoutOccurred()
{
  bool tmp = did_timeout;
  did_timeout = false;
  return tmp;
}

void LSM303::setTimeout(unsigned int timeout)
{
  io_timeout = timeout;
}

unsigned int LSM303::getTimeout()
{
  return io_timeout;
}

bool LSM303::testDevice(){
    return testReg(D_SA0_HIGH_ADDRESS, WHO_AM_I) == D_WHO_ID;
}

bool LSM303::init(deviceType device, sa0State sa0)
{
  // perform auto-detection unless device type and SA0 state were both specified
  if (device == device_auto || sa0 == sa0_auto)
  {
    // check for LSM303D if device is unidentified or was specified to be this type
    if (device == device_auto || device == device_D)
    {
      // check SA0 high address unless SA0 was specified to be low
      if (sa0 != sa0_low && testReg(D_SA0_HIGH_ADDRESS, WHO_AM_I) == D_WHO_ID)
      {
        // device responds to address 0011101 with D ID; it's a D with SA0 high
        device = device_D;
        sa0 = sa0_high;
      }
      // check SA0 low address unless SA0 was specified to be high
      else if (sa0 != sa0_high && testReg(D_SA0_LOW_ADDRESS, WHO_AM_I) == D_WHO_ID)
      {
        // device responds to address 0011110 with D ID; it's a D with SA0 low
        device = device_D;
        sa0 = sa0_low;
      }
    }
    

    // make sure device and SA0 were successfully detected; otherwise, indicate failure
    if (device == device_auto || sa0 == sa0_auto)
    {
//      return false;
    }
  }
  
  _device = device;
  
      acc_address = mag_address = (sa0 == sa0_high) ? D_SA0_HIGH_ADDRESS : D_SA0_LOW_ADDRESS;
      translated_regs[-OUT_X_L_M] = D_OUT_X_L_M;
      translated_regs[-OUT_X_H_M] = D_OUT_X_H_M;
      translated_regs[-OUT_Y_L_M] = D_OUT_Y_L_M;
      translated_regs[-OUT_Y_H_M] = D_OUT_Y_H_M;
      translated_regs[-OUT_Z_L_M] = D_OUT_Z_L_M;
      translated_regs[-OUT_Z_H_M] = D_OUT_Z_H_M;



  return true;
}

/*
Enables the LSM303's accelerometer and magnetometer. Also:
- Sets sensor full scales (gain) to default power-on values, which are
  +/- 2 g for accelerometer and +/- 1.3 gauss for magnetometer
  (+/- 4 gauss on LSM303D).
- Selects 50 Hz ODR (output data rate) for accelerometer and 7.5 Hz
  ODR for magnetometer (6.25 Hz on LSM303D). (These are the ODR
  settings for which the electrical characteristics are specified in
  the datasheets.)
- Enables high resolution modes (if available).
Note that this function will also reset other settings controlled by
the registers it writes to.
*/
void LSM303::enableDefault(void)
{

//  if (_device == device_D)
  {
    // Accelerometer

    // 0x00 = 0b00000000
    // AFS = 0 (+/- 2 g full scale)
    writeReg(CTRL2, 0x00);

    // 0x57 = 0b01010111
    // AODR = 0101 (50 Hz ODR); AZEN = AYEN = AXEN = 1 (all axes enabled)
    writeReg(CTRL1, 0x57);

    // Magnetometer

    // 0x64 = 0b01100100
    // M_RES = 11 (high resolution mode); M_ODR = 001 (6.25 Hz ODR)
    writeReg(CTRL5, 0x64);

    // 0x20 = 0b00100000
    // MFS = 01 (+/- 4 gauss full scale)
    writeReg(CTRL6, 0x20);

    // 0x00 = 0b00000000
    // MLP = 0 (low power mode off); MD = 00 (continuous-conversion mode)
    writeReg(CTRL7, 0x00);
  }
//  else  {  }
}

// Writes an accelerometer register
void LSM303::writeAccReg(byte reg, byte value)
{
  Wire.beginTransmission(acc_address);
  Wire.write(reg);
  Wire.write(value);
  last_status = Wire.endTransmission();
}

// Reads an accelerometer register
byte LSM303::readAccReg(byte reg)
{
  byte value;

  Wire.beginTransmission(acc_address);
  Wire.write(reg);
  last_status = Wire.endTransmission();
  Wire.requestFrom(acc_address, (byte)1);
  value = Wire.read();
  Wire.endTransmission();

  return value;
}

// Writes a magnetometer register
void LSM303::writeMagReg(byte reg, byte value)
{
  Wire.beginTransmission(mag_address);
  Wire.write(reg);
  Wire.write(value);
  last_status = Wire.endTransmission();
}

// Reads a magnetometer register
byte LSM303::readMagReg(int reg)
{
  byte value;

  // if dummy register address (magnetometer Y/Z), look up actual translated address (based on device type)
  if (reg < 0)
  {
    reg = translated_regs[-reg];
  }

  Wire.beginTransmission(mag_address);
  Wire.write(reg);
  last_status = Wire.endTransmission();
  Wire.requestFrom(mag_address, (byte)1);
  value = Wire.read();
  Wire.endTransmission();

  return value;
}

void LSM303::writeReg(byte reg, byte value)
{
  // mag address == acc_address for LSM303D, so it doesn't really matter which one we use.
  if (_device == device_D || reg < CTRL_REG1_A)
  {
    writeMagReg(reg, value);
  }
  else
  {
    writeAccReg(reg, value);
  }
}

// Note that this function will not work for reading TEMP_OUT_H_M and TEMP_OUT_L_M on the DLHC.
// To read those two registers, use readMagReg() instead.
byte LSM303::readReg(int reg)
{
  // mag address == acc_address for LSM303D, so it doesn't really matter which one we use.
  // Use readMagReg so it can translate OUT_[XYZ]_[HL]_M
  if (_device == device_D || reg < CTRL_REG1_A)
  {
    return readMagReg(reg);
  }
  else
  {
    return readAccReg(reg);
  }
}

// Reads the 3 accelerometer channels and stores them in vector a
byte LSM303::readAcc(void)
{
  Wire.beginTransmission(acc_address);
  // assert the MSB of the address to get the accelerometer
  // to do slave-transmit subaddress updating.
  Wire.write(OUT_X_L_A | (1 << 7));
  last_status = Wire.endTransmission();
  Wire.requestFrom(acc_address, (byte)6);
     byte _buff[8];

  int i = 0;
    while(Wire.available())         // device may send less than requested (abnormal)
    {
        _buff[i] = Wire.read();    // receive a byte
        i++;
        if (i>=6) break;
    }
    if(i != 6){
       i=0;
    }

   /*
  unsigned int millis_start = millis();
  while (Wire.available() < 6) {
    if (io_timeout > 0 && ((unsigned int)millis() - millis_start) > io_timeout)
    {
      did_timeout = true;
      return;
    }
  }

  byte xla = Wire.read();
  byte xha = Wire.read();
  byte yla = Wire.read();
  byte yha = Wire.read();
  byte zla = Wire.read();
  byte zha = Wire.read();

  // combine high and low bytes
  // This no longer drops the lowest 4 bits of the readings from the DLH/DLM/DLHC, which are always 0
  // (12-bit resolution, left-aligned). The D has 16-bit resolution
  a.x = (int16_t)(xha << 8 | xla);
  a.y = (int16_t)(yha << 8 | yla);
  a.z = (int16_t)(zha << 8 | zla);
  */

  a.x = (int16_t)(_buff[1] << 8 | _buff[0]);
  a.y = (int16_t)(_buff[3] << 8 | _buff[2]);
  a.z = (int16_t)(_buff[5] << 8 | _buff[4]);
 // a.x=_buff[0]+_buff[1];
 // a.y=_buff[3];
  return i;
}

// Reads the 3 magnetometer channels and stores them in vector m
void LSM303::readMag(void)
{
  Wire.beginTransmission(mag_address);
  // If LSM303D, assert MSB to enable subaddress updating
  // OUT_X_L_M comes first on D, OUT_X_H_M on others
  Wire.write((_device == device_D) ? translated_regs[-OUT_X_L_M] | (1 << 7) : translated_regs[-OUT_X_H_M]);
  last_status = Wire.endTransmission();
  Wire.requestFrom(mag_address, (byte)6);
     byte _buff[6];

  int i = 0;
    while(Wire.available())         // device may send less than requested (abnormal)
    {
        _buff[i] = Wire.read();    // receive a byte
        i++;
        if (i>=6) break;
    }
    if(i != 6){
       i=-i;
    }
  /*
  unsigned int millis_start = millis();
  while (Wire.available() < 6) {
    if (io_timeout > 0 && ((unsigned int)millis() - millis_start) > io_timeout)
    {
      did_timeout = true;
      return;
    }
  }

  byte xlm, xhm, ylm, yhm, zlm, zhm;

  if (_device == device_D)
  {
    // D: X_L, X_H, Y_L, Y_H, Z_L, Z_H
    xlm = Wire.read();
    xhm = Wire.read();
    ylm = Wire.read();
    yhm = Wire.read();
    zlm = Wire.read();
    zhm = Wire.read();
  }
  else
  {
    // DLHC, DLM, DLH: X_H, X_L...
    xhm = Wire.read();
    xlm = Wire.read();

    if (_device == device_DLH)
    {
      // DLH: ...Y_H, Y_L, Z_H, Z_L
      yhm = Wire.read();
      ylm = Wire.read();
      zhm = Wire.read();
      zlm = Wire.read();
    }
    else
    {
      // DLM, DLHC: ...Z_H, Z_L, Y_H, Y_L
      zhm = Wire.read();
      zlm = Wire.read();
      yhm = Wire.read();
      ylm = Wire.read();
    }
  }


  // combine high and low bytes
  m.x = (int16_t)(xhm << 8 | xlm);
  m.y = (int16_t)(yhm << 8 | ylm);
  m.z = (int16_t)(zhm << 8 | zlm);
  */
  m.x = (int16_t)(_buff[1] << 8 | _buff[0]);
  m.y = (int16_t)(_buff[3] << 8 | _buff[2]);
  m.z = (int16_t)(_buff[5] << 8 | _buff[4]);

}

// Reads all 6 channels of the LSM303 and stores them in the object variables
void LSM303::read(void)
{
  readAcc();
  readMag();
}



// Private Methods //////////////////////////////////////////////////////////////

int LSM303::testReg(byte address, regAddr reg)
{
  Wire.beginTransmission(address);
  Wire.write((byte)reg);
  if (Wire.endTransmission() != 0)
  {
    return TEST_REG_ERROR;
  }

  Wire.requestFrom(address, (byte)1);
  if (Wire.available())
  {
    return Wire.read();
  }
  else
  {
    return TEST_REG_ERROR;
  }
}
