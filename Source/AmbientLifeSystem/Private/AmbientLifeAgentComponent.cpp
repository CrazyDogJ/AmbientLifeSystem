// Fill out your copyright notice in the Description page of Project Settings.

#include "AmbientLifeAgentComponent.h"

#include "AmbientLifeSpawnerActor.h"

UAmbientLifeAgentComponent::UAmbientLifeAgentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAmbientLifeAgentComponent::RemoveAgentActor()
{
	if (RelatedSpawnerActor != nullptr)
	{
		RelatedSpawnerActor->ReduceValidCount(this);
	}

	RelatedLifeDefinition = nullptr;
	RelatedSpawnerActor = nullptr;
	
	GetOwner()->Destroy(true);
}
