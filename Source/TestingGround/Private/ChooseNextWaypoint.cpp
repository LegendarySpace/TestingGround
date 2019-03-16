// Fill out your copyright notice in the Description page of Project Settings.

#include "../Public/ChooseNextWaypoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "../Public/PatrolGuard.h"



EBTNodeResult::Type UChooseNextWaypoint::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	auto BlackboardComp = OwnerComp.GetBlackboardComponent();
	auto Index = BlackboardComp->GetValueAsInt(IndexKey.SelectedKeyName);

	// Get Patrol Points
	auto Pawn = OwnerComp.GetAIOwner()->GetPawn();
	auto Guard = Cast<APatrolGuard>(Pawn);
	auto PatrolPoints = Guard->PatrolPoints;

	// Set Next Waypoint
	if (!PatrolPoints.Num()) return EBTNodeResult::Failed;
	BlackboardComp->SetValueAsObject(WaypointKey.SelectedKeyName, PatrolPoints[Index]);

	// Cycle Index
	BlackboardComp->SetValueAsInt(IndexKey.SelectedKeyName, (Index + 1) % PatrolPoints.Num());



	return EBTNodeResult::Succeeded;
}
