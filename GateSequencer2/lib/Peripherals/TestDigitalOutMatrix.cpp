#include "TestDigitalOutMatrix.h"
#include "DigitalOutMatrix.h"
#include "SerialOut.h"

#include "Max7219Matrix.h"

void TestDigitalOutMatrix(DigitalOutMatrix& matrix, SerialOut& debugSerial, int period)
{
    debugSerial.println("Testing led matrix...");
    for(int row = 0; row<matrix.NumRows(); ++row)
    {
        debugSerial.printf("Row %d...", row);
        for(int col = 0; col<matrix.NumCols(); ++col)
        {
            debugSerial.print("x");
            matrix.Set(row, col);
            matrix.WriteAll();
            delay(period);
            matrix.Clear(row, col);
        }
        debugSerial.println(" done");
    }
    debugSerial.println("Testing led matrix ended");
}

// void TestDigitalOutMatrixFast(Max7219Matrix& matrix, SerialOut& debugSerial, int period)
// {
//     debugSerial.println("Testing led matrix fast...");
//     for(int row = 0; row<matrix.NumRows(); ++row)
//     {
//         debugSerial.printf("Row %d...", row);
//         for(int col = 0; col<8; ++col)
//         {
//             debugSerial.print("x");
//             matrix.WriteColFast(col, 0xFF, 0xEE, 0xDD, 0xCC);
//             delay(period);
//             matrix.WriteColFast(col, 0x00, 0x00, 0x00, 0x00);
//         }
//         debugSerial.println(" done");
//     }
//     debugSerial.println("Testing led matrix fast ended");
// }
