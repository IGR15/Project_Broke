// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpeedLinesWidget.generated.h"

class UImage;
class USpeedBoostComponent;

// HUD widget that fades a full-screen speed-lines image in while the owning
// pawn's SpeedBoostComponent is boosted. The widget Blueprint must contain an
// Image named "SpeedLinesImage" for the effect to show.
UCLASS()
class MOCAP_1_API USpeedLinesWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	UFUNCTION()
	void HandleSpeedBoostChanged(bool bActive, float Duration);

	// Seconds to fade the speed lines in or out.
	UPROPERTY(EditAnywhere, Category="SpeedLines")
	float FadeDuration = 0.25f;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UImage> SpeedLinesImage;

private:
	// Binds to the owning pawn's SpeedBoostComponent. Safe to call repeatedly;
	// used from NativeTick because the pawn is not yet reachable through the
	// player controller when the widget is created during PossessedBy.
	void TryBindToPawn();

	UPROPERTY()
	TObjectPtr<USpeedBoostComponent> BoundBoostComponent;

	float TargetOpacity = 0.f;
	float CurrentOpacity = 0.f;
};
