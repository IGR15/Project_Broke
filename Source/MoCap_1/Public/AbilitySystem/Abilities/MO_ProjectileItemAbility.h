// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/MO_ItemAbility.h"
#include "MO_ProjectileItemAbility.generated.h"

class AMO_ProjectileBase;

/**
 * Base for item abilities that fire a projectile (Bazooka = AMO_Missile,
 * Mosquito = AMO_HomingMissile - set ProjectileClass in each item's
 * constructor). Spawns ProjectileClass on the server in front of the avatar;
 * if the spawned actor is an AMO_HomingMissile, locks it onto whatever
 * AMO_PlayerController::GetAimedRacerTarget finds for the firer.
 *
 * TEMP spawn point: avatar location + forward/up offset, for testing before
 * the bazooka weapon mesh has a muzzle socket to spawn from instead.
 */
UCLASS(Abstract)
class MOCAP_1_API UMO_ProjectileItemAbility : public UMO_ItemAbility
{
	GENERATED_BODY()

protected:
	virtual void OnItemActivated(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;

	// Concrete projectile class to spawn; set in each item ability's constructor.
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<AMO_ProjectileBase> ProjectileClass;

	// TEMP: forward/up offset from the avatar's location used as the spawn
	// point. Replace with a muzzle socket lookup once the weapon mesh has one.
	UPROPERTY(EditAnywhere, Category = "Projectile|TempSpawnPoint")
	float SpawnForwardOffset = 150.f;

	UPROPERTY(EditAnywhere, Category = "Projectile|TempSpawnPoint")
	float SpawnUpOffset = 50.f;
};
