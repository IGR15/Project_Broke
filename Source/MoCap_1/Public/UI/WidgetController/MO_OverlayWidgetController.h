// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/MO_ItemInfo.h"
#include "UI/WidgetController/MO_WidgetController.h"
#include "MO_OverlayWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChangedSignature, int32, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemEquippedSignature, const FMO_ItemAbilityInfo&, ItemInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemClearedSignature);

/**
 * Widget controller for the in-game overlay (item slots + score + laps).
 * Binds to MO_PlayerState stat delegates and the ASC item-slot delegates,
 * then rebroadcasts to Blueprint via the delegates below.
 */
UCLASS(BlueprintType, Blueprintable)
class MOCAP_1_API UMO_OverlayWidgetController : public UMO_WidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadCastInitValues() override;
	virtual void BindCallbacks() override;

	UPROPERTY(BlueprintAssignable, Category="Stats")
	FOnPlayerStatChangedSignature OnScoreChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category="Stats")
	FOnPlayerStatChangedSignature OnCurrentLapChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category="Stats")
	FOnPlayerStatChangedSignature OnTotalLapsChangedDelegate;

	UPROPERTY(BlueprintAssignable, Category="Items")
	FOnItemEquippedSignature OnItemEquippedDelegate;

	UPROPERTY(BlueprintAssignable, Category="Items")
	FOnItemClearedSignature OnItemClearedDelegate;

protected:
	// Looks the tag up in ItemInfo and pushes the display info to widgets.
	void BroadcastItemForTag(const FGameplayTag& ItemTag);
};
