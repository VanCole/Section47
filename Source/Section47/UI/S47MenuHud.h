// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "S47MenuHud.generated.h"

/**
 * 
 */

UENUM()
enum Menu
{
	Play,
	Options,
	Credits,
	Exit
};

UCLASS()
class SECTION47_API AS47MenuHud : public AHUD
{
	GENERATED_BODY()

	AS47MenuHud();

public:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<class UUserWidget> HUDWidgetClass;

	UPROPERTY(EditAnywhere, Category = "HUD")
	class UUserWidget* currentWidget;
};
