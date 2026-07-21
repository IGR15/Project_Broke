// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/MO_ProjectileItemAbility.h"
#include "MO_BazookaAbility.generated.h"

/**
 * Bazooka item ability. Tag: Abilities.Item.Bazooka
 * Fires the non-homing AMO_Missile.
 */
UCLASS()
class MOCAP_1_API UMO_BazookaAbility : public UMO_ProjectileItemAbility
{
	GENERATED_BODY()

public:
	UMO_BazookaAbility();
};
