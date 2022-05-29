#pragma once
#include <qwidget.h>
class Piano : public QWidget
{
	Q_OBJECT
public:
	Piano(int keyFirst = 48, int keyLast = 81, QWidget *parent = nullptr) 
		: QWidget(parent), mKeyFirst(keyFirst), mKeyLast(keyLast)
	{}

private:

	static const int whiteWidth = 46;
	static const int whiteHeight = 290;
	static const int blackWidth = 30;
	static const int blackHeight = 170;

	static bool isBlack(int n);

	int mKeyFirst = 48;
	int mKeyLast = 81;
};

