
#include "fxpt.h"


const float fxpt::s_sine_table[1 << NBITS][2] = {
#include "sine_table.h"
};

const float fxpt::PI = 3.14159265358979323846;
const float fxpt::TAU = 2.0 * 3.14159265358979323846;
const float fxpt::TWO_TO_THE_31 = 2147483648.0;

