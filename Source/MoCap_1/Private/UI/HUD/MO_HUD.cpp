// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/MO_HUD.h"

#include "Blueprint/UserWidget.h"
#include "UI/Widget/MO_UserWidget.h"
#include "UI/WidgetController/MO_OverlayWidgetController.h"

UMO_OverlayWidgetController* AMO_HUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UMO_OverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WCParams);
		OverlayWidgetController->BindCallbacks();
	}
	return OverlayWidgetController;
}

void AMO_HUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(OverlayWidgetClass, TEXT("Overlay widget class uninitialized, please fill out BP_MO_HUD"));
	checkf(OverlayWidgetControllerClass, TEXT("Overlay widget controller class uninitialized, please fill out BP_MO_HUD"));

	// Guard against double init: InitAbilityActorInfo can run more than once
	// (PossessedBy + OnRep_PlayerState on listen servers).
	if (OverlayWidget)
	{
		return;
	}

	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
	OverlayWidget = Cast<UMO_UserWidget>(Widget);

	const FWidgetControllerParams WCParams(PC, PS, ASC, AS);
	UMO_OverlayWidgetController* WidgetController = GetOverlayWidgetController(WCParams);

	OverlayWidget->SetWidgetController(WidgetController);
	WidgetController->BroadCastInitValues();
	Widget->AddToViewport();
}
