// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MO_UserWidget.generated.h"

/**
 * Base widget for the WidgetController pattern (Aura-style):
 * the widget holds an opaque controller reference and Blueprint reacts
 * to WidgetControllerSet to cast it and bind to the controller's delegates.
 */
UCLASS()
class MOCAP_1_API UMO_UserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UObject* InWidgetController);

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> WidgetController;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerSet();
};
