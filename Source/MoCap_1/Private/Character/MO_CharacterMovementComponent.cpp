// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MO_CharacterMovementComponent.h"

#include "Character/MO_BaseCharacter.h"
#include "Kismet/KismetMathLibrary.h"

bool UMO_CharacterMovementComponent::IsSliding() const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == CMOVE_Slide;
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

	// Runs inside the predicted move on both the owning client and the server;
	// bWantsToSlide arrives on the server through the compressed flags, so
	// both sides evaluate the same input.
	if (IsSliding())
	{
		if (!bWantsToSlide || Velocity.Size2D() <= SlideExitSpeed)
		{
			SetMovementMode(MOVE_Walking);
		}
	}
	else if (MovementMode == MOVE_Walking && bWantsToSlide && !bSlideInputConsumed && Velocity.Size2D() > SlideEnterSpeed)
	{
		bSlideInputConsumed = true;
		SetMovementMode(MOVE_Custom, CMOVE_Slide);
	}
}

void UMO_CharacterMovementComponent::SetWantsToSlide(bool bNewWantsToSlide)
{
	bWantsToSlide = bNewWantsToSlide;
	if (!bNewWantsToSlide)
	{
		bSlideInputConsumed = false;
	}
}

void UMO_CharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);
	bWantsToSlide = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
	if (!bWantsToSlide)
	{
		bSlideInputConsumed = false;
	}
}

FNetworkPredictionData_Client* UMO_CharacterMovementComponent::GetPredictionData_Client() const
{
	check(PawnOwner != nullptr);

	if (ClientPredictionData == nullptr)
	{
		UMO_CharacterMovementComponent* MutableThis = const_cast<UMO_CharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_MO(*this);
	}
	return ClientPredictionData;
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
	}
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

void FSavedMove_MO::Clear()
{
	Super::Clear();
	bSavedWantsToSlide = 0;
}

uint8 FSavedMove_MO::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();
	if (bSavedWantsToSlide)
	{
		Result |= FLAG_Custom_0;
	}
	return Result;
}

bool FSavedMove_MO::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
{
	if (bSavedWantsToSlide != static_cast<FSavedMove_MO*>(NewMove.Get())->bSavedWantsToSlide)
	{
		return false;
	}
	return Super::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void FSavedMove_MO::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);
	bSavedWantsToSlide = CastChecked<UMO_CharacterMovementComponent>(C->GetCharacterMovement())->bWantsToSlide;
}

FSavedMovePtr FNetworkPredictionData_Client_MO::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_MO());
}
