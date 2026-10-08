// Fill out your copyright notice in the Description page of Project Settings.

#include "AmbientLifeMovementComponent.h"

#include "WaterBodyActor.h"
#include "WaterRuntimeSettings.h"
#include "Components/ShapeComponent.h"
#include "Engine/World.h"

UAmbientLifeMovementComponent::UAmbientLifeMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAmbientLifeMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CurrentWaterBodyComponents = GetWaterBodyComponents();
	
	if (!GetOwner() || !UpdatedComponent || ShouldSkipUpdate(DeltaTime)) return;
 
	if (IsInWater())
	{
		ChangeMovementMode(Swimming);
	}
	
	switch (MovementMode)
	{
		case EAmbientLifeMovementMode::Walking:  	PhysWalking(DeltaTime); 	break;
		case EAmbientLifeMovementMode::Flying:   	PhysFlying(DeltaTime);  	break;
		case EAmbientLifeMovementMode::Swimming: 	PhysSwimming(DeltaTime);	break;
		case EAmbientLifeMovementMode::Falling: 	PhysFalling(DeltaTime);		break;
		default: ;
	}
}

FVector UAmbientLifeMovementComponent::GetPendingInputVector_Implementation() const
{
	return PendingInput;
}

TArray<UWaterBodyComponent*> UAmbientLifeMovementComponent::GetWaterBodyComponents() const
{
	TArray<UWaterBodyComponent*> OverlappedWaterBodyComponents;
	
	if (!UpdatedComponent)
	{
		return OverlappedWaterBodyComponents;
	}
	
	TArray<AActor*> OverlappedActors;
	const auto PrimComp = Cast<UPrimitiveComponent>(UpdatedComponent);
	PrimComp->GetOverlappingActors(OverlappedActors);
	for (const auto Actor : OverlappedActors)
	{
		if (const auto WaterBodyActor = Cast<AWaterBody>(Actor))
		{
			OverlappedWaterBodyComponents.Add(WaterBodyActor->GetWaterBodyComponent());
		}
	}

	return OverlappedWaterBodyComponents;
}

void UAmbientLifeMovementComponent::ChangeMovementMode(EAmbientLifeMovementMode NewMode)
{
	// Dont need to change.
	if (MovementMode == NewMode)
	{
		return;
	}
	
	if (CanChangeMovementMode(MovementMode, NewMode))
	{
		// Change
		const auto CachedPrevMode = MovementMode;
		MovementMode = NewMode;
		OnMovementModeChanged(CachedPrevMode, NewMode);
	}
}

float UAmbientLifeMovementComponent::GetWaterImmersion(bool& bSuccess) const
{
	bool bInit = false;
	FVector TopLoc = FVector();
	
	for (const auto WaterBodyComponent : CurrentWaterBodyComponents)
	{
		FVector WaterLoc, WaterNor, WaterVel;
		float WaterDepth;
		WaterBodyComponent->GetWaterSurfaceInfoAtLocation(UpdatedComponent->GetComponentLocation(),
			WaterLoc, WaterNor, WaterVel, WaterDepth);

		if (!bInit)
		{
			TopLoc = WaterLoc;
			bInit = true;
		}
		else if (WaterLoc.Z >= TopLoc.Z)
		{
			TopLoc = WaterLoc;
		}
	}
	
	if (bInit)
	{
		bSuccess = true;
		return TopLoc.Z - UpdatedComponent->GetComponentLocation().Z;
	}
	
	bSuccess = false;
	return 0.0f;
}

bool UAmbientLifeMovementComponent::IsInWater() const
{
	bool bSuccess = false;
	const auto Immersion = GetWaterImmersion(bSuccess);
	if (bSuccess && Immersion >= 0.0f)
	{
		return true;
	}

	return false;
}

ECollisionChannel UAmbientLifeMovementComponent::GetWaterCollisionChannel()
{
	return GetDefault<UWaterRuntimeSettings>()->CollisionChannelForWaterTraces;
}

FVector UAmbientLifeMovementComponent::GetClosestWaterBoundLocation(const FVector InWorldLocation) const
{
	if (!UpdatedComponent)
	{
		return FVector();
	}
	
	const auto CollisionChannel = GetWaterCollisionChannel();
	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());
	GetWorld()->LineTraceMultiByChannel(HitResults, InWorldLocation, UpdatedComponent->GetComponentLocation(), CollisionChannel, Params);

	for (const auto HitResult : HitResults)
	{
		if (Cast<AWaterBody>(HitResult.GetActor()))
		{
			return HitResult.ImpactPoint;
		}
	}
	
	return FVector();
}

bool UAmbientLifeMovementComponent::FindGround(FHitResult& OutHit) const
{
	const auto ObjectType = UpdatedComponent->GetCollisionObjectType();
	const auto CurrentLoc = UpdatedComponent->GetComponentLocation();
	const auto CurrentRot = UpdatedComponent->GetComponentQuat();
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());
	if (const auto ShapeCollision = Cast<UShapeComponent>(UpdatedComponent))
	{
		return GetWorld()->SweepSingleByChannel(OutHit, CurrentLoc, CurrentLoc - FVector(0,0,-GroundCheckDistance), 
			CurrentRot, ObjectType, ShapeCollision->GetCollisionShape(), Params);
	}
	
	return false;
}

void UAmbientLifeMovementComponent::CollideSlide(FHitResult& Hit, const FVector& Delta)
{
	if (Hit.IsValidBlockingHit())
	{
		// 撞击后滑动，防止卡死
		SlideAlongSurface(Delta, 1.f - Hit.Time, Hit.Normal, Hit);
	}
}

void UAmbientLifeMovementComponent::InterpRotation(const float DeltaTime, const float TurnSpeed) const
{
	if(Velocity.SizeSquared() > 1.0f)
	{
		FRotator TargetRotation = Velocity.Rotation();
		const auto FinalRotation = FMath::RInterpTo(UpdatedComponent->GetComponentRotation(), TargetRotation, DeltaTime, TurnSpeed);
		UpdatedComponent->SetWorldRotation(FinalRotation);
	}
}

void UAmbientLifeMovementComponent::PhysFalling(float DeltaTime)
{
	// Accelerate velocity by gravity acceleration.
	Velocity += FVector(0.f, 0.f, GetGravityZ()) * DeltaTime;

	// Clamp fall velocity.
	Velocity.Z = FMath::Clamp(Velocity.Z, -TerminalVelocity, TerminalVelocity);

	FVector Delta = Velocity * DeltaTime;
	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentQuat(), true, Hit);

	if (Hit.IsValidBlockingHit())
	{
		if (Hit.Normal.Z >= WalkableFloorZ)
		{
			Velocity.Z = 0.f;

			ChangeMovementMode(Walking);
			
			return;
		}
		
		SlideAlongSurface(Delta, 1.f - Hit.Time, Hit.Normal, Hit, true);
	}
}

void UAmbientLifeMovementComponent::PhysWalking(float DeltaTime)
{
	FVector Input = GetPendingInputVector();
	if (Input.IsNearlyZero())
	{
		// Don't update when input is nearly zero.
		return;
	}
	
	FHitResult FloorHit;
	if (!FindGround(FloorHit))
	{
		ChangeMovementMode(Falling);
		return;
	}
	
	Velocity = FMath::VInterpTo(Velocity, Input * MaxWalkSpeed, DeltaTime, WalkAcceleration);
	Velocity.Z = 0.f;
	
	FVector Delta = Velocity * DeltaTime;
	// Project velocity to ground plane.
	Delta = FVector::VectorPlaneProject(
		Delta,
		FloorHit.Normal);
	if (!Delta.IsNearlyZero())
	{
		FHitResult Hit;
		SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentRotation(), true, Hit);
		CollideSlide(Hit, Delta);
		InterpRotation(DeltaTime, WalkTurnSpeed);
		
		if (!FindGround(FloorHit))
		{
			ChangeMovementMode(Falling);
		}
	}
}

void UAmbientLifeMovementComponent::PhysFlying(float DeltaTime)
{
	FVector Input = GetPendingInputVector();
	Velocity = FMath::VInterpConstantTo(Velocity, Input * MaxFlySpeed, DeltaTime, FlyAcceleration);

	FVector Delta = Velocity * DeltaTime;
	if (!Delta.IsNearlyZero())
	{
		FHitResult Hit;
		SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentRotation(), true, Hit);
		CollideSlide(Hit, Delta);
		InterpRotation(DeltaTime, FlyTurnSpeed);
	}
}

void UAmbientLifeMovementComponent::PhysSwimming(float DeltaTime)
{
	FVector Input = GetPendingInputVector();
	Velocity = FMath::VInterpTo(Velocity, Input * MaxSwimSpeed, DeltaTime, SwimAcceleration);
	
	FVector Delta = Velocity * DeltaTime;
	if (!Delta.IsNearlyZero())
	{
		bool bSuccess = false;
		const auto WaterImmersion = FMath::Max(0, GetWaterImmersion(bSuccess));
		Velocity.Z = FMath::Min(Velocity.Z, WaterImmersion / DeltaTime);
		
		Delta = Velocity * DeltaTime;
		FHitResult Hit;
		SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentRotation(), true, Hit);
		CollideSlide(Hit, Delta);
		InterpRotation(DeltaTime, SwimTurnSpeed);
	}

	if (!IsInWater())
	{
		ChangeMovementMode(Falling);
	}
}
