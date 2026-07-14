// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/MO_AbilitySystemComponent.h"

#include "MO_GameplayTags.h"
#include "AbilitySystem/Abilities/MO_GameplayAbility.h"
#include "AbilitySystem/Abilities/MO_ItemAbility.h"

namespace
{
	// Item tag of an ability class, read from its CDO (works pre-grant).
	FGameplayTag GetItemTagFromAbilityClass(const TSubclassOf<UMO_GameplayAbility>& AbilityClass)
	{
		if (const UMO_ItemAbility* ItemCDO = Cast<UMO_ItemAbility>(AbilityClass.GetDefaultObject()))
		{
			return ItemCDO->GetItemAbilityTag();
		}
		return FGameplayTag();
	}
}

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
		MarkAbilitySpecDirty(*ExistingSpec);
		ClientOnItemEquipped(GetItemTagFromAbilityClass(AbilityClass));
		return;
	}

	// One-shot consumption happens in UMO_ItemAbility::EndAbility, NOT via
	// RemoveAfterActivation here: TryActivateAbility refuses specs that
	// already carry that flag.
	FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, AbilityLevel);
	AbilitySpec.GetDynamicSpecSourceTags().AddTag(SlotTag);
	GiveAbility(AbilitySpec);
	ClientOnItemEquipped(GetItemTagFromAbilityClass(AbilityClass));
}

FGameplayTag UMO_AbilitySystemComponent::GetEquippedItemTag() const
{
	const FGameplayTag& SlotTag = FMO_GameplayTags::Get().InputTag_UseItem;

	for (const FGameplayAbilitySpec& Spec : GetActivatableAbilities())
	{
		if (Spec.GetDynamicSpecSourceTags().HasTagExact(SlotTag))
		{
			if (const UMO_ItemAbility* ItemAbility = Cast<UMO_ItemAbility>(Spec.Ability))
			{
				return ItemAbility->GetItemAbilityTag();
			}
		}
	}
	return FGameplayTag();
}

void UMO_AbilitySystemComponent::ClientOnItemEquipped_Implementation(const FGameplayTag& ItemTag)
{
	OnItemEquippedDelegate.Broadcast(ItemTag);
}

void UMO_AbilitySystemComponent::ClientOnItemConsumed_Implementation()
{
	OnItemConsumedDelegate.Broadcast();
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
