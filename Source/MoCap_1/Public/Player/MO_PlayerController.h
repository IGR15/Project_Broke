// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "MO_PlayerController.generated.h"

class UInputMappingContext;
class UMO_InputConfig;
class UMO_AbilitySystemComponent;

/**
 * PlayerController that owns the ability input layer (Aura pattern):
 * adds the ability InputMappingContext and forwards InputTag
 * Pressed/Released/Held events to the ASC on MO_PlayerState.
 */
UCLASS()
class MOCAP_1_API AMO_PlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	// Mapping context holding the ability key bindings (e.g. LMB -> IA_UseItem).
	// Added on top of whatever contexts the pawn adds for movement.
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> AbilityMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UMO_InputConfig> InputConfig;

	// Priority for AbilityMappingContext; keep above the pawn's movement context
	// so ability keys are consumed first if they ever overlap.
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	int32 AbilityMappingContextPriority = 1;

public:
	// Racer the camera is currently pointed at: nearest AMO_BaseCharacter
	// within MaxAngleDegrees of camera-forward and MaxDistance, with an
	// unobstructed line of sight from the camera. Used by homing item
	// abilities (e.g. the bazooka's homing missile) to pick a lock-on target
	// at the moment of firing - it does not track a sustained "aim mode".
	// Returns nullptr if nothing qualifies.
	UFUNCTION(BlueprintCallable, Category = "Targeting")
	AActor* GetAimedRacerTarget(float MaxDistance = 5000.f, float MaxAngleDegrees = 15.f) const;

private:
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	UMO_AbilitySystemComponent* GetASC();

	UPROPERTY()
	TObjectPtr<UMO_AbilitySystemComponent> MO_AbilitySystemComponent;
};
