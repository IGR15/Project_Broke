// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "MO_BaseCharacter.generated.h"

class ULaunchComponent;
class USpeedBoostComponent;
class UUserWidget;
class UAbilitySystemComponent;
class UInputAction;

UCLASS()
class MOCAP_1_API AMO_BaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// FObjectInitializer ctor so the CharacterMovementComponent can be swapped
	// for UMO_CharacterMovementComponent (slide support).
	AMO_BaseCharacter(const FObjectInitializer& ObjectInitializer);
	UPROPERTY(BlueprintReadOnly, Category="Launch")
	bool bIsObstacleLaunch = false;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<ULaunchComponent> LaunchComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpeedBoostComponent> SpeedBoostComponent;

	// Multiplier from an active speed boost (1.0 when not boosted). Blueprint-side
	// max-speed calculations (e.g. CalculateMaxSpeed) should multiply their result by this.
	UFUNCTION(BlueprintCallable, Category="Speed")
	float GetSpeedBoostMultiplier() const;

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// HUD widget created for the local player on possession.
	UPROPERTY(EditDefaultsOnly, Category="HUD")
	TSubclassOf<UUserWidget> PlayerHUDClass;

	// Hold-to-slide input (LeftControl). Forwarded to
	// UMO_CharacterMovementComponent::SetWantsToSlide.
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> SlideAction;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(Transient, BlueprintReadOnly, Category="HUD")
	TObjectPtr<UUserWidget> PlayerHUDWidget;

	// Binds this character to the ASC living on MO_PlayerState.
	// Called from PossessedBy (server) and OnRep_PlayerState (client).
	virtual void InitAbilityActorInfo();

	void OnSlidePressed();
	void OnSlideReleased();

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
