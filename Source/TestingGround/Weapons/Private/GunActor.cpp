// Fill out your copyright notice in the Description page of Project Settings.

#include "../Public/GunActor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include <math.h>
#include "Animation/AnimInstance.h"
#include "../Public/BallProjectile.h"

// Sets default values
AGunActor::AGunActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a gun mesh component
	GunFrame = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunFrame"));
	GunFrame->SetupAttachment(RootComponent);

	MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	MuzzleLocation->SetupAttachment(GunFrame);
	MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));
}

// Called when the game starts or when spawned
void AGunActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGunActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGunActor::TriggerDown_Implementation()
{
	// If valid, triggerTimer < calculated minimal delay between trigger pulls, return  
	if (triggerTimer.IsValid()) return;

	// Start triggerTimer  // Calculated must be < BurstRate * BurstInterval
	GetWorld()->GetTimerManager().SetTimer(triggerTimer, this, &AGunActor::resetTrigger, calcMinTriggerDelay(), false);


	if (BurstRate > 1)
	{
		timerArray = new FTimerHandle[BurstRate];
	}

	// Fire first round
	OnFire();

	if (timerArray != nullptr)
	{
		// Fire additional rounds if applicable
		for (int32 i = 1; i < BurstRate; i++)
		{
			// Set delay to fire on
			GetWorld()->GetTimerManager().SetTimer(timerArray[i], this, &AGunActor::OnFire, BurstInterval * i, false);
		}

		// Destroy timers after use
		GetWorld()->GetTimerManager().SetTimer(timerArray[0], this, &AGunActor::destroyTimers, BurstInterval * BurstRate, false);
	}
}

void AGunActor::OnFire_Implementation()
{
	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			const FRotator SpawnRotation = MuzzleLocation->GetComponentRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = MuzzleLocation->GetComponentLocation();

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// spawn the projectile at the muzzle
			World->SpawnActor<ABallProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		}

		// try and play the sound if specified
		if (FireSound != NULL)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}

		// try and play a firing animation if specified
		if (FirstPersonFireAnimation != NULL && ThirdPersonFireAnimation != NULL)
		{
	
			if (FPAnimInstance != NULL && TPAnimInstance != NULL)
			{
				FPAnimInstance->Montage_Play(FirstPersonFireAnimation, 1.f);
				TPAnimInstance->Montage_Play(ThirdPersonFireAnimation, 1.f);
			}
		}
	}
}

USoundBase * AGunActor::GetFireSound()
{
	return FireSound;
}

float AGunActor::calcMinTriggerDelay()
{
	// round()
	int maxTriggerPulls;
	float minTriggerDelay, burstTime;

	maxTriggerPulls = FireRate / BurstRate;
	minTriggerDelay = 60 / maxTriggerPulls;
	burstTime = BurstRate * BurstInterval;
	if (burstTime < minTriggerDelay) return minTriggerDelay;
	FireRate = recalcFireRate();
	return burstTime;
}

int32 AGunActor::recalcFireRate()
{
	float burstTime = BurstRate * BurstInterval;
	int32 maxTriggerPulls = (int32)floor(60.f / burstTime);
	return BurstRate * maxTriggerPulls;
}

