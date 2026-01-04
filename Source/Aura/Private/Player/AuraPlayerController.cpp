// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/TargetInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(AuraContext);


	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		GetLocalPlayer());

	if (Subsystem)
	{
		Subsystem->AddMappingContext(AuraContext, 0);
	}


	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();


	// UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	//
	// EnhancedInputComponent->BindAction(MoveAction,ETriggerEvent::Triggered,this,&AAuraPlayerController::Move);
	//
	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);

	AuraInputComponent->BindAbilityActions(InputConfig, this, &AAuraPlayerController::AbilityInputTagPressed,
	                                       &AAuraPlayerController::AbilityInputTagReleased,
	                                       &AAuraPlayerController::AbilityInputTagHeld);
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
	AutoRun();
}

void AAuraPlayerController::AutoRun()
{
	if (!bAutoRunning) return;
	if (APawn* ControlledPawn = GetPawn())
	{
		//找到线上最接近受控Pawn的位置
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(
			ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		//找到最接近世界位置的方向
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(
			LocationOnSpline, ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(Direction);
		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
		if (DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}
	}
}

void AAuraPlayerController::CursorTrace()
{
	GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;

	LastTarget = CurrentTarget;
	CurrentTarget = CursorHit.GetActor();

	if (LastTarget != CurrentTarget)
	{
		if (LastTarget) LastTarget->UnHighlightActor();
		if (CurrentTarget) CurrentTarget->HighlightActor();
	}
	/**
	 *Line trace from cursor. There are serval scenaries:
	 *A. LastTarget is null && CurrentTarget is null
	 *	--Do nothing
	 *B. LastTarget is null && CurrentTarget is valid
	 *	--Highlight CurrentTarget
	 *C. LastTarget is valid && CurrentTarget is null
	 *	--UnHighlight LastTarget
	 *D. Both Target are valid, but LastTarget != CurrentTarget
	 *	--UnHighlight LastTarget,and Highlight CurrentTarget
	 *E. Both Target are valid, and are the same Target
	 *	--Do nothing
	 */
	/*
	if (LastTarget == nullptr)
	{
		if (CurrentTarget != nullptr)
		{
			//Case B.
			CurrentTarget->HighlightActor();
		}
		else
		{
			//Case A.	--Both are null, do nothing
		}
	}
	else	//LastTarget is valid
	{
		if (CurrentTarget==nullptr)
		{
			//Case C.
			LastTarget->UnHighlightActor();
		}
		else//both Target are valid
		{
			if (LastTarget!=CurrentTarget)
			{
				//Case D.
				LastTarget->UnHighlightActor();
				CurrentTarget->HighlightActor();
			}
			else
			{
				//Case E.	--Do nothing
			}
		}
	}
	*/
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		//判断是否有瞄准对象
		bTargeting = CurrentTarget ? true : false;
		//在鼠标放开之前不确定是否需要自动移动,所以设置值为false
		bAutoRunning = false;
	}
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag);
		return;
	}
	if (bTargeting)
	{
		if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag);
	}
	else
	{
		const APawn* ControllerPawn = GetPawn();
		if (FollowTime <= ShortPressThreshold && ControllerPawn)
		{
			if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(
				this, ControllerPawn->GetActorLocation(), CachedDestination))
			{
				Spline->ClearSplinePoints();
				for (const FVector& PointLoc : NavPath->PathPoints)
				{
					Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
					DrawDebugSphere(GetWorld(), PointLoc, 8.f, 8, FColor::Green, false, 5.f);
				}
				CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num() - 1];
				bAutoRunning = true;
			}
		}
		FollowTime = 0.f;
		bTargeting = false;
	}
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);

		return;
	}

	if (bTargeting)
	{
		if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
	}
	else
	{
		FollowTime += GetWorld()->GetDeltaSeconds();

		if (CursorHit.bBlockingHit) CachedDestination = CursorHit.ImpactPoint;
	
		if (APawn* ControllerPawn = GetPawn())
		{
			const FVector WorldLocation = (CachedDestination - ControllerPawn->GetActorLocation()).GetSafeNormal();
			ControllerPawn->AddMovementInput(WorldLocation);
		}
	}
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return AuraAbilitySystemComponent;
}


void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}
