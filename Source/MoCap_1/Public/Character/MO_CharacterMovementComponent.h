// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MO_CharacterMovementComponent.generated.h"

/**
 * CharacterMovementComponent with a networked slide, ported from the Game
 * Animation Sample's Mover-only slide (BP_MovementMode_Slide +
 * BP_MovementTransition_To/FromSlide) so CMC-based characters get it too.
 *
 * The slide runs as MOVE_Custom / CMOVE_Slide but reuses PhysWalking for its
 * physics — it is ground movement with slide-tuned speed/acceleration/braking
 * (see the Slide category below; defaults mirror the Mover BP values).
 *
 * Input: hold-to-slide via bWantsToSlide (bound to IA_Slide in
 * AMO_BaseCharacter), deliberately independent of the crouch system — GASP's
 * stance logic owns crouch and fights external bWantsToCrouch changes.
 *
 * Prediction: bWantsToSlide rides the saved-move compressed flags
 * (FLAG_Custom_0) exactly like crouch/jump, so the server sees the same input
 * the client predicted with. SlideElapsedTime (initial-boost phase) is not in
 * saved moves; a client replay can re-run the boost for up to 0.2s, which only
 * affects feel during a correction and self-heals with the next server update.
 */
UCLASS()
class MOCAP_1_API UMO_CharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	static constexpr uint8 CMOVE_Slide = 0;

	UFUNCTION(BlueprintPure, Category="Slide")
	bool IsSliding() const;

	// Hold-to-slide input intent (set from IA_Slide). Replicated to the
	// server through the saved-move compressed flags.
	UFUNCTION(BlueprintCallable, Category="Slide")
	void SetWantsToSlide(bool bNewWantsToSlide);

	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	virtual float GetMaxSpeed() const override;
	virtual float GetMaxAcceleration() const override;
	virtual float GetMaxBrakingDeceleration() const override;
	virtual bool IsMovingOnGround() const override;

protected:
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	// Crouching while moving faster than this on the ground starts a slide
	// (Mover: BP_MovementTransition_ToSlide).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Slide")
	float SlideEnterSpeed = 380.f;

	// Dropping to or below this speed (or releasing crouch) ends the slide
	// (Mover: BP_MovementTransition_FromSlide).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Slide")
	float SlideExitSpeed = 200.f;

	// Initial kick when the slide starts, then acceleration falls off.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Slide")
	float InitialBoostTime = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Slide")
	float InitialBoostSpeed = 800.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Slide")
	float InitialBoostAcceleration = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Slide")
	float AfterBoostAcceleration = 300.f;

	// Slope band the target speed / braking are mapped across. Slope angle is
	// measured against the velocity direction: negative = downhill.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Slide")
	float ShallowSlopeAngle = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Slide")
	float SteepSlopeAngle = 40.f;

	// Target speed on flat ground — low on purpose so a flat slide bleeds
	// speed and exits on its own.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Slide")
	float FlatGroundSpeed = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Slide")
	float ShallowSlopeSpeed = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Slide")
	float SteepSlopeSpeed = 800.f;

	// Braking while sliding, mapped from ShallowSlopeAngle (uphill) to
	// SteepSlopeAngle: flat/downhill brakes gently, uphill kills the slide.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Slide")
	float FlatGroundDeceleration = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Slide")
	float SteepSlopeDeceleration = 2000.f;

	// Ground friction while sliding. Near zero keeps momentum and limits
	// steering to AfterBoostAcceleration, matching the Mover feel.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Slide")
	float SlideGroundFriction = 0.f;

private:
	friend class FSavedMove_MO;

	// Input intent: player is holding the slide key.
	bool bWantsToSlide = false;

	// One slide per key press: set when a slide starts, cleared when the key
	// is released. Without this, holding slide while sprinting on flat ground
	// oscillates enter→speed-bleed→exit→sprint→enter, which reads as jerking
	// and never lets the slide animation settle. Derived deterministically
	// from bWantsToSlide on both client and server, so it needs no saved-move
	// data of its own.
	bool bSlideInputConsumed = false;

	// Seconds spent in the current slide; drives the initial-boost phase.
	float SlideElapsedTime = 0.f;

	// Signed slope angle in degrees relative to travel direction: negative =
	// downhill, positive = uphill, 0 on flat ground or with no valid floor.
	float GetSlideSlopeAngle() const;
};

// Saved move carrying bWantsToSlide in FLAG_Custom_0 so the server replays
// the same slide input the owning client predicted with.
class FSavedMove_MO : public FSavedMove_Character
{
	using Super = FSavedMove_Character;

public:
	uint8 bSavedWantsToSlide : 1;

	FSavedMove_MO() : bSavedWantsToSlide(0) {}

	virtual void Clear() override;
	virtual uint8 GetCompressedFlags() const override;
	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
	virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData) override;
};

class FNetworkPredictionData_Client_MO : public FNetworkPredictionData_Client_Character
{
	using Super = FNetworkPredictionData_Client_Character;

public:
	FNetworkPredictionData_Client_MO(const UCharacterMovementComponent& ClientMovement) : Super(ClientMovement) {}

	virtual FSavedMovePtr AllocateNewMove() override;
};
