// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/MO_ProjectileBase.h"
#include "MO_HomingMissile.generated.h"

/**
 * Homing bazooka rocket: locks onto a single explicitly-assigned target
 * (the racer the player was aiming at when they fired, see
 * AMO_PlayerController::GetAimedRacerTarget) and steers toward it for the
 * rest of its flight. Never re-scans for a different target. If no target
 * is set it just flies straight, same as AMO_Missile.
 */
UCLASS()
class MOCAP_1_API AMO_HomingMissile : public AMO_ProjectileBase
{
	GENERATED_BODY()

public:
	AMO_HomingMissile();

	// Call right after spawning (before or after BeginPlay - both are safe)
	// to lock the missile onto InTarget. Pass nullptr for straight flight.
	UFUNCTION(BlueprintCallable, Category = "Projectile|Homing")
	void SetHomingTarget(AActor* InTarget);

protected:
	virtual void PreInitializeComponents() override;

	// How hard the missile steers toward its target; higher = tighter turns.
	UPROPERTY(EditAnywhere, Category = "Projectile|Homing")
	float HomingAccelerationMagnitude = 6000.f;

private:
	UPROPERTY()
	TObjectPtr<AActor> HomingTarget;
};
