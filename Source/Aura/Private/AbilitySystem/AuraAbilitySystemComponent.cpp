// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass:StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass,1);

		//获取能力系统并转换类型
		if (const UAuraGameplayAbility* AuraGameplayAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			//添加动态能力标签
			AbilitySpec.DynamicAbilityTags.AddTag(AuraGameplayAbility->StartupInputTag);
			//赋予能力
			GiveAbility(AbilitySpec);
		}
	}
	
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	//验证输入标签的有效性
	if (!InputTag.IsValid()) return;
	//检查已拥有的、可激活的、带有此标签的能力
	//获取可激活能力
	for (auto& AbilitySpec:GetActivatableAbilities())
	{
		//标签匹配
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	//不能因为输入被释放而立刻终止某项能力
	//只需要通知该输入已经停止就行
	//验证输入标签的有效性
	if (!InputTag.IsValid()) return;
	//检查已拥有的、可激活的、带有此标签的能力
	//获取可激活能力
	for (auto& AbilitySpec:GetActivatableAbilities())
	{
		//标签匹配
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
		}
	}
}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                                                const FGameplayEffectSpec& EffectSpec,
                                                FActiveGameplayEffectHandle ActiveEffectHandle)
{
	// GEngine->AddOnScreenDebugMessage(-1, 8.f, FColor::Blue, "EffectApplied");

	FGameplayTagContainer TagContainer;

	EffectSpec.GetAllAssetTags(TagContainer);
	EffectAssetTags.Broadcast(TagContainer);
	
	
}
