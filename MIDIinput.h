#pragma once

#pragma warning(disable : 4996)

/////////////////////////////////////////////////////
// Windows APIを使用してMIDIキーボードの状態を取得するクラスです
// 参考　http://puarts.com/?pid=1124

// 作成者　山本七海
// 作成日　2017/10/11
/////////////////////////////////////////////////////


#include <stdio.h>
#include <Windows.h>
#include <mmsystem.h>	

#pragma comment(lib, "winmm.lib")


#define MSGBUFFERSIZE	(1024)
#define MIDIKEYCNT			(25)

typedef enum _MIDIKEYSTATE {
	MIDIKEYSTATE_NONE		= 0,
	MIDIKEYSTATE_PRESSED	= 1,
	MIDIKEYSTATE_HOLD		= 2,
	MIDIKEYSTATE_RELEASED	= 3
}MIDIKEYSTATE;


class MIDIInput {
private:
	HMIDIIN				hMidiInHandle;
	static CHAR			mMsgBuffer[MSGBUFFERSIZE];
	static unsigned int	iWriteMsgByte;				// mMsgBufferに書き込んだバイト数
	static MIDIKEYSTATE	eKeyState[MIDIKEYCNT];



private:
	static void CALLBACK MidiInProc(
		HMIDIIN midi_in_handle,
		UINT	wMsg,
		DWORD	dwInstance,
		DWORD	dwParam1,
		DWORD	dwParam2
		);

	
	static int WriteMsgBuffer(					
		unsigned int	&totalbufCnt,		//[in][out]
		CHAR			*buffer,			//[in][out]
		CONST CHAR		*format, ...		//[in]
		);

	
public:
	MIDIInput();
	~MIDIInput();

	BOOL Create( void );
	BOOL StartInput( void );
	BOOL StopInput( void );
	BOOL Clear( void );

	VOID GetMidiMessage( CHAR *buf );
	BOOL IsMessage(void);

	BOOL GetKeyState( MIDIKEYSTATE *state);

};




