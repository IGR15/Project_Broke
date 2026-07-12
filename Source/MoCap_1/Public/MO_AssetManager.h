// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "MO_AssetManager.generated.h"

/**
 * Custom AssetManager. Registers the project's native gameplay tags at startup.
 * Set as AssetManagerClassName in DefaultEngine.ini.
 */
UCLASS()
class MOCAP_1_API UMO_AssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	static UMO_AssetManager& Get();

protected:
	virtual void StartInitialLoading() override;
};
