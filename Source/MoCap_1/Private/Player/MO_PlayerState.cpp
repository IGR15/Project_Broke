// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MO_PlayerState.h"

#include "AbilitySystem/MO_AbilitySystemComponent.h"

AMO_PlayerState::AMO_PlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UMO_AbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	SetNetUpdateFrequency(100.f);
}

UAbilitySystemComponent* AMO_PlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
