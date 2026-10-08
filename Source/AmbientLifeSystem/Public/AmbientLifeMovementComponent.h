// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WaterBodyComponent.h"
#include "GameFramework/MovementComponent.h"
#include "AmbientLifeMovementComponent.generated.h"

class UWaterBodyComponent;

UENUM(BlueprintType)
enum EAmbientLifeMovementMode
{
	Falling		UMETA(DisplayName = "Falling"),
	Walking		UMETA(DisplayName = "Walking"),
	Flying		UMETA(DisplayName = "Flying"),
	Swimming	UMETA(DisplayName = "Swimming")
};

UCLASS(ClassGroup=Movement, meta=(BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class AMBIENTLIFESYSTEM_API UAmbientLifeMovementComponent : public UMovementComponent
{
	GENERATED_BODY()

public:
	UAmbientLifeMovementComponent();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Movement")
	TArray<UWaterBodyComponent*> CurrentWaterBodyComponents;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement")
	TEnumAsByte<EAmbientLifeMovementMode> MovementMode = EAmbientLifeMovementMode::Walking;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement|Falling")
	float TerminalVelocity = 4000.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement|Walking")
	float GroundCheckDistance = 20.f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement|Falling")
	float WalkableFloorZ = 0.7f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement")
	float MaxWalkSpeed = 300.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement")
	float MaxFlySpeed = 400.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement")
	float MaxSwimSpeed = 300.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement")
	float WalkAcceleration = 6.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement")
	float FlyAcceleration = 3.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement")
	float SwimAcceleration = 3.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement")
	float WalkTurnSpeed = 5.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement")
	float FlyTurnSpeed = 4.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement")
	float SwimTurnSpeed = 2.0f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement")
	FVector PendingInput = FVector();
	
	UFUNCTION(BlueprintNativeEvent, Category = "Movement")
	FVector GetPendingInputVector() const;

	virtual FVector GetPendingInputVector_Implementation() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Movement")
	void OnMovementModeChanged(const EAmbientLifeMovementMode PrevMode, const EAmbientLifeMovementMode NewMode);

	virtual void OnMovementModeChanged_Implementation(const EAmbientLifeMovementMode PrevMode, const EAmbientLifeMovementMode NewMode) {};
	
	virtual bool CanChangeMovementMode(EAmbientLifeMovementMode CurrentMode, EAmbientLifeMovementMode NewMode) const { return true; }
	void ChangeMovementMode(EAmbientLifeMovementMode NewMode);

	virtual float GetWaterImmersion(bool& bSuccess) const;
	virtual bool IsInWater() const override;

	static ECollisionChannel GetWaterCollisionChannel();
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	FVector GetClosestWaterBoundLocation(const FVector InWorldLocation) const;
	
	virtual bool FindGround(FHitResult& OutHit) const;
	
private:
	TArray<UWaterBodyComponent*> GetWaterBodyComponents() const;
	
	void CollideSlide(FHitResult& Hit, const FVector& Delta);
	void InterpRotation(const float DeltaTime, const float TurnSpeed) const;

	void PhysFalling(float DeltaTime);
	void PhysWalking(float DeltaTime);
	void PhysFlying(float DeltaTime);
	void PhysSwimming(float DeltaTime);
};
