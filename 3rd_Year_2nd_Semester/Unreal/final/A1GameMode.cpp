// Copyright Epic Games, Inc. All Rights Reserved.

#include "A1GameMode.h"
#include "A1Character.h"
#include "UObject/ConstructorHelpers.h"

AA1GameMode::AA1GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}