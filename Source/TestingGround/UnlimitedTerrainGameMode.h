// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UnlimitedTerrainGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TESTINGGROUND_API AUnlimitedTerrainGameMode : public AGameModeBase
{
	GENERATED_BODY()
	

public:
	AUnlimitedTerrainGameMode();

	void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Nav Pool")
	void PopulateBoundsVolumePool();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Tile)
		TSubclassOf<class ATile> TileToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Tile)
		FTransform NextTileTransform = FTransform(FVector(-2000.f, 60.f, 170.f));

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Tile)
		int32 TileCount = 3;

	UFUNCTION(BlueprintCallable, Category = Tile)
		void SpawnLoop();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = Tile)
		void SpawnNextTile();

private:
	UFUNCTION()
	void AddToPool(class ANavMeshBoundsVolume* NavMesh);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pool")
	class UActorPool* NavPool;

};
