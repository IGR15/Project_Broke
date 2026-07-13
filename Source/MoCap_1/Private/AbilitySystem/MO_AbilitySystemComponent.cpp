// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/MO_AbilitySystemComponent.h"

#include "MO_GameplayTags.h"
#include "AbilitySystem/Abilities/MO_GameplayAbility.h"

void UMO_AbilitySystemComponent::AddItemAbility(const TSubclassOf<UMO_GameplayAbility>& AbilityClass, const int32 AbilityLevel)
{
	if (!AbilityClass || !IsOwnerActorAuthoritative())
	{
		return;
	}

	const FGameplayTag& SlotTag = FMO_GameplayTags::Get().InputTag_UseItem;

	// Single slot: whatever ability currently holds the slot tag loses it.
	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(SlotTag))
		{
			Spec.GetDynamicSpecSourceTags().RemoveTag(SlotTag);
			MarkAbilitySpecDirty(Spec);
		}
	}

	if (FGameplayAbilitySpec* ExistingSpec = FindAbilitySpecFromClass(AbilityClass))
	{
		ExistingSpec->GetDynamicSpecSourceTags().AddTag(SlotTag);
		ExistingSpec->RemoveAfterActivation = true;
		MarkAbilitySpecDirty(*ExistingSpec);
		return;
	}

	FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, AbilityLevel);
	AbilitySpec.GetDynamicSpecSourceTags().AddTag(SlotTag);
	// One-shot item: the engine clears the spec once the ability ends, so it
	// cannot be re-activated; the next box pickup grants it fresh.
	AbilitySpec.RemoveAfterActivation = true;
	GiveAbility(AbilitySpec);
}

void UMO_AbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (!Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{
			continue;
		}

		if (Spec.IsActive())
		{
			// Feeds WaitInputPress tasks and replicates the press to the server.
			AbilitySpecInputPressed(Spec);
			if (const UGameplayAbility* Instance = Spec.GetPrimaryInstance())
			{
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed,
					Spec.Handle, Instance->GetCurrentActivationInfo().GetActivationPredictionKey());
			}
		}
		else
		{
			// One activation per click; holding the button does not re-trigger.
			AbilitySpecInputPressed(Spec);
			TryActivateAbility(Spec.Handle);
		}
	}
}

void UMO_AbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	FScopedAbilityListLock ActiveScopeLock(*this);
	for (FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag) && Spec.IsActive())
		{
			// Feeds WaitInputRelease tasks and replicates the release to the server.
			AbilitySpecInputReleased(Spec);
			if (const UGameplayAbility* Instance = Spec.GetPrimaryInstance())
			{
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased,
					Spec.Handle, Instance->GetCurrentActivationInfo().GetActivationPredictionKey());
			}
		}
	}
}

void UMO_AbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	// Activation happens on Pressed (one per click). Reserved for future
	// hold-to-charge behavior so items don't re-trigger every frame.
}
