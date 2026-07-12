// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "MO_GameplayAbility.generated.h"

/**
 * Base class for every gameplay ability in the project.
 */
UCLASS(Abstract)
class MOCAP_1_API UMO_GameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UMO_GameplayAbility();
};
