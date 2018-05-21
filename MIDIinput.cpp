#include "MIDIinput.h"


unsigned int  MIDIInput::iWriteMsgByte = 0;
char MIDIInput::mMsgBuffer[MSGBUFFERSIZE];
MIDIKEYSTATE MIDIInput::eKeyState[MIDIKEYCNT];

MIDIInput::MIDIInput() {
	hMidiInHandle	= NULL;
	iWriteMsgByte	= 0;
	ZeroMemory(mMsgBuffer, sizeof(mMsgBuffer));
	for (int i=0; i<MIDIKEYCNT; i++) {
		eKeyState[i] = MIDIKEYSTATE_NONE;
	}

}

MIDIInput::~MIDIInput() {
	StopInput();
	Clear();
}


void CALLBACK MIDIInput::MidiInProc(HMIDIIN hMidiInHandle, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2) {
	switch (wMsg) 
	{
		case MIM_OPEN:
			//printf("MIDI device was opened\n");
			WriteMsgBuffer(iWriteMsgByte, mMsgBuffer, "MIDI device was opened\n");
			break;
		case MIM_CLOSE:
			//printf("MIDI device was closed\n");
			WriteMsgBuffer(iWriteMsgByte, mMsgBuffer, "MIDI device was closed\n");
			break;
		case MIM_DATA:
		{
			unsigned char statusByte = (dwParam1 & 0x000000ff);


			if (statusByte != 0x80 && statusByte != 0x90) {
				WriteMsgBuffer(iWriteMsgByte, mMsgBuffer, "MIM_DATA: wMsg=%08X, p1=%08X, p2=%08X\n", wMsg, dwParam1, dwParam2);
				break;
			}

			byte velocity = (byte)((dwParam1 & 0x00ff0000) >> 16);
			bool is_pressed = velocity != 0;
			byte note = (byte)(( dwParam1 & 0x0000ff00 ) >> 8);

			// c4 - c6 まで
			if (note < 48 || note > 72) break;

			switch (statusByte) {
				case 0x90:	// ノートオン
					eKeyState[note-48] = MIDIKEYSTATE_PRESSED;
					WriteMsgBuffer(iWriteMsgByte, mMsgBuffer, "MIM_DATA: wMsg=%08X, p1=%08X, p2=%08X\n"
						"<NOTE ON> note = %u, velocity = %u\n", wMsg, dwParam1, dwParam2, note, velocity);
					break;
				case 0x80:	// ノートオフ
					eKeyState[note-48] = MIDIKEYSTATE_RELEASED;
					WriteMsgBuffer(iWriteMsgByte, mMsgBuffer, "MIM_DATA: wMsg=%08X, p1=%08X, p2=%08X\n"
						"<NOTE OFF>note = %u, velocity = %u\n", wMsg, dwParam1, dwParam2, note, velocity);
					break;
				default:
					break;
			}

		}
		break;
		case MIM_LONGDATA:
		case MIM_ERROR:
		case MIM_LONGERROR:
		case MIM_MOREDATA:
		default:
			break;
	}
}


int MIDIInput::WriteMsgBuffer(unsigned int &writeTotalByte, CHAR *buffer, CONST CHAR * format, ...) {
	if (&writeTotalByte	== NULL) return -1;
	if (buffer			== NULL) return -1;
	if (format			== NULL) return -1;


	// 前回書き込んだNULL文字と同じ位置に、書き込む	※NULL文字を上書きする
	if (writeTotalByte != 0) writeTotalByte--;

	int cnt = 0;
	va_list list;
	va_start(list, format);
	cnt = vsnprintf(mMsgBuffer + writeTotalByte, MSGBUFFERSIZE - writeTotalByte, format, list);
	va_end(list);

	writeTotalByte += cnt+1;	//NULL文字もカウント

	return cnt;
}



BOOL MIDIInput::Create(void) {
	if (hMidiInHandle != NULL) {
		StopInput();
		Clear();
	}


	MMRESULT res;
	UINT deviceID;
	/*WCHAR errmsg[MAXERRORLENGTH]; 
	char errmsg_buff[MAXERRORLENGTH];
*/
	deviceID = 0u;

	res = midiInOpen(&hMidiInHandle, deviceID, (DWORD_PTR)MidiInProc, NULL, CALLBACK_FUNCTION);
	if (res != MMSYSERR_NOERROR) {
		WriteMsgBuffer(iWriteMsgByte, mMsgBuffer, "Cannot open MIDI input device %u.\n", deviceID);
		return FALSE;
	} 

	WriteMsgBuffer(iWriteMsgByte, mMsgBuffer, "Successfully opened a MIDI input device %u.\n", deviceID);

	return TRUE;
}



BOOL MIDIInput::StartInput(void) {
	if (hMidiInHandle == NULL) {
		return FALSE;
	}

	midiInStart(hMidiInHandle);

	return TRUE;
}

BOOL MIDIInput::StopInput(void) {
	if (hMidiInHandle == NULL) {
		return FALSE;
	}

	midiInStop(hMidiInHandle);

	return TRUE;
}

BOOL MIDIInput::Clear(void) {
	if (hMidiInHandle == NULL) {
		return FALSE;
	}

	midiInReset(hMidiInHandle);
	midiInClose(hMidiInHandle);
	hMidiInHandle = NULL;

	return TRUE;
}


VOID MIDIInput::GetMidiMessage(CHAR *buf) {
	memcpy(buf, mMsgBuffer, MSGBUFFERSIZE);

	ZeroMemory(mMsgBuffer, sizeof(mMsgBuffer));
	iWriteMsgByte = 0;
}


BOOL MIDIInput::IsMessage(void) {
	if (iWriteMsgByte > 0)	return TRUE;
	else					return FALSE;
}


BOOL MIDIInput::GetKeyState(MIDIKEYSTATE *state) {
	if (state			== NULL) return FALSE;
	if (hMidiInHandle	== NULL) return FALSE;

	memcpy(state, eKeyState, sizeof(eKeyState));

	for (int i=0; i<MIDIKEYCNT; i++) {
		if	(eKeyState[i] == MIDIKEYSTATE_PRESSED)		eKeyState[i] = MIDIKEYSTATE_HOLD;
		else if (eKeyState[i] == MIDIKEYSTATE_RELEASED) eKeyState[i] = MIDIKEYSTATE_NONE;
	}

	return TRUE;
}
