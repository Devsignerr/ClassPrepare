// Copyright Epic Games, Inc. All Rights Reserved.

#include "ClassGameMode.h"
#include "UObject/ConstructorHelpers.h"

AClassGameMode::AClassGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ParagonCrunch/Characters/Heroes/Crunch/CrunchPlayerCharacter.CrunchPlayerCharacter_C"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
