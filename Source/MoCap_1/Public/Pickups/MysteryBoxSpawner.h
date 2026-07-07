// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MysteryBoxSpawner.generated.h"

class AMysteryBox;

UCLASS()
class MOCAP_1_API AMysteryBoxSpawner : public AActor
{
	GENERATED_BODY()

public:
	AMysteryBoxSpawner();

protected:
	virtual void BeginPlay() override;

	void CheckAndSpawn();

public:

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	// Class of mystery box to spawn
	UPROPERTY(EditAnywhere, Category="MysteryBoxSpawner")
	TSubclassOf<AMysteryBox> MysteryBoxClass;

	// How often to check whether a box needs to be (re)spawned
	UPROPERTY(EditAnywhere, Category="MysteryBoxSpawner")
	float CheckInterval = 5.f;

private:
	// The box this spawner currently owns, if any. Becomes invalid once the box is destroyed (collected).
	UPROPERTY()
	AMysteryBox* SpawnedBox;

	FTimerHandle CheckTimerHandle;
};
