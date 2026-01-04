// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class ITargetInterface;
class UAuraInputConfig;
class UAuraAbilitySystemComponent;
class USplineComponent;

struct FInputActionValue;

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AAuraPlayerController();
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
	virtual void PlayerTick(float DeltaTime) override;
private:
	UPROPERTY(EditAnywhere,Category = "Input")
	TObjectPtr<UInputMappingContext> AuraContext;
	UPROPERTY(EditAnywhere,Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	void Move(const FInputActionValue& InputActionValue);
	void CursorTrace();
	FHitResult CursorHit;
	TScriptInterface<ITargetInterface> LastTarget;
	TScriptInterface<ITargetInterface> CurrentTarget;

	//按键回调函数
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);
	
	UPROPERTY(EditDefaultsOnly,Category = "Input")
	TObjectPtr<UAuraInputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

	UAuraAbilitySystemComponent* GetASC();
	void AutoRun();
	//缓存目的地
	FVector CachedDestination= FVector::Zero();
	//鼠标长按时间计数
	float FollowTime = 0.f;
	//短按时间阈值 s
	float ShortPressThreshold = 0.5f;
	//是否正在自动移动
	bool bAutoRunning = false;
	//是否正在瞄准
	bool bTargeting = false;
	//目的地可接受范围
	UPROPERTY(EditDefaultsOnly,Category = "AutoMove")
	float AutoRunAcceptanceRadius = 50.f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;
};
