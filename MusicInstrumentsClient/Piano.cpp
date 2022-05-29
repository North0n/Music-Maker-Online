#include "Piano.h"

#include <QPainter>
#include <QPen>
#include <QBrush>
#include <qdebug.h>
#include <QtGlobal>

Piano::Piano(QWidget* parent, int keyFirst)
    : QWidget(parent), mKeyFirst(keyFirst), mPixmapLabel(this)
{
    calcKeyRects();
    midiOutOpen(&hMidiOut, MIDI_MAPPER, NULL, NULL, CALLBACK_NULL);
}

Piano::~Piano()
{
    midiOutClose(hMidiOut);
}

void Piano::paintEvent(QPaintEvent* event)
{
    static QPen penBlack(Qt::black);
    static QBrush brushBlack(Qt::black), brushWhite(Qt::white), brushPressed(Qt::gray);

    mPianoPixmap = QPixmap(size().width(), qMin(size().height(), WhiteHeight) + 1);
    mPixmapLabel.setFixedSize(mPianoPixmap.size());

    int keyFirst = mKeyFirst - static_cast<int>(isBlack(mKeyFirst));

    QPainter painter(&mPianoPixmap);
    painter.setPen(penBlack);
    // White keys are drawn first, then black, because they overlap
    for (int color = 0; color < 2; ++color) {
        for (int key = keyFirst; key < KeysCount && mPianoKeys[key].rect().left() <= geometry().right(); ++key) {
            if (!mPianoKeys[key].isBlack() ^ color) {
                if (mPianoKeys[key].isPressed()) {
                    painter.setBrush(brushPressed);
                } else {
                    painter.setBrush(color ? brushBlack : brushWhite);
                }
                painter.drawRect(mPianoKeys[key].rect());
            }
        }
    }
    mPixmapLabel.setPixmap(mPianoPixmap);
}

void Piano::mousePressEvent(QMouseEvent* event)
{
    int key = pointToKey(event->pos());
    if (key == -1)
        return;

    mKeyMouse = key;
    noteOn();
}

void Piano::mouseReleaseEvent(QMouseEvent* event)
{
    if (mKeyMouse == -1)
        return;

    noteOff();
}

bool Piano::isBlack(int n)
{
    // 0x54A = 010101001010b - black keys
    // 12 - keys period
    return 0x54A & (1 << (n % 12));
}

void Piano::calcKeyRects()
{
    QRect whiteKey(0, 0, WhiteWidth, WhiteHeight);
    QRect blackKey(-BlackWidth / 2, 0, BlackWidth, BlackHeight);

    int keyFirst = mKeyFirst - static_cast<int>(isBlack(mKeyFirst));

    for (int key = keyFirst; key < KeysCount; ++key) {
        if (isBlack(key)) {
            mPianoKeys[key] = PianoKey(blackKey, true);
        } else {
            mPianoKeys[key] = PianoKey(whiteKey);
            whiteKey.translate(WhiteWidth, 0);
            blackKey.translate(WhiteWidth, 0);
        }
    }
}

int Piano::pointToKey(const QPoint& point) const
{
    for (int color = 0; color < 2; ++color) {
        for (int key = 0; key < KeysCount; ++key) {
            if (mPianoKeys[key].isBlack() ^ static_cast<bool>(color)) {
                if (mPianoKeys[key].rect().contains(point))
                    return key;
            }
        }
    }

    return -1;
}

void Piano::noteOn()
{
    mPianoKeys[mKeyMouse].setPressed(true);
    if (!isConnected) {
        midiOutShortMsg(hMidiOut, 0x000090 | (mNoteVelocity << 16) | (mKeyMouse << 8) | mMidiChannel);
    }
}

void Piano::noteOff()
{
    mPianoKeys[mKeyMouse].setPressed(false);
    if (!isConnected) {
        midiOutShortMsg(hMidiOut, 0x000080 | (mNoteVelocity << 16) | (mKeyMouse << 8) | mMidiChannel);
    }
}
