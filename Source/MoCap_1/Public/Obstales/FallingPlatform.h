// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FallingPlatform.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

UCLASS()
class MOCAP_1_API AFallingPlatform : public AActor
{
	GENERATED_BODY()

public:
	AFallingPlatform();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

public:

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* PlatformMesh;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* TriggerCollision;

	// Time after the player steps on the platform before it starts falling
	UPROPERTY(EditAnywhere, Category="Falling Platform")
	float FallDelay = 2.f;

	// Time after the player steps on the platform before the actor is destroyed
	UPROPERTY(EditAnywhere, Category="Falling Platform")
	float DestroyDelay = 5.f;

	// Downward acceleration applied once the platform starts falling, in cm/s^2
	UPROPERTY(EditAnywhere, Category="Falling Platform")
	float FallAcceleration = 980.f;

private:
	void StartFalling();
	void DestroySelf();

	bool bHasTriggered = false;
	bool bIsFalling = false;
	float FallSpeed = 0.f;

	FTimerHandle FallTimerHandle;
	FTimerHandle DestroyTimerHandle;
};
