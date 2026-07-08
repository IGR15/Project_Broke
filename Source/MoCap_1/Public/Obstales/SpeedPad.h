// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpeedPad.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

UCLASS()
class MOCAP_1_API ASpeedPad : public AActor
{
	GENERATED_BODY()

public:
	ASpeedPad();

protected:
	virtual void BeginPlay() override;

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
	UStaticMeshComponent* PadMesh;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* TriggerCollision;

	// How much MaxWalkSpeed is multiplied by while boosted
	UPROPERTY(EditAnywhere, Category="Speed")
	float SpeedMultiplier = 3.f;

	// How long the boost lasts, in seconds
	UPROPERTY(EditAnywhere, Category="Speed")
	float BoostDuration = 2.f;
};
