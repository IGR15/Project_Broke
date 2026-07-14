// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/MO_WidgetController.h"

#include "AbilitySystem/MO_AbilitySystemComponent.h"
#include "Player/MO_PlayerController.h"
#include "Player/MO_PlayerState.h"

void UMO_WidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UMO_WidgetController::BroadCastInitValues()
{
}

void UMO_WidgetController::BindCallbacks()
{
}

AMO_PlayerController* UMO_WidgetController::GetMOPC()
{
	if (MO_PlayerController == nullptr)
	{
		MO_PlayerController = Cast<AMO_PlayerController>(PlayerController);
	}
	return MO_PlayerController;
}

AMO_PlayerState* UMO_WidgetController::GetMOPS()
{
	if (MO_PlayerState == nullptr)
	{
		MO_PlayerState = Cast<AMO_PlayerState>(PlayerState);
	}
	return MO_PlayerState;
}

UMO_AbilitySystemComponent* UMO_WidgetController::GetMOASC()
{
	if (MO_AbilitySystemComponent == nullptr)
	{
		MO_AbilitySystemComponent = Cast<UMO_AbilitySystemComponent>(AbilitySystemComponent);
	}
	return MO_AbilitySystemComponent;
}
