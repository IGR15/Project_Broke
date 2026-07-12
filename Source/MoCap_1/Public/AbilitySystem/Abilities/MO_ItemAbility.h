// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/MO_GameplayAbility.h"
#include "MO_ItemAbility.generated.h"

/**
 * Base class for the item abilities granted by the Mystery Box.
 * For now activation only prints the ability's item tag.
 */
UCLASS(Abstract)
class MOCAP_1_API UMO_ItemAbility : public UMO_GameplayAbility
{
	GENERATED_BODY()

public:
	FGameplayTag GetItemAbilityTag() const;

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	// Full tag name, e.g. Abilities.Item.Bazooka. Set in each item ability's constructor
	// and resolved at runtime, because native tags register after CDO construction.
	FName ItemTagName;
};
