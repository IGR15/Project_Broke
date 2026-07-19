// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MO_BaseCharacter.h"

#include "AbilitySystemComponent.h"
#include "Character/MO_CharacterMovementComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/AudioComponent.h"
#include "Components/LaunchComponent.h"
#include "Components/SpeedBoostComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Player/MO_PlayerState.h"
#include "UI/HUD/MO_HUD.h"


// Sets default values
AMO_BaseCharacter::AMO_BaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UMO_CharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{

	LaunchComponent= CreateDefaultSubobject<ULaunchComponent>("LaunchComponent");
	SpeedBoostComponent = CreateDefaultSubobject<USpeedBoostComponent>("SpeedBoostComponent");
	PrimaryActorTick.bCanEverTick = true;
}

void AMO_BaseCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void AMO_BaseCharacter::OnStartSlideCapsuleResize(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	RecalculateBaseEyeHeight();

	// Shift the mesh up relative to the shrunken capsule so the visual stays
	// on the floor — same fixup ACharacter::OnStartCrouch does.
	const ACharacter* DefaultChar = GetDefault<ACharacter>(GetClass());
	if (GetMesh() && DefaultChar->GetMesh())
	{
		FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z = DefaultChar->GetMesh()->GetRelativeLocation().Z + HalfHeightAdjust;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
	}
	else
	{
		BaseTranslationOffset.Z = DefaultChar->GetBaseTranslationOffset().Z + HalfHeightAdjust;
	}
}

void AMO_BaseCharacter::OnEndSlideCapsuleResize(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	RecalculateBaseEyeHeight();

	const ACharacter* DefaultChar = GetDefault<ACharacter>(GetClass());
	if (GetMesh() && DefaultChar->GetMesh())
	{
		FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z = DefaultChar->GetMesh()->GetRelativeLocation().Z;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
	}
	else
	{
		BaseTranslationOffset.Z = DefaultChar->GetBaseTranslationOffset().Z;
	}
}

void AMO_BaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (LaunchComponent)
	{
		bIsObstacleLaunch = LaunchComponent->IsObstacleLaunch();
	}

	// Mover parity (Update_SlidingAudio): the loop's MetaSound scales with speed.
	if (SlidingAudioComponent)
	{
		SlidingAudioComponent->SetFloatParameter(TEXT("Speed"), GetVelocity().Size());
	}
}

void AMO_BaseCharacter::OnSlideStarted()
{
	// Charge bar is per-player UI: local players only.
	if (SlideChargeBarClass && IsLocallyControlled())
	{
		if (!SlideChargeBarWidget)
		{
			if (APlayerController* PC = Cast<APlayerController>(GetController()))
			{
				SlideChargeBarWidget = CreateWidget<UUserWidget>(PC, SlideChargeBarClass);
			}
		}
		if (SlideChargeBarWidget && !SlideChargeBarWidget->IsInViewport())
		{
			SlideChargeBarWidget->AddToViewport();
		}
	}

	// A client replay can re-enter the slide mode; don't stack loops.
	if (!SlideLoopSound || SlidingAudioComponent)
	{
		return;
	}

	SlidingAudioComponent = UGameplayStatics::SpawnSoundAttached(
		SlideLoopSound, GetMesh(), NAME_None, FVector::ZeroVector,
		EAttachLocation::KeepRelativeOffset, /*bStopWhenAttachedToDestroyed*/ true);
	if (SlidingAudioComponent)
	{
		SlidingAudioComponent->SetFloatParameter(TEXT("Speed"), GetVelocity().Size());
	}
}

bool AMO_BaseCharacter::CanJumpInternal_Implementation() const
{
	const UMO_CharacterMovementComponent* MOMovement = Cast<UMO_CharacterMovementComponent>(GetCharacterMovement());
	if (MOMovement && MOMovement->IsSliding())
	{
		// A crouch-driven slide is always IsCrouched(), which the base
		// implementation vetoes outright. Skip only that veto and keep the
		// rest: JumpIsAllowedInternal routes through the movement component's
		// CanAttemptJump, which allows the jump for the whole slide (jump
		// interrupts the slide; a full bar upgrades it to the boosted leap).
		return JumpIsAllowedInternal();
	}
	return Super::CanJumpInternal_Implementation();
}

void AMO_BaseCharacter::OnSlideEnded()
{
	if (SlideChargeBarWidget)
	{
		SlideChargeBarWidget->RemoveFromParent();
	}

	if (SlidingAudioComponent)
	{
		// FadeOut stops the component when it reaches zero; the pointer is
		// dropped now so a new slide can start a fresh loop over the tail.
		SlidingAudioComponent->FadeOut(SlideAudioFadeOutTime, 0.f);
		SlidingAudioComponent = nullptr;
	}
}

float AMO_BaseCharacter::GetSpeedBoostMultiplier() const
{
	return SpeedBoostComponent ? SpeedBoostComponent->GetCurrentSpeedMultiplier() : 1.f;
}

void AMO_BaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Init ability actor info for the server
	InitAbilityActorInfo();

	UE_LOG(LogTemp, Log, TEXT("MO_BaseCharacter::PossessedBy: %s possessed by %s (PlayerHUDClass=%s)"),
		*GetName(), *GetNameSafe(NewController), *GetNameSafe(PlayerHUDClass));

	if (PlayerHUDWidget || !PlayerHUDClass)
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(NewController);

	if (!PC || !PC->IsLocalController())
	{
		UE_LOG(LogTemp, Log, TEXT("MO_BaseCharacter::PossessedBy: skipping HUD creation (not a local player controller)"));
		return;
	}

	PlayerHUDWidget = CreateWidget<UUserWidget>(PC, PlayerHUDClass);

	UE_LOG(LogTemp, Log, TEXT("MO_BaseCharacter::PossessedBy: created HUD widget %s"), *GetNameSafe(PlayerHUDWidget));

	if (PlayerHUDWidget)
	{
		PlayerHUDWidget->AddToViewport();
	}
}

void AMO_BaseCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Init ability actor info for the client
	InitAbilityActorInfo();
}

UAbilitySystemComponent* AMO_BaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AMO_BaseCharacter::InitAbilityActorInfo()
{
	AMO_PlayerState* MO_PlayerState = GetPlayerState<AMO_PlayerState>();

	if (!MO_PlayerState)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("MO_BaseCharacter::InitAbilityActorInfo: PlayerState is not AMO_PlayerState. Set 'Player State Class' to MO_PlayerState in your GameMode."));
		return;
	}

	MO_PlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(MO_PlayerState, this);
	AbilitySystemComponent = MO_PlayerState->GetAbilitySystemComponent();

	// Aura pattern: the overlay comes up once the ASC is wired, from both the
	// server (PossessedBy) and client (OnRep_PlayerState) paths. GetHUD() is
	// only valid on the local player's controller.
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (AMO_HUD* MO_HUD = Cast<AMO_HUD>(PC->GetHUD()))
		{
			MO_HUD->InitOverlay(PC, MO_PlayerState, AbilitySystemComponent, /*AttributeSet*/ nullptr);
		}
	}
}

