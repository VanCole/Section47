// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Blueprint/WidgetTree.h"
#include "Engine/Classes/GameFramework/GameUserSettings.h"
#include "S47HUD.generated.h"

UCLASS()
class AS47HUD : public AHUD
{
	GENERATED_BODY()

public:
	AS47HUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	// Widget inGame
	UFUNCTION()
	void ShowFuelInformation(float DeltaTime);

	UFUNCTION()
	void UpdateAmmo();

	UFUNCTION()
	void ChangeCrosshairTexture();

	UFUNCTION()
	void WeaponSelector();

	// Widget pause menu
	UFUNCTION()
	void ShowHidePauseMenu();

	UFUNCTION()
	void ResumeGame();

	UFUNCTION()
	void EnableEndGame();

	// Widget endGame
	UFUNCTION()
	void EnableGameOver();

	UFUNCTION()
	void PostGeneration();

	UFUNCTION()
	void PreGeneration();

public:

	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<class UUserWidget> HUDWidgetClass;

	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<class UUserWidget> PauseMenuWidgetClass;

	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<class UUserWidget> EndLevelWidgetClass;

	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<class UUserWidget> GameOverWidgetClass;

	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<class UUserWidget> LoadingWidgetClass;

	class UUserWidget* currentHUDWidget;
	class UUserWidget* currentPauseMenuWidget;
	class UUserWidget* currentEndGameWidget;
	class UUserWidget* currentGameOverWidget;
	class UUserWidget* currentLoadingWidget;

	UWidgetTree* widgetHudTree;
	UWidgetTree* inGameWidgetTree;

	/** Fuel **/
	bool showFuel;
	float fuelWidgetOpacity;
	float timerFuelOpacity;

	UPROPERTY()
	class AS47PlayerCharacter* characterOwner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isInGameMenu;

	UPROPERTY(BlueprintReadWrite)
	class AS47PlayerState* playerStateSpectate;
};

