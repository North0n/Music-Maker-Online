#pragma once
#include "PianoKey.h"

#include <QWidget>
#include <QLabel>
#include <QPixmap>
#include <QVector>
#include <QRect>
#include <QPaintEvent>
#include <QPoint>
#include <QMouseEvent>

#include "Windows.h"
#include "mmeapi.h"

class Piano : public QWidget
{
	Q_OBJECT
public:
	Piano(QWidget* parent = nullptr, int keyFirst = 48);
	~Piano();

protected:
	void paintEvent(QPaintEvent* event) override;

	void mousePressEvent(QMouseEvent* event) override;

	void mouseReleaseEvent(QMouseEvent* event) override;

private:

	// Static configuration fields and methods
	static const int WhiteWidth = 46;
	static const int WhiteHeight = 290;
	static const int BlackWidth = 30;
	static const int BlackHeight = 170;
	static const int KeysCount = 128;

	static bool isBlack(int n);

	// General purpose methods
	void calcKeyRects();

	int pointToKey(const QPoint& point) const;

	// Audio handling methods
	void noteOn();

	void noteOff();

	// General purpose fields
	QLabel mPixmapLabel;
	QPixmap mPianoPixmap;
	QVector<PianoKey> mPianoKeys{ KeysCount };
	bool isConnected = false;

	int mKeyFirst;
	int mKeyMouse = -1;

	// Music fields
	HMIDIOUT hMidiOut;
	quint8 mNoteVelocity = 0x7F; // (0 - 127)
	quint8 mMidiChannel = 0;
};

