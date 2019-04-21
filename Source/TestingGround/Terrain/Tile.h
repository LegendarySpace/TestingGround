// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

class UActorPool;
struct FSpawnParameters;




USTRUCT(BlueprintType)
struct FSpawnItem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawn)
		FVector Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawn)
		FRotator Rotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawn)
		FVector Scale;

	FSpawnParameters* Parameters;

	FSpawnItem()
	{
		Location = FVector();
		Rotation = FRotator();
		Scale = FVector();
		Parameters = nullptr;
	}

	FSpawnItem(FSpawnParameters* Param)
	{
		Location = FVector();
		Rotation = FRotator();
		Scale = FVector();
		Parameters = Param;
	}
};



USTRUCT(BlueprintType)
struct FSpawnParameters
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setup)
	TSubclassOf<AActor> SpawnClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setup)
	TArray<FName> Tags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setup)
	int SpawnMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setup)
	int SpawnMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setup)
	float ScaleMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setup)
	float ScaleMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setup)
	bool UseRandomScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setup)
	bool UseUniformScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Setup)
		bool IsPawn;

	FSpawnParameters()
	{
		SpawnClass = NULL;
		SpawnMin = 0;
		SpawnMax = 7;
		ScaleMin = .25;
		ScaleMax = 4;
		UseRandomScale = false;
		UseUniformScale = true;
		IsPawn = false;
	}
	
	bool operator==(const FSpawnParameters& rightValue)
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

	UActorPool* Pool;

	AActor* NavMeshBounds;
	
public:	
	// Sets default values for this actor's properties
	ATile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, Category = Spawning)
	FVector TileMin = FVector(0, -2000, 0);

	UPROPERTY(EditDefaultsOnly, Category = Spawning)
	FVector TileMax = FVector(4000, 2000, 0);

	UFUNCTION(BlueprintCallable, Category = Tile)
	void SetPool(UActorPool* InPool);

	void PositionNavMeshBounds();

	FVector GetFloorOffset();

	UFUNCTION(BlueprintCallable, Category = setup)
	void PlaceActors(TArray<FSpawnParameters> ActorsToSpawn);

	UFUNCTION(BlueprintCallable, Category = setup)
	bool SpawnAreaClear(FVector Location, float Radius);

	UFUNCTION(BlueprintCallable, Category = setup)
	bool FindEmptyLocation(FVector& OutLocation, float Radius);

	UFUNCTION(BlueprintCallable, Category = setup)
	AActor* PlaceActor(FSpawnItem ActorInfo);

	UFUNCTION(BlueprintCallable, Category = setup)
	APawn* PlaceAIPawn(FSpawnItem PawnInfo);

	UFUNCTION(BlueprintCallable, Category = setup)
	float FindObjectRadius(TSubclassOf<AActor> ClassToCheck, FVector ScaleBounds = FVector::ZeroVector);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = Tile)
	FTransform GetAttachLocation();

};
