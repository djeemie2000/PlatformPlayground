#include "Max7219Matrix.h"
#include "BitWise.h"

Max7219Matrix::Device::Device()
{
    for(int idx = 0; idx<Size; ++idx)
    {
        m_Digit[idx] = 0x00;
    }
}

void Max7219Matrix::Device::Set(uint8_t row, uint8_t col)
{
    BitSet(m_Digit[row], col);
}

void Max7219Matrix::Device::Clear(uint8_t row, uint8_t col)
{
    BitClear(m_Digit[row], col);
}

Max7219Matrix::Max7219Matrix(int numDevices,PinName mosi, PinName miso, PinName sclk, PinName cs)
 : m_NumDevices(numDevices)
 , m_LedMatrix(mosi, miso, sclk, cs)
{
}

void Max7219Matrix::Configure()
{
  max7219_configuration_t cfg = {
         .device_number = 1,
         .decode_mode = 0,
         .intensity = Max7219::MAX7219_INTENSITY_1,
         .scan_limit = Max7219::MAX7219_SCAN_8
     };
  m_LedMatrix.set_num_devices(m_NumDevices);
  m_LedMatrix.init_display(cfg);//init all devices!
  m_LedMatrix.enable_display();//enable all devices
}

void Max7219Matrix::Test()
{
  //test
  m_LedMatrix.set_display_test();
  wait_ms(500);
  m_LedMatrix.clear_display_test();
  m_LedMatrix.display_all_off();
  wait_ms(1000);
}

void Max7219Matrix::Set(int row, int col)
{
    int device = row/Size;
    int r = row;
    int c = col-device*Size;
    m_Device[device].Set(c,r);//rotated
}

void Max7219Matrix::Clear(int row, int col)
{
    int device = row/Size;
    int r = row;
    int c = col-device*Size;
    m_Device[device].Clear(c,r);//rotated
}

void Max7219Matrix::Write()
{
    for(int row =0; row<Size; ++row)
    {
        Write(row);
    }
}

void Max7219Matrix::Write(int row)
{
    if(0<m_NumDevices)
    {
        m_LedMatrix.write_digit(1, 1+row, m_Device[0].m_Digit[row]);
    }
    if(1<m_NumDevices)
    {
        m_LedMatrix.write_digit(2, 1+row, m_Device[1].m_Digit[row]);
    }
    if(2<m_NumDevices)
    {
        m_LedMatrix.write_digit(3, 1+row, m_Device[2].m_Digit[row]);
    }
    if(3<m_NumDevices)
    {
        m_LedMatrix.write_digit(4, 1+row, m_Device[3].m_Digit[row]);
    }
}
