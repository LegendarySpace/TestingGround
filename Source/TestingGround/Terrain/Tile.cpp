// Fill out your copyright notice in the Description page of Project Settings.

#include "Tile.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "System/ActorPool.h"
#include "NavigationSystem.h"
#include "../UnlimitedTerrainGameMode.h"

// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();


}

void ATile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// Return Nav Mesh
	Pool->Return(NavMeshBounds);

	for (size_t i = 0; i < Props.Num(); i++)
	{
		if (Props[i] != nullptr) Props[i]->Destroy();
	}

	// TODO:: Make sure the game mode isn't shutting down
	if (EndPlayReason == EEndPlayReason::Destroyed)
	{
		auto GameMode = (AUnlimitedTerrainGameMode*)GetWorld()->GetAuthGameMode();
		GameMode->SpawnNextTile();
	}
}

// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATile::SetPool(UActorPool * InPool)
{
	Pool = InPool;

	PositionNavMeshBounds();
}

void ATile::PositionNavMeshBounds()
{
	// TODO:: Fix location. Centering on tile origin, not Tile->Floor->Location
	NavMeshBounds = Pool->Checkout();
	if (NavMeshBounds == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Not Enough Actors in pool"));
		return;
	}
	NavMeshBounds->SetActorLocation(GetActorLocation() + GetFloorOffset());
	FNavigationSystem::Build(*GetWorld());
}

FVector ATile::GetFloorOffset()
{
	FVector Delta = (TileMax - TileMin) / 2;
	return FVector(Delta.X, 0.f, 0.f);
}

void ATile::PlaceActors(TArray<FSpawnParameters> ActorsToSpawn)
{
	// Iterate through each actor type
	for (size_t i = 0; i < ActorsToSpawn.Num(); i++)
	{
		if (ActorsToSpawn[i].isEmpty()) continue;
		FSpawnParameters SpawnInfo = ActorsToSpawn[i];
		int32 NumberToSpawn = FMath::RandRange(SpawnInfo.SpawnMin, SpawnInfo.SpawnMax);

		// Spawn each actor
		for (size_t j = 0; j < NumberToSpawn; j++)
		{
			FSpawnItem ItemToSpawn = FSpawnItem(&SpawnInfo);

			// If actor can be scaled try spawing at different sizes
			int Attempts = SpawnInfo.UseRandomScale ? 5 : 1;
			for (size_t i = 0; i < Attempts; i++)
			{
				FVector RandomScale = SpawnInfo.UseUniformScale ? FVector(FMath::FRandRange(SpawnInfo.ScaleMin, SpawnInfo.ScaleMax)) : FVector(FMath::FRandRange(SpawnInfo.ScaleMin, SpawnInfo.ScaleMax), 
							FMath::FRandRange(SpawnInfo.ScaleMin, SpawnInfo.ScaleMax), FMath::FRandRange(SpawnInfo.ScaleMin, SpawnInfo.ScaleMax));
				ItemToSpawn.Scale = SpawnInfo.UseRandomScale ? RandomScale : FVector(1.f);

				float Radius = FindObjectRadius(SpawnInfo.SpawnClass, ItemToSpawn.Scale);
				if (FindEmptyLocation(ItemToSpawn.Location, Radius))
				{
					// If AI spawn as pawn, otherwise spawn actor
					//auto special = SpawnInfo.GetSpawnClass();
					if (SpawnInfo.IsPawn)
					{
						PlaceAIPawn(ItemToSpawn);
					}
					else
					{
						ItemToSpawn.Rotation = FRotator(0.f, FMath::FRandRange(-180, 180), 0.f);
						Props.Push(PlaceActor(ItemToSpawn));
						break;
					}
				}
			}
		}
	}
}

bool ATile::SpawnAreaClear(FVector Location, float Radius)
{
	FVector GlobalLocation = ActorToWorld().TransformPosition(Location);
	FHitResult HitResult;
	bool HasHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		GlobalLocation,
		GlobalLocation,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(Radius)
	);

	FColor ResultColor = HasHit?FColor::Red: FColor::Emerald;

	// DrawDebugSphere(GetWorld(), GlobalLocation, Radius, 10, ResultColor, true, 20);

	return !HasHit;
}

bool ATile::FindEmptyLocation(FVector& OutLocation, float Radius)
{
	FBox bounds = FBox(TileMin, TileMax);
	const int MAX_ATTEMPTS = 40;
	for (size_t i = 0; i < MAX_ATTEMPTS; i++)
	{
		FVector SpawnPoint = FMath::RandPointInBox(bounds);
		if (SpawnAreaClear(SpawnPoint, Radius))
		{
			OutLocation = SpawnPoint;
			return true;
		}
	}
	return false;
}

AActor* ATile::PlaceActor(FSpawnItem ActorInfo)
{
	AActor* Spawned = GetWorld()->SpawnActor<AActor>(ActorInfo.Parameters->SpawnClass);
	Spawned->SetActorRelativeLocation(ActorInfo.Location);
	Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	Spawned->SetActorScale3D(FVector(ActorInfo.Scale));
	Spawned->SetActorRotation(ActorInfo.Rotation);
	return Spawned;
}

APawn* ATile::PlaceAIPawn(FSpawnItem PawnInfo)
{
	APawn* Spawned = GetWorld()->SpawnActor<APawn>(PawnInfo.Parameters->SpawnClass);
	if (Spawned == nullptr) return Spawned;
	Spawned->SetActorRelativeLocation(PawnInfo.Location);
	Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	Spawned->SetActorScale3D(PawnInfo.Scale);
	Spawned->SetActorRotation(PawnInfo.Rotation);
	Spawned->SpawnDefaultController();
	for (FName tag : PawnInfo.Parameters->Tags)
	{
		Spawned->Tags.Push(tag);
	}
	return Spawned;
}

float ATile::FindObjectRadius(TSubclassOf<AActor> ClassToCheck, FVector ScaleBounds)
{
	float X2, Y2, Z2, Radius;
	/** Determine Delta from object **/
	// Spawn Actor
	AActor* TestObject = GetWorld()->SpawnActor<AActor>(ClassToCheck);
	// DeltaBounds = GetBoundsExtent() - GetBoundsOrigin()
	FVector Origin, Extent;
	TestObject->GetActorBounds(true, Origin, Extent);
	FVector DeltaBounds = Extent - Origin;
	DeltaBounds *= ScaleBounds;
	TestObject->Destroy();
	// Radius = SqRt(DeltaX^2 + DeltaY^2 + DeltaZ^2)
	X2 = FMath::Square<float>(DeltaBounds.X);
	Y2 = FMath::Square<float>(DeltaBounds.Y);
	Z2 = FMath::Square<float>(DeltaBounds.Z);
	Radius = FMath::Sqrt(X2 + Y2 + Z2);
	return Radius;
}

