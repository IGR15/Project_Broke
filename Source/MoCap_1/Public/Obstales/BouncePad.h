// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BouncePad.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

UCLASS()
class MOCAP_1_API ABouncePad : public AActor
{
	GENERATED_BODY()

public:
	ABouncePad();

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

	// Upward launch strength
	UPROPERTY(EditAnywhere, Category="Bounce")
	float VerticalLaunchStrength = 1800.f;

	// Forward push (0 = straight up)
	UPROPERTY(EditAnywhere, Category="Bounce")
	float ForwardLaunchStrength = 0.f;
};
