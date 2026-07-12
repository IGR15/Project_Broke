// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/MO_AbilitySystemComponent.h"

#include "AbilitySystem/Abilities/MO_GameplayAbility.h"

void UMO_AbilitySystemComponent::AddItemAbility(const TSubclassOf<UMO_GameplayAbility>& AbilityClass, const int32 AbilityLevel)
{
	if (!AbilityClass || !IsOwnerActorAuthoritative())
	{
		return;
	}

	if (const FGameplayAbilitySpec* ExistingSpec = FindAbilitySpecFromClass(AbilityClass))
	{
		TryActivateAbility(ExistingSpec->Handle);
		return;
	}

	const FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, AbilityLevel);
	const FGameplayAbilitySpecHandle Handle = GiveAbility(AbilitySpec);
	TryActivateAbility(Handle);
}
