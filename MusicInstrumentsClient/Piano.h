#pragma once
#include <QWidget>
#include <QLabel>
#include <QPixmap>
#include <QVector>
#include <QRect>
#include <QPaintEvent>
#include "PianoKey.h"


class Piano : public QWidget
{
	Q_OBJECT
public:
	Piano(QWidget *parent = nullptr, int keyFirst = 48)
		: QWidget(parent), mKeyFirst(keyFirst), mPixmapLabel(this)
	{
		calcKeyRects();
	}		


protected:
	void paintEvent(QPaintEvent* event) override;

private:

	static const int WhiteWidth = 46;
	static const int WhiteHeight = 290;
	static const int BlackWidth = 30;
	static const int BlackHeight = 170;
	static const int KeysCount = 128;

	static bool isBlack(int n);

	void calcKeyRects();



	int mKeyFirst;

	QLabel mPixmapLabel;
	QPixmap mPianoPixmap;
	QVector<PianoKey> mPianoKeys{ 128 };
};

