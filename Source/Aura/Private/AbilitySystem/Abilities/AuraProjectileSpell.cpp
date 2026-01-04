// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}
