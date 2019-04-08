// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GunActor.generated.h"

UCLASS()
class TESTINGGROUND_API AGunActor : public AActor
{
	GENERATED_BODY()

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* GunFrame;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* MuzzleLocation;
	
public:	
	// Sets default values for this actor's properties
	AGunActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Firing)
	TSubclassOf<class ABallProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Firing)
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Firing)
	class UAnimMontage* FirstPersonFireAnimation;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Firing)
	class UAnimMontage* ThirdPersonFireAnimation;

	/** First Player animInstances */
	UPROPERTY()
	class UAnimInstance* FPAnimInstance;

	/** Third Player animInstances */
	UPROPERTY()
	class UAnimInstance* TPAnimInstance;

	/** Number of projectiles to spawn per trigger pull */
	/** ENUM { automatic = 0, semi-automatic, burst2, burst3, burst4, burst5 } */
	UPROPERTY(EditDefaultsOnly, Category = Firing)
	int32 BurstRate = 0;

	/** Time between projectile spawns per trigger pull */ // Applies to all Burst Rates except semi-automatic
	UPROPERTY(EditDefaultsOnly, Category = Firing)
	float BurstInterval = 0.2f;

	/** Number of projectiles to spawn per trigger pull */
	UPROPERTY(EditDefaultsOnly, Category = Firing)
	int32 FireRate = 120;

	/** Trigger is pulled, firing process begins */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Firing)
	void TriggerDown();

	/** Fires a projectile */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Firing)
	void OnFire();

	/** Getter method for the sound */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	USoundBase* GetFireSound();

	void setFireAnimation(UAnimMontage* mon) { FireAnimation = mon; }

private:
	UAnimMontage* FireAnimation;

	FTimerHandle triggerTimer;

	FTimerHandle* timerArray = nullptr;

	void destroyTimers() { delete timerArray; }

	void resetTrigger() { triggerTimer.Invalidate(); }

	float calcMinTriggerDelay();

	int32 recalcFireRate();
};
