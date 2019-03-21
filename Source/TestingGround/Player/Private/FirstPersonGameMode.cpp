// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "../Public/FirstPersonGameMode.h"
#include "../Public/FirstPersonHUD.h"
#include "../Public/FirstPersonCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFirstPersonGameMode::AFirstPersonGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Player/Character/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFirstPersonHUD::StaticClass();
}
