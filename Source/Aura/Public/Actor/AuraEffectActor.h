// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include  "GameplayEffect.h"
#include "GameFramework/Actor.h"
#include "AuraEffectActor.generated.h"

class UGameplayEffect;

UENUM(BlueprintType)
enum class EEffectApplicationPolicy
{
	ApplyOnOverlap,
	ApplyOnEndOverlap,
	DoNotApply,
};

UENUM(BlueprintType)
enum class EEffectRemovePolicy
{
	RemoveOnEndOverlap,
	DoNotRemove,
};

UCLASS()
class AURA_API AAuraEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AAuraEffectActor();
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* TargetActor,TSubclassOf<UGameplayEffect> GameplayEffectClass);

	UFUNCTION(BlueprintCallable)
	void OnOverlap(AActor* TargetActor);
	
	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* TargetActor);
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Applied Effects")
	bool bDestroyOnEffectRemove = false;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Applied Effects")
	TSubclassOf<UGameplayEffect> InstanceGamePlayEffectClass;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Applied Effects")
	EEffectApplicationPolicy InstanceEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Applied Effects")
	TSubclassOf<UGameplayEffect> DurationGameplayEffectClass;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Applied Effects")
	EEffectApplicationPolicy DurationEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Applied Effects")
	TSubclassOf<UGameplayEffect> InfiniteGameplayEffectClass;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Applied Effects")
	EEffectApplicationPolicy InfiniteEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Applied Effects")
	EEffectRemovePolicy InfiniteEffectRemovePolicy = EEffectRemovePolicy::RemoveOnEndOverlap;

	TMap<FActiveGameplayEffectHandle,UAbilitySystemComponent*> ActiveEffectHandles;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Applied Effects")
	float ActorLevel = 1.f;
private:
	
	
};
