// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SpeedLinesWidget.h"

#include "Components/Image.h"
#include "Components/SpeedBoostComponent.h"
#include "GameFramework/Pawn.h"

void USpeedLinesWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (SpeedLinesImage)
	{
		SpeedLinesImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		SpeedLinesImage->SetRenderOpacity(0.f);
	}

	TryBindToPawn();
}

void USpeedLinesWidget::TryBindToPawn()
{
	APawn* Pawn = GetOwningPlayerPawn();

	if (!Pawn)
	{
		return;
	}

	BoundBoostComponent = Pawn->FindComponentByClass<USpeedBoostComponent>();

	if (BoundBoostComponent)
	{
		BoundBoostComponent->OnSpeedBoostChanged.AddUniqueDynamic(
			this,
			&USpeedLinesWidget::HandleSpeedBoostChanged);

		TargetOpacity = BoundBoostComponent->IsBoosted() ? 1.f : 0.f;

		UE_LOG(LogTemp, Log, TEXT("SpeedLinesWidget: bound to %s on %s (SpeedLinesImage=%s, boosted=%d)"),
			*BoundBoostComponent->GetName(), *Pawn->GetName(),
			*GetNameSafe(SpeedLinesImage), BoundBoostComponent->IsBoosted() ? 1 : 0);
	}
}

void USpeedLinesWidget::NativeDestruct()
{
	if (BoundBoostComponent)
	{
		BoundBoostComponent->OnSpeedBoostChanged.RemoveDynamic(
			this,
			&USpeedLinesWidget::HandleSpeedBoostChanged);
		BoundBoostComponent = nullptr;
	}

	Super::NativeDestruct();
}

void USpeedLinesWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// The pawn is not possessed yet when the widget is constructed, and the
	// component goes away on respawn, so (re)bind lazily here.
	if (!IsValid(BoundBoostComponent))
	{
		TryBindToPawn();
	}

	if (!SpeedLinesImage || CurrentOpacity == TargetOpacity)
	{
		return;
	}

	const float FadeSpeed = FadeDuration > 0.f ? 1.f / FadeDuration : BIG_NUMBER;
	CurrentOpacity = FMath::FInterpConstantTo(CurrentOpacity, TargetOpacity, InDeltaTime, FadeSpeed);
	SpeedLinesImage->SetRenderOpacity(CurrentOpacity);
}

void USpeedLinesWidget::HandleSpeedBoostChanged(bool bActive, float Duration)
{
	UE_LOG(LogTemp, Log, TEXT("SpeedLinesWidget: boost %s (duration %.1fs)"),
		bActive ? TEXT("started") : TEXT("ended"), Duration);

	TargetOpacity = bActive ? 1.f : 0.f;
}
