// Fill out your copyright notice in the Description page of Project Settings.

#include "AmbientLifeSpawnerActor.h"

#include "AmbientLifeAgentComponent.h"
#include "AmbientLifeAgentDefinition.h"
#include "Engine/World.h"

AAmbientLifeSpawnerActor::AAmbientLifeSpawnerActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

FTransform AAmbientLifeSpawnerActor::GetSpawnTransform_Implementation()
{
	return GetActorTransform();
}

UAmbientLifeAgentDefinition* AAmbientLifeSpawnerActor::GetLifeDefinition_Implementation() const
{
	if (LifeAgentDefinitions.Num() > 0)
	{
		const auto LastIndex = LifeAgentDefinitions.Num() - 1;
		const auto RandomIndex = FMath::RandRange(0, LastIndex);
		return LifeAgentDefinitions[RandomIndex];
	}

	return nullptr;
}

void AAmbientLifeSpawnerActor::SpawnAgentActors()
{
	if (HasAuthority())
	{
		for (int i = 0; i < SpawnCount; ++i)
		{
			SpawnAgentActor();
		}
	}
}

TObjectPtr<UAmbientLifeAgentComponent> AAmbientLifeSpawnerActor::SpawnAgentActor()
{
	if (const auto LifeAgentDefinition = GetLifeDefinition())
	{
		if (const auto ActorClass = LifeAgentDefinition->GetAgentActorClass())
		{
			const auto Transform = GetSpawnTransform();
			const FVector Location = Transform.GetLocation();
			const FRotator Rotation = Transform.GetRotation().Rotator();
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			if (const auto SpawnedActor = GetWorld()->SpawnActor(ActorClass, &Location, &Rotation, SpawnParameters))
			{
				if (const auto Comp = SpawnedActor->GetComponentByClass<UAmbientLifeAgentComponent>())
				{
					SpawnedAgentActorComponents.Add(Comp);
					ValidCount++;

					Comp->RelatedSpawnerActor = this;
					Comp->RelatedLifeDefinition = LifeAgentDefinition;
					Comp->OnLifeAgentComponentReady.Broadcast(Comp);
					
					return Comp;
				}
			}
		}
	}

	return nullptr;
}

void AAmbientLifeSpawnerActor::ReduceValidCount(const TObjectPtr<UAmbientLifeAgentComponent>& AgentComponent)
{
	SpawnedAgentActorComponents.Remove(AgentComponent);
	if (ValidCount > 0)
	{
		ValidCount--;
	}
}

void AAmbientLifeSpawnerActor::ClearAgents()
{
	for (const auto ActorComp : SpawnedAgentActorComponents)
	{
		if (ActorComp)
		{
			if (const auto Actor = ActorComp->GetOwner())
			{
				Actor->Destroy(true);
			}
		}
	}
	
	SpawnedAgentActorComponents.Empty();
}

void AAmbientLifeSpawnerActor::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnAgentActors();
}

void AAmbientLifeSpawnerActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	ClearAgents();
}
