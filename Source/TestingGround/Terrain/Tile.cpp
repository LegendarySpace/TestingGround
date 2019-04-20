// Fill out your copyright notice in the Description page of Project Settings.

#include "Tile.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "System/ActorPool.h"

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
	UE_LOG(LogTemp, Warning, TEXT("%s Returning %s to pool"), *GetName(), *NavMeshBounds->GetName());
	Pool->Return(NavMeshBounds);

	for (size_t i = 0; i < Props.Num(); i++)
	{
		Props[i]->Destroy();
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
	UE_LOG(LogTemp, Warning, TEXT("%s Checkedout %s"), *GetName(), *NavMeshBounds->GetName());
	NavMeshBounds->SetActorLocation(GetActorLocation());
}

void ATile::PlaceActors(TArray<FSpawner> ActorsToSpawn)
{
	for (size_t i = 0; i < ActorsToSpawn.Num(); i++)
	{
		if (ActorsToSpawn[i].isEmpty()) continue;
		FSpawner SpawnInfo = ActorsToSpawn[i];
		TSubclassOf<AActor> ToSpawn = SpawnInfo.SpawnClass;
		int32 NumberToSpawn = FMath::RandRange(SpawnInfo.SpawnMin, SpawnInfo.SpawnMax);

		for (size_t j = 0; j < NumberToSpawn; j++)
		{
			FVector SpawnPoint;
			int Attempts = SpawnInfo.RandomScale ? 5 : 1;
			for (size_t i = 0; i < Attempts; i++)
			{
				FVector RandomScale = SpawnInfo.UniformScale ? FVector(FMath::FRandRange(SpawnInfo.ScaleMin, SpawnInfo.ScaleMax)) : FVector(FMath::FRandRange(SpawnInfo.ScaleMin, SpawnInfo.ScaleMax), 
							FMath::FRandRange(SpawnInfo.ScaleMin, SpawnInfo.ScaleMax), FMath::FRandRange(SpawnInfo.ScaleMin, SpawnInfo.ScaleMax));
				FVector Scale = SpawnInfo.RandomScale ? RandomScale : FVector(1.f);
				float Radius = FindObjectRadius(ToSpawn, Scale);
				if (FindEmptyLocation(SpawnPoint, Radius))
				{
					float Rotation = FMath::FRandRange(-180, 180);
					Props.Push(PlaceActor(ToSpawn, SpawnPoint, Rotation, FVector(Scale)));
					break;
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

AActor* ATile::PlaceActor(TSubclassOf<AActor> ToSpawn, FVector Location, float Rotation, FVector Scale)
{
	FRotator Rot = FRotator(0.f, Rotation, 0.f);
	AActor* Spawned = GetWorld()->SpawnActor<AActor>(ToSpawn);
	Spawned->SetActorRelativeLocation(Location);
	Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	Spawned->SetActorScale3D(FVector(Scale));
	Spawned->SetActorRotation(Rot);
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

