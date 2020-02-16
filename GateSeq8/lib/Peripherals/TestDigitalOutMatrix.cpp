#include "TestDigitalOutMatrix.h"
#include "DigitalOutMatrix.h"
#include "../IO/SerialOut.h"

void TestDigitalOutMatrix(DigitalOutMatrix& matrix, SerialOut& debugSerial)
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
            delay(300);
            matrix.Clear(row, col);
        }
        debugSerial.println(" done");
    }
    debugSerial.println("Testing led matrix ended");
}
