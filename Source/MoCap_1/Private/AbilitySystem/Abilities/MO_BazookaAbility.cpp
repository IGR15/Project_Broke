// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/MO_BazookaAbility.h"

#include "Actors/MO_Missile.h"

UMO_BazookaAbility::UMO_BazookaAbility()
{
	ItemTagName = FName("Abilities.Item.Bazooka");
	ProjectileClass = AMO_Missile::StaticClass();
}
