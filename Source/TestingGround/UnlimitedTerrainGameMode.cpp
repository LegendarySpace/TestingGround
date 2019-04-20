// Fill out your copyright notice in the Description page of Project Settings.

#include "UnlimitedTerrainGameMode.h"
#include "System/ActorPool.h"
#include "Terrain/Tile.h"
#include "EngineUtils.h"
#include "NavMeshBoundsVolume.h"
#include "UObject/ConstructorHelpers.h"

AUnlimitedTerrainGameMode::AUnlimitedTerrainGameMode()
{
	// Set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Characters/BP_Character"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	NavPool = CreateDefaultSubobject<UActorPool>(FName("Nav Pool"));
}

void AUnlimitedTerrainGameMode::AddToPool(ANavMeshBoundsVolume* NavMesh)
{
	UE_LOG(LogTemp, Warning, TEXT("%s Added %s to pool"), *GetName(), *NavMesh->GetName());
	NavPool->Add(NavMesh);
}

void AUnlimitedTerrainGameMode::BeginPlay()
{
	Super::BeginPlay();

	PopulateBoundsVolumePool();

	SpawnLoop();
}

void AUnlimitedTerrainGameMode::SpawnLoop()
{
	for (size_t i = 0; i < TileCount; i++)
	{
		SpawnNextTile();
	}
}

//void AUnlimitedTerrainGameMode::SpawnNextTile_Implementation()
//{
//	UE_LOG(LogTemp, Warning, TEXT("Spawning Tile: %s"), *TileToSpawn);
//	if (TileToSpawn = NULL) return;
//	auto Tile = GetWorld()->SpawnActor<ATile>(TileToSpawn, NextTileTransform);
//	if (Tile == nullptr) return;
//	NextTileTransform = Tile->GetAttachLocation();
//	UE_LOG(LogTemp, Warning, TEXT("Done Spawning Tile"));
//}

void AUnlimitedTerrainGameMode::PopulateBoundsVolumePool()
{
	auto NavMeshIter = TActorIterator<ANavMeshBoundsVolume>(GetWorld());
	while (NavMeshIter)
	{
		AddToPool(*NavMeshIter);
		++NavMeshIter;
	}
}
