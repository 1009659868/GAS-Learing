// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Interaction/TargetInterface.h"
#include "AuraEnemy.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;

/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public ITargetInterface
{
	GENERATED_BODY()

public:
	AAuraEnemy();

	/** Enemy Interface*/
	virtual void HighlightActor() override;
	virtual void UnHighlightActor() override;
	/** end Enemy Interface*/
	UPROPERTY(BlueprintReadOnly)
	bool bHighlighted = false;

	/** Combat Interface*/
	virtual int32 GetPlayerLevel() override;
	/** end Interface*/
protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Character Class Defaults")
	int32 Level=1;
	
};
