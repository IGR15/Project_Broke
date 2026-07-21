// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/MO_ProjectileItemAbility.h"

#include "Actors/MO_HomingMissile.h"
#include "Actors/MO_ProjectileBase.h"
#include "GameFramework/Character.h"
#include "Player/MO_PlayerController.h"

void UMO_ProjectileItemAbility::OnItemActivated(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	// Actual spawning is server-only; the client side of this LocalPredicted
	// ability just ends immediately and picks up the replicated actor.
	if (!ActorInfo->IsNetAuthority())
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, /*bReplicateEndAbility*/ true, /*bWasCancelled*/ false);
		return;
	}

	ACharacter* AvatarCharacter = Cast<ACharacter>(ActorInfo->AvatarActor.Get());

	if (!AvatarCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: OnItemActivated - AvatarActor is not an ACharacter, no projectile spawned."), *GetName());
	}
	else if (!ProjectileClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s: OnItemActivated - ProjectileClass is not set, no projectile spawned."), *GetName());
	}
	else
	{
		const FVector Forward = AvatarCharacter->GetActorForwardVector();
		const FVector SpawnLocation =
			AvatarCharacter->GetActorLocation() + Forward * SpawnForwardOffset + FVector::UpVector * SpawnUpOffset;
		const FRotator SpawnRotation = Forward.Rotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = AvatarCharacter;
		SpawnParams.Instigator = AvatarCharacter;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AMO_ProjectileBase* Projectile = GetWorld()->SpawnActor<AMO_ProjectileBase>(
			ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);

		if (!Projectile)
		{
			UE_LOG(LogTemp, Error, TEXT("%s: OnItemActivated - SpawnActor(%s) failed."), *GetName(), *ProjectileClass->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("%s: spawned %s at %s"), *GetName(), *Projectile->GetName(), *SpawnLocation.ToString());

			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Cyan,
					FString::Printf(TEXT("Fired %s"), *Projectile->GetClass()->GetName()));
			}

			if (AMO_HomingMissile* Homing = Cast<AMO_HomingMissile>(Projectile))
			{
				AActor* Target = nullptr;
				if (AMO_PlayerController* PC = Cast<AMO_PlayerController>(ActorInfo->PlayerController.Get()))
				{
					Target = PC->GetAimedRacerTarget();
				}
				Homing->SetHomingTarget(Target);
				UE_LOG(LogTemp, Log, TEXT("%s: homing target = %s"), *GetName(), Target ? *Target->GetName() : TEXT("none"));
			}
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, /*bReplicateEndAbility*/ true, /*bWasCancelled*/ false);
}
