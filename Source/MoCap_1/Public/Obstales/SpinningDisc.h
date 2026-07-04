// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpinningDisc.generated.h"

class UStaticMeshComponent;
class URotatingMovementComponent;

UCLASS()
class MOCAP_1_API ASpinningDisc : public AActor
{
	GENERATED_BODY()

public:
	ASpinningDisc();

protected:
	virtual void BeginPlay() override;

public:

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* DiscMesh;

	UPROPERTY(VisibleAnywhere)
	URotatingMovementComponent* RotatingMovement;

	// Degrees per second
	UPROPERTY(EditAnywhere, Category="Rotation")
	float RotationSpeed = 180.f;

	// true = clockwise, false = counter-clockwise
	UPROPERTY(EditAnywhere, Category="Rotation")
	bool bRotateLeft = true;
};
