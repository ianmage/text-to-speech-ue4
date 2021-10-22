#include "FMRTTSLib.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#pragma warning(push)
#pragma warning(disable: 4191)
#pragma warning(disable: 4996)
#ifndef DeleteFile
#define DeleteFile DeleteFileW
#endif
#ifndef MoveFile
#define MoveFile MoveFileW
#endif
#ifndef LoadString
#define LoadString LoadStringW
#endif
#ifndef InterlockedDecrement
#define InterlockedDecrement _InterlockedDecrement
#endif
#ifndef InterlockedIncrement
#define InterlockedIncrement _InterlockedIncrement
#endif
#ifndef GetMessage
#define GetMessage GetMessageW
#endif

#define _ATL_APARTMENT_THREADED

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override something,
//but do not change the name of _Module
//extern CComModule _Module;
#include <atlcom.h>

#include <sapi.h>
#include <sphelper.h>

#undef DeleteFile
#undef MoveFile
#undef LoadString
#undef InterlockedDecrement
#undef InterlockedIncrement
#undef GetMessage
#pragma warning(pop)


namespace NS_FMRTTS
{

class TTSException {};


static void checkAndThowException(HRESULT hr)
{
	if (FAILED(hr)) throw TTSException();
}


static char* TextToWavInner(const wchar_t* voiceRequiredAttributes, const wchar_t* voiceOptionalAttributes, long rate, const wchar_t* textToRender, ULONG* pBytesRead)
{
	HRESULT hr;
	CComPtr<ISpVoice> cpVoice; //Will send data to ISpStream
	CComPtr<ISpStream> cpStream; //Will contain IStream
	CComPtr<IStream> cpBaseStream; //raw data
	ISpObjectToken* cpToken(NULL); //Will set voice characteristics

	GUID guidFormat;
	WAVEFORMATEX* pWavFormatEx = nullptr;

	hr = cpVoice.CoCreateInstance(CLSID_SpVoice);
	checkAndThowException(hr);

	hr = SpFindBestToken(SPCAT_VOICES, voiceRequiredAttributes, voiceOptionalAttributes, &cpToken);
	checkAndThowException(hr);

	hr = cpVoice->SetVoice(cpToken);
	cpToken->Release();
	checkAndThowException(hr);

	cpVoice->SetRate(rate);

	hr = cpStream.CoCreateInstance(CLSID_SpStream);
	checkAndThowException(hr);

	hr = CreateStreamOnHGlobal(NULL, TRUE, &cpBaseStream);
	checkAndThowException(hr);

	hr = SpConvertStreamFormatEnum(SPSF_44kHz16BitMono, &guidFormat, &pWavFormatEx);
	checkAndThowException(hr);

	hr = cpStream->SetBaseStream(cpBaseStream, guidFormat, pWavFormatEx);
	checkAndThowException(hr);

	hr = cpVoice->SetOutput(cpStream, FALSE);
	checkAndThowException(hr);

	SpeechVoiceSpeakFlags voiceFlags = SpeechVoiceSpeakFlags::SVSFDefault;
	hr = cpVoice->Speak(textToRender, voiceFlags, NULL);
	checkAndThowException(hr);

	// Uncomment below to directly output speech
	//cpVoice->SetOutput(NULL, FALSE);
	//cpVoice->SpeakStream(cpStream, SPF_DEFAULT, NULL);

	LARGE_INTEGER a = { 0 };
	hr = cpStream->Seek(a, STREAM_SEEK_SET, NULL);
	checkAndThowException(hr);

	STATSTG stats;
	cpStream->Stat(&stats, STATFLAG_NONAME);
		
	ULONG sSize = stats.cbSize.LowPart;
		
	char *pBuffer = new char[sSize];
	cpStream->Read(pBuffer, sSize, pBytesRead);

	return pBuffer;
}

#include "Windows/HideWindowsPlatformTypes.h"


char* FMRTTSLib::TextToWav(const wchar_t* voiceRequiredAttributes, const wchar_t* voiceOptionalAttributes, long rate, const wchar_t* textToRender, unsigned long* pBytesRead)
{
	if (FAILED(::CoInitialize(NULL))) return NULL;
		
	try
	{
		char* r = TextToWavInner(voiceRequiredAttributes, voiceOptionalAttributes, rate, textToRender, pBytesRead);
		::CoUninitialize();
		return r;
	}
	catch (TTSException)
	{
		::CoUninitialize();
		return NULL;
	}
}

}