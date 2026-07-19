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
 * Input: crouch-driven like the Mover version — crouching (C toggle) while
 * moving faster than SlideEnterSpeed starts the slide; toggling crouch off or
 * bleeding below SlideExitSpeed ends it, and the exit clears the crouch
 * intent (Mover's FromSlide does the same) so the character stands back up.
 * bWantsToCrouch already rides the native compressed flags, so no custom
 * saved-move data is needed.
 *
 * Prediction: SlideElapsedTime (initial-boost phase) is not in saved moves; a
 * client replay can re-run the boost for up to 0.2s, which only affects feel
 * during a correction and self-heals with the next server update.
 */
UCLASS()
class MOCAP_1_API UMO_CharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	static constexpr uint8 CMOVE_Slide = 0;

	UFUNCTION(BlueprintPure, Category="Slide")
	bool IsSliding() const;

	// 0..1 fill of the boost charge for the current slide (SlideElapsedTime /
	// SlideChargeTime); 0 when not sliding. The charge-bar UI polls this.
	UFUNCTION(BlueprintPure, Category="Slide")
	float GetSlideChargeFraction() const;

	// True while sliding with a full charge bar: jumping now performs the
	// boosted leap. Exiting the slide without jumping forfeits the charge
	// (use it or lose it, like a drift boost).
	UFUNCTION(BlueprintPure, Category="Slide")
	bool IsSlideJumpBoostReady() const;

	virtual bool DoJump(bool bReplayingMoves, float DeltaTime) override;

	// Base CanAttemptJump vetoes on bWantsToCrouch, which a crouch-driven
	// slide always has set. Jumping is allowed at any point of a slide and
	// interrupts it (full bar = boosted leap, otherwise a plain jump), so
	// skip the veto while sliding.
	virtual bool CanAttemptJump() const override;

	virtual float GetMaxSpeed() const override;
	virtual float GetMaxAcceleration() const override;
	virtual float GetMaxBrakingDeceleration() const override;
	virtual bool IsMovingOnGround() const override;

protected:
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	// Crouching while moving faster than this on the ground starts a slide;
	// slower presses just crouch. Sits between GASP's run (500) and sprint
	// (700) forward speeds so only a sprinting character slides (Mover's
	// BP_MovementTransition_ToSlide used 380, which run speed also cleared).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Slide")
	float SlideEnterSpeed = 600.f;

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

	// Capsule half height while sliding (standing height comes from the class
	// defaults). Clamped to the capsule radius. Lets the slide pass under
	// obstacles the standing capsule would hit.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Slide")
	float SlideCapsuleHalfHeight = 45.f;

	// Seconds of continuous sliding needed to fill the boost charge bar.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Slide")
	float SlideChargeTime = 2.f;

	// Boosted leap paid for by a full charge bar, as multiples of a regular
	// jump: apex height ×H (jump velocity scales by √H) and horizontal travel
	// ×D over the √H-longer airtime (horizontal speed scales by D/√H).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Slide")
	float SlideBoostJumpHeightMultiplier = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Slide")
	float SlideBoostJumpDistanceMultiplier = 5.f;

	// Shrinks the capsule to SlideCapsuleHalfHeight, keeping the feet planted,
	// and offsets the mesh through AMO_BaseCharacter::OnStartSlideCapsuleResize.
	// Mirrors UCharacterMovementComponent::Crouch, but stays independent of the
	// crouch system because GASP's stance logic owns that.
	void ShrinkCapsuleForSlide();

	// Restores the capsule if the restored size fits where the character is
	// (UnCrouch-style encroachment test). While still crouched — the usual end
	// of a crouch-driven slide — the target is the crouched height and UnCrouch
	// grows the rest when the player stands; otherwise the standing default.
	// Returns false while blocked overhead — callers keep the slide (or retry
	// next tick) until it succeeds.
	bool TryRestoreCapsuleAfterSlide();

private:
	// Seconds spent in the current slide; drives the initial-boost phase.
	float SlideElapsedTime = 0.f;

	// True while the capsule is at slide size. Like SlideElapsedTime this is
	// derived from movement-mode transitions rather than saved-move data; the
	// resize operations are idempotent, so a client replay re-running them is
	// harmless.
	bool bSlideCapsuleShrunk = false;

	// Signed slope angle in degrees relative to travel direction: negative =
	// downhill, positive = uphill, 0 on flat ground or with no valid floor.
	float GetSlideSlopeAngle() const;
};
