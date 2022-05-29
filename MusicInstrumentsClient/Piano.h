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
	Piano(QWidget *parent = nullptr, int keyFirst = 48, int keyLast = 81)
		: QWidget(parent), mKeyFirst(keyFirst), mKeyLast(keyLast), mPixmapLabel(this)
	{
		calcKeyRects();
	}		


protected:
	void paintEvent(QPaintEvent* event) override;

private:

	static const int whiteWidth = 46;
	static const int whiteHeight = 290;
	static const int blackWidth = 30;
	static const int blackHeight = 170;
	static const int keysCount = 128;

	static bool isBlack(int n);

	void calcKeyRects();

	int mKeyFirst;
	int mKeyLast;

	QLabel mPixmapLabel;
	QPixmap mPianoPixmap;
	QVector<PianoKey> mPianoKeys{ 128 };
	//QVector<QRect> mKeyRects{keysCount, {0, 0, 0, 0}};
	//QVector<bool> mKeyStates = QVector<bool>(keysCount, false);
};

