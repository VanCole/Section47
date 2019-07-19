// Fill out your copyright notice in the Description page of Project Settings.

#include "S47MenuHud.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

AS47MenuHud::AS47MenuHud()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> PlayerHudObj(TEXT("/Game/UI/Menu/MenuWidget"));
	HUDWidgetClass = PlayerHudObj.Class;
}

void AS47MenuHud::BeginPlay()
{
	Super::BeginPlay();

	if (nullptr != HUDWidgetClass)
	{
		currentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);

		if (currentWidget)
		{
			currentWidget->AddToViewport();
			APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
			controller->bShowMouseCursor = true;
			controller->SetInputMode(FInputModeUIOnly());
		}
	}
}