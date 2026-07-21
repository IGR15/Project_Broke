// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/MO_HomingMissile.h"

#include "GameFramework/ProjectileMovementComponent.h"

AMO_HomingMissile::AMO_HomingMissile()
{
}

void AMO_HomingMissile::PreInitializeComponents()
{
	// Same ordering reason as AMO_ProjectileBase::Speed.
	if (ProjectileMovement)
	{
		ProjectileMovement->HomingAccelerationMagnitude = HomingAccelerationMagnitude;
	}

	Super::PreInitializeComponents();
}

void AMO_HomingMissile::SetHomingTarget(AActor* InTarget)
{
	HomingTarget = InTarget;

	if (!ProjectileMovement)
	{
		return;
	}

	ProjectileMovement->HomingTargetComponent = InTarget ? InTarget->GetRootComponent() : nullptr;
	ProjectileMovement->bIsHomingProjectile = InTarget != nullptr;
}
