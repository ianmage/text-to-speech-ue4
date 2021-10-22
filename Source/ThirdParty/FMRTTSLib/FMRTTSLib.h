#pragma once

#include "CoreMinimal.h"


namespace NS_FMRTTS
{

class FMRTTSLib
{
	public:
		static char* TextToWav(const wchar_t* voiceRequiredAttributes, const wchar_t* voiceOptionalAttributes, long rate, const wchar_t* textToRender, unsigned long* pBytesRead);
};

}