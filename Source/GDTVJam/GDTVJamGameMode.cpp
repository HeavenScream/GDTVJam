// Copyright Epic Games, Inc. All Rights Reserved.

#include "GDTVJamGameMode.h"
#include "GDTVJamCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGDTVJamGameMode::AGDTVJamGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
