// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "MO_PlayerState.generated.h"

class UAbilitySystemComponent;

/**
 * PlayerState owning the player's AbilitySystemComponent (Aura-style placement),
 * so abilities survive pawn death/respawn.
 */
UCLASS()
class MOCAP_1_API AMO_PlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AMO_PlayerState();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
};
