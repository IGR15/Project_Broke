// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MO_KillVolume.generated.h"

class UBoxComponent;

/**
 * Out-of-bounds catcher. Stretch one (or several) beneath and around the
 * track; any racer pawn that falls in gets teleported back to the last
 * checkpoint they validly crossed (server-authoritative, delegated to
 * AMO_RaceGameState so respawn logic lives in one place).
 */
UCLASS()
class MOCAP_1_API AMO_KillVolume : public AActor
{
	GENERATED_BODY()

public:
	AMO_KillVolume();

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Race")
	TObjectPtr<UBoxComponent> Volume;
};
