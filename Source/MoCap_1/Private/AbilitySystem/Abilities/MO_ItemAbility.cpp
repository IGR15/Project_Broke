// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/MO_ItemAbility.h"

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
