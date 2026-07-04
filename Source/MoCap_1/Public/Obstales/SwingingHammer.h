#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SwingingHammer.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class UBoxComponent;

UCLASS()
class MOCAP_1_API ASwingingHammer : public AActor
{
	GENERATED_BODY()

public:
	ASwingingHammer();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
	void OnOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

public:
	UPROPERTY(EditAnywhere, Category="Launch")
	float HorizontalLaunchStrength = 1000.f;

	UPROPERTY(EditAnywhere, Category="Launch")
	float VerticalLaunchStrength = 500.f;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;
	
	UPROPERTY(EditAnywhere, Category="Swing")
	float PhaseOffset = 0.f;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* Pivot;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* HammerMesh;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* DamageCollision;

	UPROPERTY(EditAnywhere, Category="Swing")
	float SwingAngle = 60.f;

	UPROPERTY(EditAnywhere, Category="Swing")
	float SwingSpeed = 1.5f;

	UPROPERTY(EditAnywhere, Category="Swing")
	float StartOffset = 0.f;

private:

	float RunningTime = 0.f;
};