// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Interaction/TargetInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(AuraContext);

	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (Subsystem)
	{
		Subsystem->AddMappingContext(AuraContext,0);
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

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	EnhancedInputComponent->BindAction(MoveAction,ETriggerEvent::Triggered,this,&AAuraPlayerController::Move);	
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility,false,CursorHit);
	if (!CursorHit.bBlockingHit) return;

	LastTarget = CurrentTarget;
	CurrentTarget = CursorHit.GetActor();

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
	
	
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f,Rotation.Yaw,0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn =GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection,InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection,InputAxisVector.X);
	}
}

