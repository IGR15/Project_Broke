// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/MO_UserWidget.h"

void UMO_UserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}
