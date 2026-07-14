// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "MO_AbilitySystemComponent.generated.h"

class UMO_GameplayAbility;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemSlotEquipped, const FGameplayTag& /*ItemTag*/);
DECLARE_MULTICAST_DELEGATE(FOnItemSlotCleared);

/**
 * Project AbilitySystemComponent. Lives on MO_PlayerState for players.
 *
 * Item abilities use a main input slot plus a queue: the spec holding
 * InputTag.UseItem in its DynamicAbilityTags is the equipped item, activated
 * by LMB. Picking up an item while the slot is occupied stores it as the
 * queued (secondary) item; consuming the main item promotes the queued one.
 */
UCLASS()
class MOCAP_1_API UMO_AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	// Grants the item ability (server only). Empty main slot: equips it into
	// InputTag.UseItem. Occupied: stores it as the queued item, replacing any
	// previous queued item. One-shot: the spec is removed after the ability
	// ends, so each pickup gives exactly one use.
	void AddItemAbility(const TSubclassOf<UMO_GameplayAbility>& AbilityClass, int32 AbilityLevel = 1);

	// Called by MO_ItemAbility::EndAbility on the server when the main item is
	// used up: notifies the UI and promotes the queued item into the main slot.
	void NotifyItemConsumed();

	// Input routing from MO_PlayerController. Slot = InputTag in DynamicAbilityTags.
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	void AbilityInputTagHeld(const FGameplayTag& InputTag);

	// UI: fired on the owning client when the item slots change.
	// The overlay widget controller binds to these.
	FOnItemSlotEquipped OnItemEquippedDelegate;
	FOnItemSlotCleared OnItemConsumedDelegate;
	FOnItemSlotEquipped OnSecondaryItemEquippedDelegate;
	FOnItemSlotCleared OnSecondaryItemClearedDelegate;

	// Item tag of the ability currently holding the UseItem slot
	// (invalid tag when the slot is empty).
	FGameplayTag GetEquippedItemTag() const;

	// Item tag of the queued (secondary) item; invalid when nothing is queued.
	// Server-set, client-cached through the RPCs, so it is valid on both sides.
	FGameplayTag GetQueuedItemTag() const { return QueuedItemTag; }

	// Server -> owning client notifications (broadcast locally in standalone).
	UFUNCTION(Client, Reliable)
	void ClientOnItemEquipped(const FGameplayTag& ItemTag);

	UFUNCTION(Client, Reliable)
	void ClientOnItemConsumed();

	UFUNCTION(Client, Reliable)
	void ClientOnSecondaryItemEquipped(const FGameplayTag& ItemTag);

	UFUNCTION(Client, Reliable)
	void ClientOnSecondaryItemCleared();

private:
	// Moves the ability into the InputTag.UseItem slot, granting it if needed
	// (the old single-slot AddItemAbility behavior).
	void EquipItemAbility(const TSubclassOf<UMO_GameplayAbility>& AbilityClass, int32 AbilityLevel);

	// Ability waiting for the main slot to free up. Server-only.
	UPROPERTY()
	TSubclassOf<UMO_GameplayAbility> QueuedItemAbilityClass;

	FGameplayTag QueuedItemTag;
};
