// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MysteryBox.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class URotatingMovementComponent;

UCLASS()
class MOCAP_1_API AMysteryBox : public AActor
{
	GENERATED_BODY()

public:
	AMysteryBox();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:
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
	UStaticMeshComponent* BoxMesh;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* TriggerCollision;

	UPROPERTY(VisibleAnywhere)
	URotatingMovementComponent* RotatingMovement;

	// Degrees per second
	UPROPERTY(EditAnywhere, Category="MysteryBox|Spin")
	float RotationSpeed = 60.f;

	// Vertical distance travelled up/down from the spawn height
	UPROPERTY(EditAnywhere, Category="MysteryBox|Float")
	float FloatAmplitude = 15.f;

	// Full up-down cycles per second
	UPROPERTY(EditAnywhere, Category="MysteryBox|Float")
	float FloatFrequency = 0.5f;

	// Inclusive random range given to the player on overlap
	UPROPERTY(EditAnywhere, Category="MysteryBox|Reward")
	int32 MinItemNumber = 1;

	UPROPERTY(EditAnywhere, Category="MysteryBox|Reward")
	int32 MaxItemNumber = 12;

private:
	FVector InitialLocation = FVector::ZeroVector;
	float RunningTime = 0.f;
};
