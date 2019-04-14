// Fill out your copyright notice in the Description page of Project Settings.

#include "Tile.h"
#include "DrawDebugHelpers.h"

// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void ATile::BeginDestroy()
{
	Super::BeginDestroy();
	for (size_t i = 0; i < Props.Num(); i++)
	{
		Props[i]->Destroy();
	}
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
			float Scale = SpawnInfo.RandomScale ? FMath::FRandRange(SpawnInfo.ScaleMin, SpawnInfo.ScaleMax) : 1.f;
			// FVector Bounds = SpawnInfo.UniformScale ? {Scale*SpawnInfo.BoundSize.X, Scale*SpawnInfo.BoundSize.Y, Scale*SpawnInfo.BoundSize.Z} : {rand*SpawnInfo.BoundSize.X, rand*SpawnInfo.BoundSize.Y, rand*SpawnInfo.BoundSize.Z}
			float Radius = FindObjectRadius(ToSpawn, SpawnInfo.BoundSize);
			if (FindEmptyLocation(SpawnPoint, Radius*Scale))
			{
				float Rotation = FMath::FRandRange(-180, 180);
				Props.Push(PlaceActor(ToSpawn, SpawnPoint, Rotation, Scale));
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

	DrawDebugSphere(GetWorld(), GlobalLocation, Radius, 10, ResultColor, true, 20);

	return !HasHit;
}

bool ATile::FindEmptyLocation(FVector& OutLocation, float Radius)
{
	FVector min = FVector(0, -2000, 0);
	FVector max = FVector(4000, 2000, 0);
	FBox bounds = FBox(min, max);
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

AActor* ATile::PlaceActor(TSubclassOf<AActor> ToSpawn, FVector Location, float Rotation, float Scale)
{
	FRotator Rot = FRotator(0.f, Rotation, 0.f);
	AActor* Spawned = GetWorld()->SpawnActor<AActor>(ToSpawn);
	Spawned->SetActorRelativeLocation(Location);
	Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	Spawned->SetActorScale3D(FVector(Scale));
	Spawned->SetActorRotation(Rot);
	return Spawned;
}

float ATile::FindObjectRadius(TSubclassOf<AActor> ClassToCheck, FVector DeltaBounds)
{
	float X2, Y2, Z2, Radius;
	if (DeltaBounds == FVector::ZeroVector)
	{
		/** Determine Delta from object **/
		// Spawn Actor
		AActor* TestObject = GetWorld()->SpawnActor<AActor>(ClassToCheck);
		// DeltaBounds = GetBoundsExtent() - GetBoundsOrigin()
		FVector Origin, Extent;
		TestObject->GetActorBounds(true, Origin, Extent);
		DeltaBounds = Extent - Origin;
		TestObject->Destroy();
	}
	// Radius = SqRt(DeltaX^2 + DeltaY^2 + DeltaZ^2)
	X2 = FMath::Square<float>(DeltaBounds.X);
	Y2 = FMath::Square<float>(DeltaBounds.Y);
	Z2 = FMath::Square<float>(DeltaBounds.Z);
	Radius = FMath::Sqrt(X2 + Y2 + Z2);
	return Radius;
}

