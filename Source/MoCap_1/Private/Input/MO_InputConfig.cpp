// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/MO_InputConfig.h"

const UInputAction* UMO_InputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, const bool bLogNotFound) const
{
	for (const FMO_InputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag == InputTag)
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find InputAction for tag [%s] on InputConfig [%s]"),
			*InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}
