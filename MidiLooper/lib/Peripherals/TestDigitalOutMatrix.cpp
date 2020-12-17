#include "TestDigitalOutMatrix.h"

void testDigitalOutMatrix(DigitalOutMatrix &matrix, int repeats)
{
    for (int repeat = 0; repeat < repeats || repeats < 0; ++repeat)
    {
        for (int row = 0; row < matrix.NumRows(); ++row)
        {
            for (int col = 0; col < matrix.NumCols(); ++col)
            {
                matrix.Set(row, col);
                matrix.WriteAll();
                delay(100);
                matrix.Clear(row, col);
                matrix.WriteAll();
                delay(100);
                //matrix.set(row, col);
            }
        }
    }
}
