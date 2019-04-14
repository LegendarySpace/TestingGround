// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

USTRUCT(BlueprintType)
struct FSpawner
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setup)
	TSubclassOf<AActor> SpawnClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setup)
	int SpawnMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setup)
	int SpawnMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setup)
	float ScaleMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setup)
	float ScaleMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setup)
	bool RandomScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setup)
	bool UniformScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setup)
	FVector BoundSize;

	FSpawner()
	{
		SpawnClass = NULL;
		SpawnMin = 0;
		SpawnMax = 7;
		ScaleMin = .25;
		ScaleMax = 4;
		RandomScale = false;
		UniformScale = true;
		BoundSize = FVector::ZeroVector;
	}
	
	bool operator==(const FSpawner& rightValue)
	{
		if (rightValue.SpawnClass == NULL) return false;
		return true;
	}

	bool operator==(const int& rightValue)
	{
		if (rightValue == NULL) return false;
		return true;
	}

	bool isEmpty()
	{
		if (SpawnClass == NULL) return true;
		return false;
	}
};

UCLASS()
class TESTINGGROUND_API ATile : public AActor
{
	GENERATED_BODY()

private:
	TArray<AActor*> Props = TArray<AActor*>();
	
public:	
	// Sets default values for this actor's properties
	ATile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called before the actor is destroyed
	virtual void BeginDestroy() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = setup)
	void PlaceActors(TArray<FSpawner> ActorsToSpawn);

	UFUNCTION(BlueprintCallable, Category = setup)
	bool SpawnAreaClear(FVector Location, float Radius);

	UFUNCTION(BlueprintCallable, Category = setup)
	bool FindEmptyLocation(FVector& OutLocation, float Radius);

	UFUNCTION(BlueprintCallable, Category = setup)
	AActor* PlaceActor(TSubclassOf<AActor> ToSpawn, FVector Location, float Rotation, float Scale);

	UFUNCTION(BlueprintCallable, Category = setup)
	float FindObjectRadius(TSubclassOf<AActor> ClassToCheck, FVector DeltaBounds = FVector::ZeroVector);

};
