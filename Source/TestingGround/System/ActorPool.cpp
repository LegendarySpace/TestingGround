// Fill out your copyright notice in the Description page of Project Settings.

#include "ActorPool.h"

// Sets default values for this component's properties
UActorPool::UActorPool()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

AActor* UActorPool::Checkout()
{
	if (Pool.Num() < 1)
	{
		return nullptr;
	}
	UE_LOG(LogTemp, Warning, TEXT("Pool count before checkout: %d"), Pool.Num());
	UE_LOG(LogTemp, Warning, TEXT("Checking Out Actor, remaing in pool: %d"), Pool.Num()-1);
	return Pool.Pop();
}

void UActorPool::Return(AActor * ActorToReturn)
{
	if (ActorToReturn == nullptr)
	{
		return;
	}
	Pool.Push(ActorToReturn);
	UE_LOG(LogTemp, Warning, TEXT("Returning %s to Pool, Pool count: %d"), *ActorToReturn->GetName(), Pool.Num());
}

void UActorPool::Add(AActor * ActorToAdd)
{
	if (ActorToAdd == nullptr)
	{
		return;
	}
	Pool.Push(ActorToAdd);
	UE_LOG(LogTemp, Warning, TEXT("Adding %s to Pool, Pool count: %d"), *ActorToAdd->GetName(), Pool.Num());
}
