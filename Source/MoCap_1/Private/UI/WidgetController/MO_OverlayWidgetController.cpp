// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/MO_OverlayWidgetController.h"

#include "AbilitySystem/MO_AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/MO_ItemAbility.h"
#include "Player/MO_PlayerState.h"

void UMO_OverlayWidgetController::BroadCastInitValues()
{
	if (AMO_PlayerState* MOPS = GetMOPS())
	{
		OnScoreChangedDelegate.Broadcast(MOPS->GetPlayerScore());
		OnCurrentLapChangedDelegate.Broadcast(MOPS->GetCurrentLap());
		OnTotalLapsChangedDelegate.Broadcast(MOPS->GetTotalLaps());
	}

	// An item can already be slotted if the overlay is (re)created mid-game.
	if (UMO_AbilitySystemComponent* MOASC = GetMOASC())
	{
		const FGameplayTag EquippedTag = MOASC->GetEquippedItemTag();
		if (EquippedTag.IsValid())
		{
			BroadcastItemForTag(EquippedTag);
		}
	}
}

void UMO_OverlayWidgetController::BindCallbacks()
{
	if (AMO_PlayerState* MOPS = GetMOPS())
	{
		MOPS->OnScoreChangedDelegate.AddLambda([this](const int32 NewValue)
		{
			OnScoreChangedDelegate.Broadcast(NewValue);
		});
		MOPS->OnCurrentLapChangedDelegate.AddLambda([this](const int32 NewValue)
		{
			OnCurrentLapChangedDelegate.Broadcast(NewValue);
		});
		MOPS->OnTotalLapsChangedDelegate.AddLambda([this](const int32 NewValue)
		{
			OnTotalLapsChangedDelegate.Broadcast(NewValue);
		});
	}

	if (UMO_AbilitySystemComponent* MOASC = GetMOASC())
	{
		MOASC->OnItemEquippedDelegate.AddLambda([this](const FGameplayTag& ItemTag)
		{
			BroadcastItemForTag(ItemTag);
		});
		MOASC->OnItemConsumedDelegate.AddLambda([this]()
		{
			OnItemClearedDelegate.Broadcast();
		});
	}
}

void UMO_OverlayWidgetController::BroadcastItemForTag(const FGameplayTag& ItemTag)
{
	if (!ItemInfo)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("MO_OverlayWidgetController: ItemInfo unset, cannot broadcast item %s. Fill it out on the controller Blueprint."),
			*ItemTag.ToString());
		return;
	}

	const FMO_ItemAbilityInfo Info = ItemInfo->FindItemInfoByTag(ItemTag);
	if (!Info.AbilityClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("MO_OverlayWidgetController: no ItemInfo row for tag %s."), *ItemTag.ToString());
		return;
	}

	OnItemEquippedDelegate.Broadcast(Info);
}
