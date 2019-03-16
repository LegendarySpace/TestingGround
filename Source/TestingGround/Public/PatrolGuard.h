// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TP_ThirdPerson/TP_ThirdPersonCharacter.h"
#include "PatrolGuard.generated.h"

/**
 * 
 */
UCLASS()
class TESTINGGROUND_API APatrolGuard : public ATP_ThirdPersonCharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(EditInstanceOnly, Category = "Patrol")
	TArray<AActor*> PatrolPoints;
};
