#include "Piano.h"

bool Piano::isBlack(int n)
{
    // 0x54A - black keys
    // 12 - keys period
    return 0x54A & (1 << (n % 12));
}
