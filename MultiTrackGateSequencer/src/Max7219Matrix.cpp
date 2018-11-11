#include "Max7219Matrix.h"
#include "BitWise.h"

Max7219Matrix::Max7219Matrix(int numDevices,PinName mosi, PinName miso, PinName sclk, PinName cs)
 : m_NumDevices(numDevices)
 , m_LedMatrix(mosi, miso, sclk, cs)
{
    for(int idx = 0; idx<Size; ++idx)
    {
        m_Pattern[idx] = 0;
    }
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
    SetInternal(col%Size, row+(col/Size)*Size);
}

void Max7219Matrix::Clear(int row, int col)
{
    ClearInternal(col%Size, row+(col/Size)*Size);
}

void Max7219Matrix::SetInternal(int row, int col) 
{
    //no check on row, col
    uint32_t c = col;
    BitSet(m_Pattern[row],c);
}

void Max7219Matrix::ClearInternal(int row, int col) 
{
    //no check on row, col
    uint32_t c = col;
    BitClear(m_Pattern[row],c);
}

void Max7219Matrix::Write()
{
    for(uint8_t row =1; row<=Size; ++row)
    {
        if(0<m_NumDevices)
        {
            m_LedMatrix.write_digit(1, row, m_Pattern[row]&0xFF);
        }
        if(1<m_NumDevices)
        {
            m_LedMatrix.write_digit(2, row, (m_Pattern[row]>>8)&0xFF);
        }
        if(2<m_NumDevices)
        {
            m_LedMatrix.write_digit(3, row, (m_Pattern[row]>>16)&0xFF);
        }
        if(3<m_NumDevices)
        {
            m_LedMatrix.write_digit(4, row, (m_Pattern[row]>>24)&0xFF);
        }
    }
}

void Max7219Matrix::Write(int row)
{
    if(0<m_NumDevices)
    {
        m_LedMatrix.write_digit(1, 1+row, m_Pattern[row]&0xFF);
    }
    if(1<m_NumDevices)
    {
        m_LedMatrix.write_digit(2, 1+row, (m_Pattern[row]>>8)&0xFF);
    }
    if(2<m_NumDevices)
    {
        m_LedMatrix.write_digit(3, 1+row, (m_Pattern[row]>>16)&0xFF);
    }
    if(3<m_NumDevices)
    {
        m_LedMatrix.write_digit(4, 1+row, (m_Pattern[row]>>24)&0xFF);
    }
}
