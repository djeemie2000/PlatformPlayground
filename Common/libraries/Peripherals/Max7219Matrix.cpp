#include "Max7219Matrix.h"
#include "BitWise.h"

Max7219Matrix::Device::Device()
{
    for(int idx = 0; idx<Size; ++idx)
    {
        m_Digit[idx] = 0x00;
    }
}

bool Max7219Matrix::Device::Set(uint8_t row, uint8_t col)
{
    if(row<Size && col<Size)
    {
        BitSet(m_Digit[row], col);
        return true;
    }
    return false;
}

bool Max7219Matrix::Device::Clear(uint8_t row, uint8_t col)
{
    if(row<Size && col<Size)
    {
        BitClear(m_Digit[row], col);
        return true;
    }
    return false;
}

Max7219Matrix::Max7219Matrix(int numDevices, SPI* spi, PinName cs)
 : m_NumDevices(numDevices)
 , m_LedMatrix(spi, cs)
 , m_Device()
 , m_Rotated(false)
{
    spi->format(8,0);
}

void Max7219Matrix::Configure(bool rotated)
{
    m_Rotated =rotated;
  m_LedMatrix.set_num_devices(m_NumDevices);
  m_LedMatrix.disable_display();//disable all devices
   wait_us(5); 

  max7219_configuration_t cfg = {
         .device_number = 1,
         .decode_mode = 0,
         .intensity = Max7219::MAX7219_INTENSITY_0,
         .scan_limit = Max7219::MAX7219_SCAN_8
     };
  m_LedMatrix.set_num_devices(m_NumDevices);
  m_LedMatrix.init_display(cfg);//init all devices!
   wait_us(5); 
  m_LedMatrix.enable_display();//enable all devices
   wait_us(5); 
}

void Max7219Matrix::Test()
{
  //test
  m_LedMatrix.set_display_test();
  wait_us(500000);
  m_LedMatrix.clear_display_test();
  wait_us(500000);
  m_LedMatrix.display_all_off();
  wait_us(1000000);
}

bool Max7219Matrix::Set(int row, int col)
{
    int device = col/Size;
    if(0<=device && device<m_NumDevices)
    {
        if(m_Rotated)
        {
            int r = row;
            int c = col-device*Size;
            return m_Device[device].Set(c,r);//rotated
        }
        else
        {
            int r = Size-1-row;
            int c = col-device*Size;
            return m_Device[device].Set(r,c);//not rotated
        }
    }
    return false;
}

bool Max7219Matrix::Clear(int row, int col)
{
    int device = col/Size;
    if(0<=device && device<m_NumDevices)
    {
        if(m_Rotated)
        {
            int r = row;
            int c = col-device*Size;
            return m_Device[device].Clear(c,r);//rotated
        }
        else
        {
            int r = Size-1-row;
            int c = col-device*Size;
            return m_Device[device].Clear(r,c);//not rotated
        }
    }
    return false;
}

int Max7219Matrix::Write()
{
    int retval = 0;
    for(int row =0; row<Size; ++row)
    {
        retval += Write(row);
    }
    return retval;
}

int Max7219Matrix::Write(int row)
{
    int retval = 0;
    for(int device = 0; device<m_NumDevices; ++device)
    {
        retval += m_LedMatrix.write_digit(1+device, 1+row, m_Device[device].m_Digit[row]);
    }
    return retval;
    // if(0<m_NumDevices)
    // {
    //     m_LedMatrix.write_digit(1, 1+row, m_Device[0].m_Digit[row]);
    // }
    // if(1<m_NumDevices)
    // {
    //     m_LedMatrix.write_digit(2, 1+row, m_Device[1].m_Digit[row]);
    // }
    // if(2<m_NumDevices)
    // {
    //     m_LedMatrix.write_digit(3, 1+row, m_Device[2].m_Digit[row]);
    // }
    // if(3<m_NumDevices)
    // {
    //     m_LedMatrix.write_digit(4, 1+row, m_Device[3].m_Digit[row]);
    // }
}

void TestExtended(Max7219Matrix& matrix, Serial& pc)
{
    for(int repeat = 0; repeat<2; ++repeat)
    {
        for(int row = 0; row<matrix.NumRows();++row)
        {
            for(int col = 0; col<matrix.NumCols(); ++col)
            {
                bool ok = true;
                bool clear = repeat%2;
                if(!clear)
                {
                    ok = matrix.Set(row, col);
                }
                else
                {
                    ok = matrix.Clear(row, col);
                }
                int retval = matrix.Write();
                if(clear)
                {
                    pc.printf("clr r %d c %d r -> %d write %d\r\n", row, col, ok?1:0, retval);
                }
                else
                {
                    pc.printf("set r %d c %d r -> %d wirte %d\r\n", row, col, ok?1:0, retval);
                }
                wait_us(100000);
            }
        }
    }
}
