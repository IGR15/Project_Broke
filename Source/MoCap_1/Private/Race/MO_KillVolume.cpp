// Fill out your copyright notice in the Description page of Project Settings.


#include "Race/MO_KillVolume.h"

#include "Components/BoxComponent.h"
#include "Game/MO_RaceGameState.h"
#include "GameFramework/Pawn.h"
#include "Player/MO_PlayerState.h"

AMO_KillVolume::AMO_KillVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	Volume = CreateDefaultSubobject<UBoxComponent>("Volume");
	SetRootComponent(Volume);
	Volume->SetBoxExtent(FVector(2000.f, 2000.f, 200.f));
	Volume->SetCollisionProfileName(TEXT("Trigger"));
}

void AMO_KillVolume::NotifyActorBeginOverlap(AActor* OtherActor)
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
		RaceGS->RespawnRacerAtLastCheckpoint(RacerPS);
	}
}
