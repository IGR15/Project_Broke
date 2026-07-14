// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MO_PlayerState.h"

#include "AbilitySystem/MO_AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

AMO_PlayerState::AMO_PlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UMO_AbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	SetNetUpdateFrequency(100.f);
}

void AMO_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMO_PlayerState, PlayerScore);
	DOREPLIFETIME(AMO_PlayerState, CurrentLap);
	DOREPLIFETIME(AMO_PlayerState, TotalLaps);
}

UAbilitySystemComponent* AMO_PlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AMO_PlayerState::AddToScore(const int32 InScore)
{
	if (!HasAuthority())
	{
		return;
	}

	PlayerScore += InScore;
	// OnRep only fires on clients; broadcast here so listen-server /
	// standalone UI updates too.
	OnScoreChangedDelegate.Broadcast(PlayerScore);
}

void AMO_PlayerState::SetCurrentLap(const int32 InCurrentLap)
{
	if (!HasAuthority())
	{
		return;
	}

	CurrentLap = InCurrentLap;
	OnCurrentLapChangedDelegate.Broadcast(CurrentLap);
}

void AMO_PlayerState::SetTotalLaps(const int32 InTotalLaps)
{
	if (!HasAuthority())
	{
		return;
	}

	TotalLaps = InTotalLaps;
	OnTotalLapsChangedDelegate.Broadcast(TotalLaps);
}

void AMO_PlayerState::OnRep_PlayerScore(int32 OldPlayerScore)
{
	OnScoreChangedDelegate.Broadcast(PlayerScore);
}

void AMO_PlayerState::OnRep_CurrentLap(int32 OldCurrentLap)
{
	OnCurrentLapChangedDelegate.Broadcast(CurrentLap);
}

void AMO_PlayerState::OnRep_TotalLaps(int32 OldTotalLaps)
{
	OnTotalLapsChangedDelegate.Broadcast(TotalLaps);
}
