// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "MO_AbilitySystemComponent.generated.h"

class UMO_GameplayAbility;

/**
 * Project AbilitySystemComponent. Lives on MO_PlayerState for players.
 *
 * Item abilities use a single input slot: the spec holding InputTag.UseItem in
 * its DynamicAbilityTags is the equipped item, activated by LMB. Picking up a
 * new item moves the slot tag to the new ability.
 */
UCLASS()
class MOCAP_1_API UMO_AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	// Grants the item ability (server only) and equips it into the single
	// InputTag.UseItem slot. One-shot: the spec is removed after the ability
	// ends, so each pickup gives exactly one use.
	void AddItemAbility(const TSubclassOf<UMO_GameplayAbility>& AbilityClass, int32 AbilityLevel = 1);

	// Input routing from MO_PlayerController. Slot = InputTag in DynamicAbilityTags.
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
};
