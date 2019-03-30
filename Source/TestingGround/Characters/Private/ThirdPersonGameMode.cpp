// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "../Public/ThirdPersonGameMode.h"
#include "../Public/ThirdPersonCharacter.h"
#include "UObject/ConstructorHelpers.h"

AThirdPersonGameMode::AThirdPersonGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Characters/ThirdPersonCharacter_BP"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
