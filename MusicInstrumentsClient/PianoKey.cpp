#include "PianoKey.h"

void PianoKey::setPressed(bool pressed)
{
    mIsPressed = pressed;
}

QRect PianoKey::rect() const
{
    return mKeyRect;
}

bool PianoKey::isPressed() const
{
    return mIsPressed;
}

bool PianoKey::isBlack() const
{
    return mIsBlack;
}
