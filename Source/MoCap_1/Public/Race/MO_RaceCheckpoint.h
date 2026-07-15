// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MO_RaceCheckpoint.generated.h"

class UBoxComponent;

/**
 * Ordered race gate. Place several around the course with contiguous indices
 * starting at 0 (0 = the start/finish line); racers must cross them in order
 * for a lap to count, which is what stops reversing/shortcutting from
 * advancing laps. Registration happens in PostInitializeComponents so every
 * gate is known before AMO_RaceTrack registers on BeginPlay.
 */
UCLASS()
class MOCAP_1_API AMO_RaceCheckpoint : public AActor
{
	GENERATED_BODY()

public:
	AMO_RaceCheckpoint();

	virtual void PostInitializeComponents() override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Race")
	TObjectPtr<UBoxComponent> Trigger;

	// 0 = start/finish line; must be contiguous around the course.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Race", meta=(ClampMin=0))
	int32 CheckpointIndex = 0;
};
