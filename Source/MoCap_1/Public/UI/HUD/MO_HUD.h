// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MO_HUD.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class UMO_OverlayWidgetController;
class UMO_UserWidget;
struct FWidgetControllerParams;

/**
 * Game HUD (Aura pattern): owns the overlay widget and lazily creates its
 * widget controller. InitOverlay is called from
 * MO_BaseCharacter::InitAbilityActorInfo once the ASC is ready.
 */
UCLASS()
class MOCAP_1_API AMO_HUD : public AHUD
{
	GENERATED_BODY()

public:
	UMO_OverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WCParams);

	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);

private:
	UPROPERTY()
	TObjectPtr<UMO_UserWidget> OverlayWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UMO_UserWidget> OverlayWidgetClass;

	UPROPERTY()
	TObjectPtr<UMO_OverlayWidgetController> OverlayWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UMO_OverlayWidgetController> OverlayWidgetControllerClass;
};
