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
class UAudioComponent;
class USoundBase;

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

	// Slide boost charge bar (bottom-center): lazily created for the local
	// player on the first slide, added to the viewport while sliding and
	// removed on exit. The widget's own Tick fills its ProgressBar from
	// UMO_CharacterMovementComponent::GetSlideChargeFraction.
	UPROPERTY(EditDefaultsOnly, Category="HUD")
	TSubclassOf<UUserWidget> SlideChargeBarClass;

	// Slide foley: looping sound started when the slide begins and faded out
	// when it ends; its "Speed" MetaSound parameter is fed from velocity every
	// tick while playing (mirrors the Mover character's Foley.Event.Slide.Loop
	// + Update_SlidingAudio).
	UPROPERTY(EditDefaultsOnly, Category="Audio")
	TObjectPtr<USoundBase> SlideLoopSound;

	UPROPERTY(EditDefaultsOnly, Category="Audio")
	float SlideAudioFadeOutTime = 0.5f;

	// Called by UMO_CharacterMovementComponent on every role when the slide
	// movement mode is entered/left (simulated proxies included, via
	// replicated movement mode).
	virtual void OnSlideStarted();
	virtual void OnSlideEnded();

	// Mesh / eye-height fixups when the slide shrinks or restores the capsule,
	// mirroring ACharacter::OnStartCrouch/OnEndCrouch. The slide keeps its own
	// capsule path because GASP's stance logic owns the crouch system.
	// HalfHeightAdjust is measured from the class-default capsule.
	virtual void OnStartSlideCapsuleResize(float HalfHeightAdjust, float ScaledHalfHeightAdjust);
	virtual void OnEndSlideCapsuleResize(float HalfHeightAdjust, float ScaledHalfHeightAdjust);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(Transient, BlueprintReadOnly, Category="HUD")
	TObjectPtr<UUserWidget> PlayerHUDWidget;

	// Binds this character to the ASC living on MO_PlayerState.
	// Called from PossessedBy (server) and OnRep_PlayerState (client).
	virtual void InitAbilityActorInfo();

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	// Live while the slide foley loop plays; nulled on fade-out.
	UPROPERTY(Transient)
	TObjectPtr<UAudioComponent> SlidingAudioComponent;

	// Kept between slides; re-added to the viewport on each slide start.
	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> SlideChargeBarWidget;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
