// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/AuraHUD.h"

UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this,OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetContorllerParams(WCParams);
		OverlayWidgetController->BindCallbacksToDependencies();	
	}
	return OverlayWidgetController;
}

void AAuraHUD::InitOverlayWidget(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC,
	UAttributeSet* AS)
{
	checkf(OverlayWidgetClass,TEXT("Overlay Widget uninitiallized, please fill out BP_AuraHUD"));
	checkf(OverlayWidgetControllerClass,TEXT("Overlay Widget Controller Class uninitiallized, please fill out BP_AuraHUD"));
	
	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
	OverlayWidget = Cast<UAuraUserWidget>(Widget);

	const FWidgetControllerParams WidgetControllerParams(PC,PS,ASC,AS);
	UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParams);
	OverlayWidget->SetWidgetController(OverlayWidgetController);

	WidgetController->BroadcastInitialValues();
	
	Widget->AddToViewport();
	
}

