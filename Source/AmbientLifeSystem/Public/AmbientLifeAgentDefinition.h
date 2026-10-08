// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AmbientLifeAgentDefinition.generated.h"

class UTexture2D;
class UAnimInstance;
class USkeletalMesh;
class UStaticMesh;

UCLASS(BlueprintType)
class AMBIENTLIFESYSTEM_API UAmbientLifeAgentDefinition : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ambient Life")
	FString NameId = "";

#if WITH_EDITORONLY_DATA
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ambient Life")
	TObjectPtr<UTexture2D> DisplayTexture = nullptr;
#endif
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ambient Life")
	TSoftClassPtr<AActor> AgentActorClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh")
	TSoftObjectPtr<UStaticMesh> StaticMeshAsset = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh")
	TSoftObjectPtr<USkeletalMesh> SkeletalMeshAsset = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Mesh")
	TSoftClassPtr<UAnimInstance> SkeletalMeshAnimInstanceClass = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Ambient Life")
	TSubclassOf<AActor> GetAgentActorClass() const;
	
	UFUNCTION(BlueprintCallable, Category = "Ambient Life")
	UStaticMesh* GetAgentStaticMesh() const;
	
	UFUNCTION(BlueprintCallable, Category = "Ambient Life")
	USkeletalMesh* GetAgentSkeletalMesh() const;
	
	UFUNCTION(BlueprintCallable, Category = "Ambient Life")
	TSoftClassPtr<UAnimInstance> GetAnimInstanceClass() const;
};
