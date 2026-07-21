// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MO_ProjectileBase.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;
class UNiagaraSystem;
class UNiagaraComponent;
class USoundBase;
class UAudioComponent;

/**
 * Shared plumbing for the bazooka's rockets (Aura-projectile pattern):
 * mesh + sphere collision + ProjectileMovementComponent, overlap -> impact
 * FX/SFX + knockback via the hit character's ULaunchComponent (no
 * AttributeSet/damage pipeline exists yet, so knockback is the whole effect).
 * Concrete flight behaviour (straight vs. homing) lives in the subclasses.
 */
UCLASS(Abstract)
class MOCAP_1_API AMO_ProjectileBase : public AActor
{
	GENERATED_BODY()

public:
	AMO_ProjectileBase();

protected:
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	UFUNCTION()
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	// Impact FX/SFX (all machines) + knockback and Destroy (authority only).
	// bHit guards against a double-fire from overlapping components in the
	// same frame.
	virtual void OnImpact(AActor* HitActor, const FVector& HitLocation, const FVector& HitNormal);

	UPROPERTY(VisibleAnywhere, Category = "Projectile")
	TObjectPtr<USphereComponent> CollisionComp;

	UPROPERTY(VisibleAnywhere, Category = "Projectile")
	TObjectPtr<UStaticMeshComponent> MeshComp;

	UPROPERTY(VisibleAnywhere, Category = "Projectile")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	// Applied to ProjectileMovement's Initial/MaxSpeed at BeginPlay (before
	// Super::BeginPlay() starts the component, which snapshots InitialSpeed
	// into Velocity) - editing this directly is safe on a Blueprint child,
	// unlike editing ProjectileMovement's own speed from a C++ constructor.
	UPROPERTY(EditAnywhere, Category = "Projectile")
	float Speed = 1500.f;

	// Impulse handed to the hit character's ULaunchComponent, in the missile's
	// direction of travel at impact (same LaunchCharacter path BouncePad/Yeet use).
	UPROPERTY(EditAnywhere, Category = "Projectile|Knockback")
	float KnockbackForwardStrength = 1200.f;

	UPROPERTY(EditAnywhere, Category = "Projectile|Knockback")
	float KnockbackUpStrength = 600.f;

	UPROPERTY(EditAnywhere, Category = "Projectile|Effects")
	TObjectPtr<UNiagaraSystem> ImpactEffect;

	UPROPERTY(EditAnywhere, Category = "Projectile|Effects")
	TObjectPtr<USoundBase> ImpactSound;

	// Looping thrust sound played while the missile is in flight (Aura-projectile
	// pattern: spawned attached in BeginPlay, stopped on impact/destroy).
	UPROPERTY(EditAnywhere, Category = "Projectile|Effects")
	TObjectPtr<USoundBase> ThrusterSound;

	UPROPERTY()
	TObjectPtr<UAudioComponent> ThrusterSoundComponent;

	// Exhaust FX attached to the back of the mesh; acts as the missile's thrust trail.
	UPROPERTY(EditAnywhere, Category = "Projectile|Effects")
	TObjectPtr<UNiagaraSystem> ThrusterEffect;

	UPROPERTY(VisibleAnywhere, Category = "Projectile|Effects")
	TObjectPtr<UNiagaraComponent> ThrusterEffectComponent;

	// Stops and clears the looping thrust sound/FX; safe to call more than once
	// (impact and Destroyed() can both trigger it).
	void StopThrusterFX();

	// Safety net if the missile never overlaps anything.
	UPROPERTY(EditAnywhere, Category = "Projectile")
	float LifeSpanSeconds = 8.f;

	bool bHit = false;
};
