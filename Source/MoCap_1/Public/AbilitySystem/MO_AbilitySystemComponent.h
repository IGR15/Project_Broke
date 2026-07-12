// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "MO_AbilitySystemComponent.generated.h"

class UMO_GameplayAbility;

/**
 * Project AbilitySystemComponent. Lives on MO_PlayerState for players.
 */
UCLASS()
class MOCAP_1_API UMO_AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	// Grants the item ability (server only) and activates it once.
	// If the ability was already granted by a previous Mystery Box, it is just re-activated.
	void AddItemAbility(const TSubclassOf<UMO_GameplayAbility>& AbilityClass, int32 AbilityLevel = 1);
};
