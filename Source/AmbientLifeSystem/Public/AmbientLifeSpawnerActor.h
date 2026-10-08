// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AmbientLifeSpawnerActor.generated.h"

class UAmbientLifeAgentDefinition;
class UAmbientLifeAgentComponent;

UCLASS(BlueprintType, Blueprintable)
class AMBIENTLIFESYSTEM_API AAmbientLifeSpawnerActor : public AActor
{
	GENERATED_BODY()

public:
	AAmbientLifeSpawnerActor();

	/** Life count in this spawner area. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ambient Life")
	int SpawnCount = 0;

	/** Runtime life actors in this spawner area. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ambient Life", Transient)
	TArray<TObjectPtr<UAmbientLifeAgentComponent>> SpawnedAgentActorComponents;

	/** Agent life definition data asset. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ambient Life")
	TArray<TObjectPtr<UAmbientLifeAgentDefinition>> LifeAgentDefinitions = {};
	
	/** If spawned life actor call remove, we reduce this value. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Ambient Life", SaveGame)
	int ValidCount = 0;
	
	UFUNCTION(BlueprintNativeEvent, Category = "Ambient Life")
	FTransform GetSpawnTransform();

	virtual FTransform GetSpawnTransform_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Ambient Life")
	UAmbientLifeAgentDefinition* GetLifeDefinition() const;

	virtual UAmbientLifeAgentDefinition* GetLifeDefinition_Implementation() const;
	
	UFUNCTION(BlueprintCallable, Category = "Ambient Life", BlueprintAuthorityOnly)
	void SpawnAgentActors();
	
	TObjectPtr<UAmbientLifeAgentComponent> SpawnAgentActor();
	
	void ReduceValidCount(const TObjectPtr<UAmbientLifeAgentComponent>& AgentComponent);
	void ClearAgents();
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
