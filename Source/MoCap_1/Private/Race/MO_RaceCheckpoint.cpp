// Fill out your copyright notice in the Description page of Project Settings.


#include "Race/MO_RaceCheckpoint.h"

#include "Components/BoxComponent.h"
#include "Game/MO_RaceGameState.h"
#include "GameFramework/Pawn.h"
#include "Player/MO_PlayerState.h"

AMO_RaceCheckpoint::AMO_RaceCheckpoint()
{
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>("Trigger");
	SetRootComponent(Trigger);
	Trigger->SetBoxExtent(FVector(60.f, 800.f, 400.f));
	Trigger->SetCollisionProfileName(TEXT("Trigger"));
}

void AMO_RaceCheckpoint::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!HasAuthority())
	{
		return;
	}

	if (AMO_RaceGameState* RaceGS = GetWorld()->GetGameState<AMO_RaceGameState>())
	{
		RaceGS->RegisterCheckpoint(CheckpointIndex, GetActorLocation());
	}
}

void AMO_RaceCheckpoint::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (!HasAuthority())
	{
		return;
	}

	const APawn* Pawn = Cast<APawn>(OtherActor);
	if (!Pawn)
	{
		return;
	}

	AMO_PlayerState* RacerPS = Pawn->GetPlayerState<AMO_PlayerState>();
	if (!RacerPS)
	{
		return;
	}

	if (AMO_RaceGameState* RaceGS = GetWorld()->GetGameState<AMO_RaceGameState>())
	{
		RaceGS->RacerCrossedCheckpoint(RacerPS, CheckpointIndex);
	}
}
