// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/MO_ProjectileBase.h"
#include "MO_Missile.generated.h"

/**
 * Non-homing bazooka rocket: flies straight along the direction it was
 * fired in. Tag: Abilities.Item.Bazooka.
 */
UCLASS()
class MOCAP_1_API AMO_Missile : public AMO_ProjectileBase
{
	GENERATED_BODY()

public:
	AMO_Missile();
};
