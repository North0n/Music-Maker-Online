#include "Piano.h"

#include <QPainter>
#include <QPen>
#include <QBrush>
#include <qdebug.h>

void Piano::paintEvent(QPaintEvent* event)
{
    static QPen penBlack(Qt::black);
    static QBrush brushBlack(Qt::black), brushWhite(Qt::white), brushPressed(Qt::gray);

    mPianoPixmap = QPixmap(size());
    mPixmapLabel.setFixedSize(size());

    int keyFirst = mKeyFirst - static_cast<int>(isBlack(mKeyFirst));
    int keyLast = mKeyLast + static_cast<int>(isBlack(mKeyLast));

    QPainter painter(&mPianoPixmap);
    painter.setPen(penBlack);
    // White keys are drawn first, then black, because they overlap
    for (int color = 0; color < 2; ++color) {
        for (int key = keyFirst; key <= keyLast; ++key) {
            if (!isBlack(key) ^ color) {
                if (mKeyStates[key]) {
                    painter.setBrush(brushPressed);
                } else {
                    painter.setBrush(color ? brushBlack : brushWhite);
                }
                painter.drawRect(mKeyRects[key]);
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
    int keyFirst = mKeyFirst - static_cast<int>(isBlack(mKeyFirst));
    int keyLast = mKeyLast + static_cast<int>(isBlack(mKeyLast));

    QRect whiteKey(0, 0, whiteWidth, whiteHeight);
    QRect blackKey(-blackWidth / 2, 0, blackWidth, blackHeight);

    mKeyRects.reserve(keyLast - keyFirst + 1);
    mKeyStates.reserve(keyLast - keyFirst + 1);
    for (int key = keyFirst; key <= keyLast; ++key) {
        if (isBlack(key)) {
            mKeyRects[key] = blackKey;
        } else {
            mKeyRects[key] = whiteKey;
            whiteKey.translate(whiteWidth, 0);
            blackKey.translate(whiteWidth, 0);
        }
    }
}
