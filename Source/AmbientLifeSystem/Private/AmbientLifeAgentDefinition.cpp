// Fill out your copyright notice in the Description page of Project Settings.

#include "AmbientLifeAgentDefinition.h"

#include "AmbientLifeAgentComponent.h"
#include "AmbientLifeSystem.h"
#include "GameFramework/Actor.h"

TSubclassOf<AActor> UAmbientLifeAgentDefinition::GetAgentActorClass() const
{
	// Load and check actor has component.
	if (const auto Loaded = AgentActorClass.LoadSynchronous())
	{
		if (AActor::GetActorClassDefaultComponent(Loaded, UAmbientLifeAgentComponent::StaticClass()))
		{
			return Loaded;
		}

		UE_LOG(LogAmbientLife, Warning, TEXT("Unable to get agent component for actor class : %s"), *Loaded->GetName())
	}

	return nullptr;
}

UStaticMesh* UAmbientLifeAgentDefinition::GetAgentStaticMesh() const
{
	if (const auto SM = StaticMeshAsset.LoadSynchronous())
	{
		return SM;
	}

	return nullptr;
}

USkeletalMesh* UAmbientLifeAgentDefinition::GetAgentSkeletalMesh() const
{
	if (const auto SkelM = SkeletalMeshAsset.LoadSynchronous())
	{
		return SkelM;
	}

	return nullptr;
}

TSoftClassPtr<UAnimInstance> UAmbientLifeAgentDefinition::GetAnimInstanceClass() const
{
	if (const auto Loaded = SkeletalMeshAnimInstanceClass.LoadSynchronous())
	{
		return Loaded;
	}

	return nullptr;
}
