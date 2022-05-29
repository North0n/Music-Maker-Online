#include "Client.h"
#include <QtWidgets/QApplication>

#include "Windows.h"
#include "mmeapi.h"

int main(int argc, char *argv[])
{
    HMIDIOUT hMidiOut;
    midiOutOpen(&hMidiOut, MIDI_MAPPER, NULL, NULL, CALLBACK_NULL);
    // From low to high in bytes: (https://www.midi.org/specifications-old/item/table-2-expanded-messages-list-status-bytes)
    //      1st: Function (90 - On, 80 - Off)
    //      2nd: Note Number (0 - 127) (0 - 7F)
    //      3rd: Note Velocity (0 - 127) (0-7F)
    midiOutShortMsg(hMidiOut, 0x7F3F90);
    /*midiOutShortMsg(hMidiOut, 0x787880);
    midiOutShortMsg(hMidiOut, 0x787890);
    midiOutShortMsg(hMidiOut, 0x787880);*/

    QApplication a(argc, argv);
    Client w;
    w.show();
    return a.exec();
}
