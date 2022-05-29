#pragma once

#include <QRect>

class PianoKey
{
public:

	PianoKey(QRect keyRect, bool black = false, bool pressed = false)
		: mKeyRect(keyRect), mIsBlack(black), mIsPressed(pressed)
	{}

	// Setters
	void setPressed(bool pressed);

	// Getters
	QRect rect() const;

	bool isPressed() const;

	bool isBlack() const;

private:
	QRect mKeyRect;
	bool mIsPressed;
	bool mIsBlack;
};

