// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MO_ItemInfo.generated.h"

class UMO_ItemAbility;

USTRUCT(BlueprintType)
struct FMO_ItemAbilityInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString ItemName = FString();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UMO_ItemAbility> AbilityClass;
};

/**
 * Data asset mapping Mystery Box items to their granted abilities.
 * The class defaults already contain the six items, so the Mystery Box
 * works without an asset; create a DA to override the table in-editor.
 */
UCLASS(BlueprintType)
class MOCAP_1_API UMO_ItemInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UMO_ItemInfo();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ItemInformation")
	TArray<FMO_ItemAbilityInfo> ItemAbilities;

	FMO_ItemAbilityInfo GetRandomItem() const;
};
