#include "Piano.h"

#include <QPainter>
#include <QPen>
#include <QBrush>
#include <qdebug.h>
#include <QtGlobal>
#include <QMessageBox>

Piano::Piano(QWidget* parent, int keyFirst)
    : QWidget(parent), mKeyFirst(keyFirst), mPixmapLabel(this)
{
    calcKeyRects();
    midiOutOpen(&hMidiOut, MIDI_MAPPER, NULL, NULL, CALLBACK_NULL);
    setFocus();
}

Piano::~Piano()
{
    midiOutClose(hMidiOut);
}

void Piano::connectToServer(const QHostAddress& host, quint16 port)
{
    mServerSocket = std::make_unique<ServerSocket>(mPort, host, port, this);
    
    // Send a message to notify server about our connection
    connect(mServerSocket.get(), &ServerSocket::dataReceived, this, &Piano::getMsgFromServer);
    mServerSocket->establishConnection(mMidiInstrument);
}

void Piano::createRoom(const QHostAddress& host, quint16 port)
{
    auto* const connection = new QMetaObject::Connection;

    auto setRoomPort = [this, host, port, connection](QByteArray bytes)
    {
        quint16 port;
        QDataStream in(&bytes, QIODevice::ReadOnly);

        in >> port >> mMaxServerDowntime;
        mServerSocket->setServerPort(port);
        QMessageBox::information(this, "Комната создана", QString("Комната создана по адресу ") + host.toString() + ":" + QString::number(port));

        QObject::disconnect(*connection);
        delete connection;
        connect(mServerSocket.get(), &ServerSocket::dataReceived, this, &Piano::getMsgFromServer);
        mServerSocket->establishConnection(mMidiInstrument);
    };

    mServerSocket = std::make_unique<ServerSocket>(mPort, host, port, this);

    mServerSocket->establishConnection(mMidiInstrument);
    *connection = connect(mServerSocket.get(), &ServerSocket::dataReceived, setRoomPort);
}

void Piano::disconnectFormServer()
{
    mServerSocket.reset(nullptr);
}

void Piano::getMsgFromServer(QByteArray bytes)
{
    QDataStream data(&bytes, QIODevice::ReadOnly);
    quint32 msg;
    while (!data.atEnd()) {
        data >> msg;
        if (!mIsMuted || ((msg & 0xF0) == 0xC0)) {
            midiOutShortMsg(hMidiOut, msg);
        }
    }
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
    if (key == NoKey)
        return;

    mKeyMouse = key;
    noteOn(mKeyMouse, mMidiChannel);
}

void Piano::mouseReleaseEvent(QMouseEvent* event)
{
    if (mKeyMouse == NoKey)
        return;

    noteOff(mKeyMouse, mMidiChannel);
}

void Piano::keyPressEvent(QKeyEvent* event)
{
    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_M)
        mIsMuted = !mIsMuted;

    if (event->isAutoRepeat())
        return;
    if (event->key() <= 0 || event->key() >= 256) {
        QWidget::keyPressEvent(event);
        return;
    }

    int key = KeyToNote[event->key()];
    if (key == NoKey) {
        QWidget::keyPressEvent(event);
        return;
    }
    noteOn(key, mMidiChannel);
}

void Piano::keyReleaseEvent(QKeyEvent* event)
{
    if (event->isAutoRepeat())
        return;
    if (event->key() <= 0 || event->key() >= 256) {
        QWidget::keyReleaseEvent(event);
        return;
    }

    int key = KeyToNote[event->key()];
    if (key == NoKey) {
        QWidget::keyReleaseEvent(event);
        return;
    }
    noteOff(key, mMidiChannel);
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

    return NoKey;
}

void Piano::noteOn(int note, int channel)
{
    mPianoKeys[note].setPressed(true);
    quint32 msg = 0x000090 | (mNoteVelocity << 16) | (note << 8) | channel;
    if (!isConnected() && !mIsMuted) {
        midiOutShortMsg(hMidiOut, msg);
    } else if (isConnected()) {
        mServerSocket->sendShortMsg(msg);
    }
}

void Piano::noteOff(int note, int channel)
{
    mPianoKeys[note].setPressed(false);
    quint32 msg = 0x000080 | (mNoteVelocity << 16) | (note << 8) | channel;
    if (!isConnected() && !mIsMuted) {
        midiOutShortMsg(hMidiOut, 0x000080 | (mNoteVelocity << 16) | (note << 8) | channel);
    } else if (isConnected()) {
        mServerSocket->sendShortMsg(msg);
    }
}

void Piano::changeInstrument(int index)
{
    mMidiInstrument = index;
    quint32 msg = 0x0000C0 | (index << 8) | mMidiChannel;
    if (!isConnected()) {
        midiOutShortMsg(hMidiOut, msg);
    } else if (isConnected()) {
        mServerSocket->sendShortMsg(msg);
    }
    setFocus();
}