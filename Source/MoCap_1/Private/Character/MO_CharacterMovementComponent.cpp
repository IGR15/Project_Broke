// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MO_CharacterMovementComponent.h"

#include "Character/MO_BaseCharacter.h"
#include "CollisionQueryParams.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

bool UMO_CharacterMovementComponent::IsSliding() const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == CMOVE_Slide;
}

float UMO_CharacterMovementComponent::GetSlideChargeFraction() const
{
	if (!IsSliding() || SlideChargeTime <= 0.f)
	{
		return 0.f;
	}
	return FMath::Clamp(SlideElapsedTime / SlideChargeTime, 0.f, 1.f);
}

bool UMO_CharacterMovementComponent::IsSlideJumpBoostReady() const
{
	return IsSliding() && GetSlideChargeFraction() >= 1.f;
}

bool UMO_CharacterMovementComponent::DoJump(bool bReplayingMoves, float DeltaTime)
{
	if (!IsSliding())
	{
		return Super::DoJump(bReplayingMoves, DeltaTime);
	}

	// Jumping interrupts the slide at any charge. Clear the crouch intent
	// before Super: it flips to falling, and the slide-exit capsule restore
	// that triggers reads bWantsToCrouch for its target height — cleared
	// now, the character goes straight to standing and lands as a stand,
	// not a crouch.
	bWantsToCrouch = false;

	if (!IsSlideJumpBoostReady())
	{
		// Plain jump out of the slide; an unfilled charge is forfeited.
		return Super::DoJump(bReplayingMoves, DeltaTime);
	}

	// Boosted leap out of the slide. Apex height scales with JumpZ² and
	// airtime with JumpZ, so height ×H needs JumpZ ×√H; distance ×D over the
	// ×√H airtime needs horizontal speed ×(D/√H). Runs inside the predicted
	// move on client and server, and both derive the ready state from the
	// same simulation, so no extra replication is needed.
	const float JumpZFactor = FMath::Sqrt(FMath::Max(SlideBoostJumpHeightMultiplier, 0.f));
	const float HorizontalFactor = JumpZFactor > 0.f ? SlideBoostJumpDistanceMultiplier / JumpZFactor : 0.f;

	TGuardValue<float> JumpZGuard(JumpZVelocity, JumpZVelocity * JumpZFactor);
	if (!Super::DoJump(bReplayingMoves, DeltaTime))
	{
		return false;
	}

	// Super already flipped us to falling, which ends the slide (bar/foley
	// stop via OnMovementModeChanged). Scale the carried slide speed for the
	// ×D travel.
	Velocity.X *= HorizontalFactor;
	Velocity.Y *= HorizontalFactor;
	return true;
}

bool UMO_CharacterMovementComponent::CanAttemptJump() const
{
	// The !bWantsToCrouch veto in Super would make jumping out of a
	// crouch-driven slide impossible — it always wants crouch.
	if (IsSliding())
	{
		return IsJumpAllowed();
	}
	return Super::CanAttemptJump();
}

bool UMO_CharacterMovementComponent::IsMovingOnGround() const
{
	return (MovementMode == MOVE_Walking || MovementMode == MOVE_NavWalking || IsSliding()) && UpdatedComponent;
}

float UMO_CharacterMovementComponent::GetMaxSpeed() const
{
	if (!IsSliding())
	{
		return Super::GetMaxSpeed();
	}

	float TargetSpeed;
	if (SlideElapsedTime < InitialBoostTime)
	{
		TargetSpeed = InitialBoostSpeed;
	}
	else
	{
		const float SlopeAngle = GetSlideSlopeAngle();
		TargetSpeed = SlopeAngle > -ShallowSlopeAngle
			? FlatGroundSpeed
			: UKismetMathLibrary::MapRangeClamped(SlopeAngle, -ShallowSlopeAngle, -SteepSlopeAngle, ShallowSlopeSpeed, SteepSlopeSpeed);
	}

	// Speed pads still work mid-slide.
	const AMO_BaseCharacter* MOCharacter = Cast<AMO_BaseCharacter>(CharacterOwner);
	return MOCharacter ? TargetSpeed * MOCharacter->GetSpeedBoostMultiplier() : TargetSpeed;
}

float UMO_CharacterMovementComponent::GetMaxAcceleration() const
{
	if (!IsSliding())
	{
		return Super::GetMaxAcceleration();
	}
	return SlideElapsedTime < InitialBoostTime ? InitialBoostAcceleration : AfterBoostAcceleration;
}

float UMO_CharacterMovementComponent::GetMaxBrakingDeceleration() const
{
	if (!IsSliding())
	{
		return Super::GetMaxBrakingDeceleration();
	}
	return UKismetMathLibrary::MapRangeClamped(GetSlideSlopeAngle(), ShallowSlopeAngle, SteepSlopeAngle, FlatGroundDeceleration, SteepSlopeDeceleration);
}

void UMO_CharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	// Super handles crouch/uncrouch from bWantsToCrouch first, so the capsule
	// state is settled before the slide decision.
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);

	if (!HasValidData())
	{
		return;
	}

	// Crouch-driven like Mover's To/FromSlide: Super above already processed
	// bWantsToCrouch (the C toggle), so a press at speed crouches and slides
	// in the same tick. bWantsToCrouch rides the native compressed flags, so
	// client and server evaluate the same input.
	if (IsSliding())
	{
		if (!bWantsToCrouch || Velocity.Size2D() <= SlideExitSpeed)
		{
			// Mover parity: FromSlide clears the crouch intent, so the
			// character stands out of the slide instead of settling into
			// crouch-walk. The C toggle branches on bIsCrouched (which the
			// base crouch update clears right after), so it stays consistent.
			bWantsToCrouch = false;

			// Standing up needs headroom: while something blocks the restored
			// capsule the slide keeps going (steerable with the slide
			// acceleration) and this retries every tick.
			if (TryRestoreCapsuleAfterSlide())
			{
				SetMovementMode(MOVE_Walking);
			}
		}
	}
	else
	{
		if (bSlideCapsuleShrunk)
		{
			// Slide ended outside the normal exit path (e.g. slid off a ledge
			// into falling) with the capsule still small — keep retrying.
			TryRestoreCapsuleAfterSlide();
		}
		if (MovementMode == MOVE_Walking && bWantsToCrouch && Velocity.Size2D() > SlideEnterSpeed)
		{
			SetMovementMode(MOVE_Custom, CMOVE_Slide);
		}
	}
}

void UMO_CharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);

	if (CustomMovementMode != CMOVE_Slide)
	{
		return;
	}

	SlideElapsedTime += deltaTime;

	// The slide is walking with slide-tuned values: GetMaxSpeed /
	// GetMaxAcceleration / GetMaxBrakingDeceleration above feed CalcVelocity,
	// and the friction guard limits steering to the acceleration alone.
	TGuardValue<float> FrictionGuard(GroundFriction, SlideGroundFriction);
	PhysWalking(deltaTime, Iterations);
}

void UMO_CharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

	if (IsSliding())
	{
		SlideElapsedTime = 0.f;

		// Shrink before the floor search below so the floor is found from the
		// capsule's slide-size location. Runs on every role — simulated
		// proxies land here through replicated movement mode.
		ShrinkCapsuleForSlide();

		// Base OnMovementModeChanged treats MOVE_Custom as airborne and clears
		// CurrentFloor + movement base. The slide is ground movement, so
		// restore them — without a walkable floor, PhysWalking's first
		// iteration skips MoveAlongFloor and then zeroes Velocity from the
		// (empty) actual move, killing the slide one frame after it starts.
		FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, false);
		if (CurrentFloor.IsWalkableFloor())
		{
			AdjustFloorHeight();
			SetBaseFromFloor(CurrentFloor);
		}

		if (AMO_BaseCharacter* MOCharacter = Cast<AMO_BaseCharacter>(CharacterOwner))
		{
			MOCharacter->OnSlideStarted();
		}
	}
	else if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == CMOVE_Slide)
	{
		if (bSlideCapsuleShrunk)
		{
			// Slide left through something other than the guarded exit in
			// UpdateCharacterStateBeforeMovement (ledge fall, teleport, proxy
			// replication). If this fails the per-tick retry picks it up.
			TryRestoreCapsuleAfterSlide();
		}

		if (AMO_BaseCharacter* MOCharacter = Cast<AMO_BaseCharacter>(CharacterOwner))
		{
			MOCharacter->OnSlideEnded();
		}
	}
}

void UMO_CharacterMovementComponent::ShrinkCapsuleForSlide()
{
	if (!HasValidData())
	{
		return;
	}

	UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
	const float OldUnscaledHalfHeight = Capsule->GetUnscaledCapsuleHalfHeight();
	const float OldUnscaledRadius = Capsule->GetUnscaledCapsuleRadius();
	const float ClampedSlideHalfHeight = FMath::Max3(0.f, OldUnscaledRadius, SlideCapsuleHalfHeight);

	if (OldUnscaledHalfHeight == ClampedSlideHalfHeight)
	{
		bSlideCapsuleShrunk = true;
		return;
	}

	const float ComponentScale = Capsule->GetShapeScale();
	Capsule->SetCapsuleSize(OldUnscaledRadius, ClampedSlideHalfHeight);
	const float ScaledHalfHeightAdjust = (OldUnscaledHalfHeight - ClampedSlideHalfHeight) * ComponentScale;

	// Slides start from walking, so drop the capsule center to keep the feet
	// planted (what Crouch does under bCrouchMaintainsBaseLocation — that flag
	// itself is false here because base OnMovementModeChanged already cleared
	// it for MOVE_Custom). Simulated proxies take their location from the
	// server instead.
	if (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)
	{
		UpdatedComponent->MoveComponent(ScaledHalfHeightAdjust * GetGravityDirection(), UpdatedComponent->GetComponentQuat(), true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
	}
	bForceNextFloorCheck = true;
	bSlideCapsuleShrunk = true;

	// Mesh offset is measured from the default capsule, like OnStartCrouch.
	const ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
	const float HalfHeightAdjust = DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - ClampedSlideHalfHeight;
	if (AMO_BaseCharacter* MOCharacter = Cast<AMO_BaseCharacter>(CharacterOwner))
	{
		MOCharacter->OnStartSlideCapsuleResize(HalfHeightAdjust, HalfHeightAdjust * ComponentScale);
	}

	// Keep smoothing from easing the mesh through the sudden capsule change
	// on simulated proxies and listen-server remote pawns (same as Crouch).
	if (CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy || (IsNetMode(NM_ListenServer) && CharacterOwner->GetRemoteRole() == ROLE_AutonomousProxy))
	{
		if (FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character())
		{
			ClientData->MeshTranslationOffset -= ScaledHalfHeightAdjust * -GetGravityDirection();
			ClientData->OriginalMeshTranslationOffset = ClientData->MeshTranslationOffset;
		}
	}
}

bool UMO_CharacterMovementComponent::TryRestoreCapsuleAfterSlide()
{
	if (!HasValidData())
	{
		return false;
	}

	// Normal exits cleared the crouch intent, so the target is standing height
	// and the base crouch update's UnCrouch early-out cleans up bIsCrouched a
	// tick later. Only exits that still want crouch (e.g. sliding off a ledge
	// while the toggle is on) restore to the crouched height instead.
	const ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
	UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
	const float DefaultUnscaledHalfHeight = DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float TargetUnscaledHalfHeight = bWantsToCrouch
		? FMath::Max3(0.f, Capsule->GetUnscaledCapsuleRadius(), GetCrouchedHalfHeight())
		: DefaultUnscaledHalfHeight;

	if (Capsule->GetUnscaledCapsuleHalfHeight() == TargetUnscaledHalfHeight)
	{
		bSlideCapsuleShrunk = false;
		return true;
	}

	const float CurrentScaledHalfHeight = Capsule->GetScaledCapsuleHalfHeight();
	const float ComponentScale = Capsule->GetShapeScale();
	const float HalfHeightAdjust = TargetUnscaledHalfHeight - Capsule->GetUnscaledCapsuleHalfHeight();
	const float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;
	const FVector PawnLocation = UpdatedComponent->GetComponentLocation();

	// Simulated proxies mirror the server's authoritative result, so only
	// autonomous/authority run the encroachment test (UnCrouch does the same
	// under bClientSimulation).
	if (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)
	{
		const UWorld* MyWorld = GetWorld();
		const float SweepInflation = UE_KINDA_SMALL_NUMBER * 10.f;
		FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(SlideStandUpTrace), false, CharacterOwner);
		FCollisionResponseParams ResponseParam;
		InitCollisionParams(CapsuleParams, ResponseParam);
		const FCollisionShape TargetCapsuleShape = GetPawnCapsuleCollisionShape(SHRINK_HeightCustom, -SweepInflation - ScaledHalfHeightAdjust); // negative shrink = grow to the target size
		const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();
		bool bEncroached = true;

		if (IsMovingOnGround())
		{
			// Grow while keeping the capsule bottom (feet) in place.
			FVector TargetLocation = PawnLocation + (TargetCapsuleShape.GetCapsuleHalfHeight() - CurrentScaledHalfHeight) * -GetGravityDirection();
			bEncroached = MyWorld->OverlapBlockingTestByChannel(TargetLocation, GetWorldToGravityTransform(), CollisionChannel, TargetCapsuleShape, CapsuleParams, ResponseParam);

			if (bEncroached && CurrentFloor.bBlockingHit && CurrentFloor.FloorDist > SweepInflation)
			{
				// Something might be just barely overhead; hug the floor and retry.
				TargetLocation -= (CurrentFloor.FloorDist - SweepInflation) * -GetGravityDirection();
				bEncroached = MyWorld->OverlapBlockingTestByChannel(TargetLocation, GetWorldToGravityTransform(), CollisionChannel, TargetCapsuleShape, CapsuleParams, ResponseParam);
			}

			if (bEncroached)
			{
				return false;
			}

			UpdatedComponent->MoveComponent(TargetLocation - PawnLocation, UpdatedComponent->GetComponentQuat(), false, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
			bForceNextFloorCheck = true;
		}
		else
		{
			// Airborne (slid off a ledge): expand in place.
			bEncroached = MyWorld->OverlapBlockingTestByChannel(PawnLocation, GetWorldToGravityTransform(), CollisionChannel, TargetCapsuleShape, CapsuleParams, ResponseParam);
			if (bEncroached)
			{
				return false;
			}
		}
	}

	Capsule->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), TargetUnscaledHalfHeight, true);
	bSlideCapsuleShrunk = false;

	// Mesh Z is always measured from the class-default capsule: a crouched
	// target keeps the crouch-style offset, a standing target resets it.
	if (AMO_BaseCharacter* MOCharacter = Cast<AMO_BaseCharacter>(CharacterOwner))
	{
		const float MeshAdjustFromDefault = DefaultUnscaledHalfHeight - TargetUnscaledHalfHeight;
		if (MeshAdjustFromDefault != 0.f)
		{
			MOCharacter->OnStartSlideCapsuleResize(MeshAdjustFromDefault, MeshAdjustFromDefault * ComponentScale);
		}
		else
		{
			MOCharacter->OnEndSlideCapsuleResize(HalfHeightAdjust, ScaledHalfHeightAdjust);
		}
	}

	if (CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy || (IsNetMode(NM_ListenServer) && CharacterOwner->GetRemoteRole() == ROLE_AutonomousProxy))
	{
		if (FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character())
		{
			ClientData->MeshTranslationOffset += ScaledHalfHeightAdjust * -GetGravityDirection();
			ClientData->OriginalMeshTranslationOffset = ClientData->MeshTranslationOffset;
		}
	}

	return true;
}

float UMO_CharacterMovementComponent::GetSlideSlopeAngle() const
{
	if (!CurrentFloor.IsWalkableFloor())
	{
		return 0.f;
	}

	const FVector VelocityDirection = Velocity.GetSafeNormal2D();
	if (VelocityDirection.IsNearlyZero())
	{
		return 0.f;
	}

	// A slope's normal leans horizontally toward the descent, so this comes
	// out negative when moving downhill and positive uphill (Mover's
	// GenerateWalkMove uses the same construction).
	const float Dot = FVector::DotProduct(CurrentFloor.HitResult.ImpactNormal, VelocityDirection);
	return UKismetMathLibrary::DegAcos(Dot) - 90.f;
}

