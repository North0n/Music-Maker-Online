#include "Piano.h"

#include <QPainter>
#include <QPen>
#include <QBrush>
#include <qdebug.h>
#include <QtGlobal>

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
