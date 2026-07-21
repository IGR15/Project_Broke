// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/MO_ProjectileItemAbility.h"
#include "MO_MosquitoAbility.generated.h"

/**
 * Mosquito item ability. Tag: Abilities.Item.Mosquito
 * Fires the homing AMO_HomingMissile, locked onto whatever the firing
 * player is currently aiming at (AMO_PlayerController::GetAimedRacerTarget).
 */
UCLASS()
class MOCAP_1_API UMO_MosquitoAbility : public UMO_ProjectileItemAbility
{
	GENERATED_BODY()

public:
	UMO_MosquitoAbility();
};
