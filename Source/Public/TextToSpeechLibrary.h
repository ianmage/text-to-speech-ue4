#pragma once

//#include "TextToSpeechPCH.h"
#include "TextToSpeechLibrary.generated.h"


UCLASS()
class TEXTTOSPEECH_API UTextToSpeechLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

	public:
		UFUNCTION(BlueprintPure, Category = "Text To Speech")
		static USoundWave* TextToWave(FString VoiceRequiredAttributes = "vendor=microsoft;language=804", FString VoiceOptionalAttributes = "gender=female", int32 Rate = 0, FString Text = "test");
};
