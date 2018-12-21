#pragma once

#include <mbed.h>//uint32_t

namespace euclidian
{

const int MaxPatternLength = 16;

// patternoffset[fills] with fills in [1,MaxPatternLength]
const int EuclidianPatterOffsets[] = {0,0,2,5,9,14,20,27,35,44,54,65,77,90,104,119,135};

const uint32_t EuclidianPatterns[] = 
{
   0x0000, // 0: 0 out of 1 -> 0
   0x0001, // 1: 1 out of 1 -> 1
   0x0000, // 2: 0 out of 2 -> 00
   0x0002, // 3: 1 out of 2 -> 10
   0x0003, // 4: 2 out of 2 -> 11
   0x0000, // 5: 0 out of 3 -> 000
   0x0004, // 6: 1 out of 3 -> 100
   0x0005, // 7: 2 out of 3 -> 101
   0x0007, // 8: 3 out of 3 -> 111
   0x0000, // 9: 0 out of 4 -> 0000
   0x0008, // 10: 1 out of 4 -> 1000
   0x000a, // 11: 2 out of 4 -> 1010
   0x000b, // 12: 3 out of 4 -> 1011
   0x000f, // 13: 4 out of 4 -> 1111
   0x0000, // 14: 0 out of 5 -> 00000
   0x0010, // 15: 1 out of 5 -> 10000
   0x0014, // 16: 2 out of 5 -> 10100
   0x0015, // 17: 3 out of 5 -> 10101
   0x0017, // 18: 4 out of 5 -> 10111
   0x001f, // 19: 5 out of 5 -> 11111
   0x0000, // 20: 0 out of 6 -> 000000
   0x0020, // 21: 1 out of 6 -> 100000
   0x0024, // 22: 2 out of 6 -> 100100
   0x002a, // 23: 3 out of 6 -> 101010
   0x002d, // 24: 4 out of 6 -> 101101
   0x002f, // 25: 5 out of 6 -> 101111
   0x003f, // 26: 6 out of 6 -> 111111
   0x0000, // 27: 0 out of 7 -> 0000000
   0x0040, // 28: 1 out of 7 -> 1000000
   0x0048, // 29: 2 out of 7 -> 1001000
   0x0054, // 30: 3 out of 7 -> 1010100
   0x0055, // 31: 4 out of 7 -> 1010101
   0x005b, // 32: 5 out of 7 -> 1011011
   0x005f, // 33: 6 out of 7 -> 1011111
   0x007f, // 34: 7 out of 7 -> 1111111
   0x0000, // 35: 0 out of 8 -> 00000000
   0x0080, // 36: 1 out of 8 -> 10000000
   0x0088, // 37: 2 out of 8 -> 10001000
   0x0092, // 38: 3 out of 8 -> 10010010
   0x00aa, // 39: 4 out of 8 -> 10101010
   0x00b6, // 40: 5 out of 8 -> 10110110
   0x00bb, // 41: 6 out of 8 -> 10111011
   0x00bf, // 42: 7 out of 8 -> 10111111
   0x00ff, // 43: 8 out of 8 -> 11111111
   0x0000, // 44: 0 out of 9 -> 000000000
   0x0100, // 45: 1 out of 9 -> 100000000
   0x0110, // 46: 2 out of 9 -> 100010000
   0x0124, // 47: 3 out of 9 -> 100100100
   0x0154, // 48: 4 out of 9 -> 101010100
   0x0155, // 49: 5 out of 9 -> 101010101
   0x016d, // 50: 6 out of 9 -> 101101101
   0x0177, // 51: 7 out of 9 -> 101110111
   0x017f, // 52: 8 out of 9 -> 101111111
   0x01ff, // 53: 9 out of 9 -> 111111111
   0x0000, // 54: 0 out of 10 -> 0000000000
   0x0200, // 55: 1 out of 10 -> 1000000000
   0x0210, // 56: 2 out of 10 -> 1000010000
   0x0248, // 57: 3 out of 10 -> 1001001000
   0x0252, // 58: 4 out of 10 -> 1001010010
   0x02aa, // 59: 5 out of 10 -> 1010101010
   0x02d6, // 60: 6 out of 10 -> 1011010110
   0x02db, // 61: 7 out of 10 -> 1011011011
   0x02f7, // 62: 8 out of 10 -> 1011110111
   0x02ff, // 63: 9 out of 10 -> 1011111111
   0x03ff, // 64: 10 out of 10 -> 1111111111
   0x0000, // 65: 0 out of 11 -> 00000000000
   0x0400, // 66: 1 out of 11 -> 10000000000
   0x0420, // 67: 2 out of 11 -> 10000100000
   0x0444, // 68: 3 out of 11 -> 10001000100
   0x0492, // 69: 4 out of 11 -> 10010010010
   0x0554, // 70: 5 out of 11 -> 10101010100
   0x0555, // 71: 6 out of 11 -> 10101010101
   0x05b6, // 72: 7 out of 11 -> 10110110110
   0x05dd, // 73: 8 out of 11 -> 10111011101
   0x05ef, // 74: 9 out of 11 -> 10111101111
   0x05ff, // 75: 10 out of 11 -> 10111111111
   0x07ff, // 76: 11 out of 11 -> 11111111111
   0x0000, // 77: 0 out of 12 -> 000000000000
   0x0800, // 78: 1 out of 12 -> 100000000000
   0x0820, // 79: 2 out of 12 -> 100000100000
   0x0888, // 80: 3 out of 12 -> 100010001000
   0x0924, // 81: 4 out of 12 -> 100100100100
   0x094a, // 82: 5 out of 12 -> 100101001010
   0x0aaa, // 83: 6 out of 12 -> 101010101010
   0x0b5a, // 84: 7 out of 12 -> 101101011010
   0x0b6d, // 85: 8 out of 12 -> 101101101101
   0x0bbb, // 86: 9 out of 12 -> 101110111011
   0x0bef, // 87: 10 out of 12 -> 101111101111
   0x0bff, // 88: 11 out of 12 -> 101111111111
   0x0fff, // 89: 12 out of 12 -> 111111111111
   0x0000, // 90: 0 out of 13 -> 0000000000000
   0x1000, // 91: 1 out of 13 -> 1000000000000
   0x1040, // 92: 2 out of 13 -> 1000001000000
   0x1110, // 93: 3 out of 13 -> 1000100010000
   0x1248, // 94: 4 out of 13 -> 1001001001000
   0x1294, // 95: 5 out of 13 -> 1001010010100
   0x1554, // 96: 6 out of 13 -> 1010101010100
   0x1555, // 97: 7 out of 13 -> 1010101010101
   0x16b5, // 98: 8 out of 13 -> 1011010110101
   0x16db, // 99: 9 out of 13 -> 1011011011011
   0x1777, // 100: 10 out of 13 -> 1011101110111
   0x17df, // 101: 11 out of 13 -> 1011111011111
   0x17ff, // 102: 12 out of 13 -> 1011111111111
   0x1fff, // 103: 13 out of 13 -> 1111111111111
   0x0000, // 104: 0 out of 14 -> 00000000000000
   0x2000, // 105: 1 out of 14 -> 10000000000000
   0x2040, // 106: 2 out of 14 -> 10000001000000
   0x2108, // 107: 3 out of 14 -> 10000100001000
   0x2244, // 108: 4 out of 14 -> 10001001000100
   0x2492, // 109: 5 out of 14 -> 10010010010010
   0x254a, // 110: 6 out of 14 -> 10010101001010
   0x2aaa, // 111: 7 out of 14 -> 10101010101010
   0x2d5a, // 112: 8 out of 14 -> 10110101011010
   0x2db6, // 113: 9 out of 14 -> 10110110110110
   0x2edd, // 114: 10 out of 14 -> 10111011011101
   0x2f7b, // 115: 11 out of 14 -> 10111101111011
   0x2fdf, // 116: 12 out of 14 -> 10111111011111
   0x2fff, // 117: 13 out of 14 -> 10111111111111
   0x3fff, // 118: 14 out of 14 -> 11111111111111
   0x0000, // 119: 0 out of 15 -> 000000000000000
   0x4000, // 120: 1 out of 15 -> 100000000000000
   0x4080, // 121: 2 out of 15 -> 100000010000000
   0x4210, // 122: 3 out of 15 -> 100001000010000
   0x4444, // 123: 4 out of 15 -> 100010001000100
   0x4924, // 124: 5 out of 15 -> 100100100100100
   0x4a52, // 125: 6 out of 15 -> 100101001010010
   0x5554, // 126: 7 out of 15 -> 101010101010100
   0x5555, // 127: 8 out of 15 -> 101010101010101
   0x5ad6, // 128: 9 out of 15 -> 101101011010110
   0x5b6d, // 129: 10 out of 15 -> 101101101101101
   0x5ddd, // 130: 11 out of 15 -> 101110111011101
   0x5ef7, // 131: 12 out of 15 -> 101111011110111
   0x5fbf, // 132: 13 out of 15 -> 101111110111111
   0x5fff, // 133: 14 out of 15 -> 101111111111111
   0x7fff, // 134: 15 out of 15 -> 111111111111111
   0x0000, // 135: 0 out of 16 -> 0000000000000000
   0x8000, // 136: 1 out of 16 -> 1000000000000000
   0x8080, // 137: 2 out of 16 -> 1000000010000000
   0x8420, // 138: 3 out of 16 -> 1000010000100000
   0x8888, // 139: 4 out of 16 -> 1000100010001000
   0x9248, // 140: 5 out of 16 -> 1001001001001000
   0x9494, // 141: 6 out of 16 -> 1001010010010100
   0x952a, // 142: 7 out of 16 -> 1001010100101010
   0xaaaa, // 143: 8 out of 16 -> 1010101010101010
   0xb56a, // 144: 9 out of 16 -> 1011010101101010
   0xb5b5, // 145: 10 out of 16 -> 1011010110110101
   0xb6db, // 146: 11 out of 16 -> 1011011011011011
   0xbbbb, // 147: 12 out of 16 -> 1011101110111011
   0xbdef, // 148: 13 out of 16 -> 1011110111101111
   0xbfbf, // 149: 14 out of 16 -> 1011111110111111
   0xbfff, // 150: 15 out of 16 -> 1011111111111111
   0xffff // 151: 16 out of 16 -> 1111111111111111
};

}

