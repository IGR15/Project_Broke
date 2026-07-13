// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "MO_InputConfig.generated.h"

class UInputAction;

USTRUCT(BlueprintType)
struct FMO_InputAction
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<const UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, meta = (Categories = "InputTag"))
	FGameplayTag InputTag = FGameplayTag();
};

/**
 * Data-driven map of InputActions to InputTags. The PlayerController binds every
 * entry through UMO_InputComponent and forwards the tag to the ASC, so abilities
 * only ever know about tags, never keys.
 */
UCLASS()
class MOCAP_1_API UMO_InputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = false) const;

	UPROPERTY(EditDefaultsOnly)
	TArray<FMO_InputAction> AbilityInputActions;
};
