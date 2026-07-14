// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/MO_ItemAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/MO_AbilitySystemComponent.h"

FGameplayTag UMO_ItemAbility::GetItemAbilityTag() const
{
	return FGameplayTag::RequestGameplayTag(ItemTagName, /*ErrorIfNotFound*/ false);
}

void UMO_ItemAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	// Blueprint children own the activation: Super dispatches to Event ActivateAbility,
	// and the graph is responsible for calling End Ability.
	if (bHasBlueprintActivate)
	{
		Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
		return;
	}

	// No Blueprint implementation - fall back to the debug print.
	const FGameplayTag ItemTag = GetItemAbilityTag();
	const FString TagString = ItemTag.IsValid() ? ItemTag.ToString() : ItemTagName.ToString();

	UE_LOG(LogTemp, Warning, TEXT("Item Ability Activated: %s"), *TagString);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.f,
			FColor::Green,
			FString::Printf(TEXT("Ability Activated: %s"), *TagString));
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, /*bReplicateEndAbility*/ true, /*bWasCancelled*/ false);
}

void UMO_ItemAbility::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const bool bReplicateEndAbility,
	const bool bWasCancelled)
{
	// Flag BEFORE Super: Super::EndAbility calls NotifyAbilityEnded, which
	// clears the spec when RemoveAfterActivation is set and no instance is
	// active. Setting the flag at grant time instead would block activation
	// (TryActivateAbility rejects flagged specs).
	if (IsEndAbilityValid(Handle, ActorInfo) && ActorInfo->IsNetAuthority())
	{
		if (UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get())
		{
			if (FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromHandle(Handle))
			{
				Spec->RemoveAfterActivation = true;
			}

			// Tell the owning client's UI the slot is now empty and promote
			// the queued item into the main slot, if one is waiting.
			if (UMO_AbilitySystemComponent* MOASC = Cast<UMO_AbilitySystemComponent>(ASC))
			{
				MOASC->NotifyItemConsumed();
			}
		}
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
