// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AmbientLifeAgentComponent.generated.h"

class UAmbientLifeAgentDefinition;
class AAmbientLifeSpawnerActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLifeAgentComponentReady, UAmbientLifeAgentComponent*, LifeComponent);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class AMBIENTLIFESYSTEM_API UAmbientLifeAgentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAmbientLifeAgentComponent();

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ambient Life", Transient)
	TObjectPtr<AAmbientLifeSpawnerActor> RelatedSpawnerActor = nullptr;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ambient Life", Transient)
	TObjectPtr<UAmbientLifeAgentDefinition> RelatedLifeDefinition = nullptr;

	UPROPERTY(BlueprintAssignable)
	FOnLifeAgentComponentReady OnLifeAgentComponentReady;
	
	/** Destroy actor and reduce valid count in related spawner actor. */
	UFUNCTION(BlueprintCallable, Category = "Ambient Life")
	void RemoveAgentActor();
};
