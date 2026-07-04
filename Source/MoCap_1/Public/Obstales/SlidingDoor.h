// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SlidingDoor.generated.h"

class UStaticMeshComponent;
UCLASS()
class MOCAP_1_API ASlidingDoor : public AActor
{
	GENERATED_BODY()

public:
	ASlidingDoor();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* DoorMesh;

	UPROPERTY(EditAnywhere, Category="Door")
	bool bSlideLeft = true;

	UPROPERTY(EditAnywhere, Category="Door")
	float SlideDistance = 300.f;

	UPROPERTY(EditAnywhere, Category="Door")
	float SlideSpeed = 250.f;

	UPROPERTY(EditAnywhere, Category="Door")
	float HoldTime = 0.5f;

private:

	FVector StartLocation;
	FVector OpenLocation;
	FVector TargetLocation;

	float HoldTimer = 0.f;
	bool bGoingToOpen = true;
};