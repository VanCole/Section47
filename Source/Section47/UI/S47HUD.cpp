// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "S47HUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include "S47GameState.h"
#include "Map/S47MapGenerator.h"
#include "Components/S47AttackComponent.h"
#include "Weapon/S47Weapon.h"
#include "Weapon/WeaponRaycast/S47Rifle.h"
#include "Section47.h"
#include "Player/S47PlayerCharacter.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/VerticalBox.h"

AS47HUD::AS47HUD()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> PlayerHudObj(TEXT("/Game/UI/HUD/PlayerHUD"));
	HUDWidgetClass = PlayerHudObj.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> PauseMenuObj(TEXT("/Game/UI/Menu/InGameMenuWidget"));
	PauseMenuWidgetClass = PauseMenuObj.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> EndLevelObj(TEXT("/Game/UI/EndGame/EndLevelWidget"));
	EndLevelWidgetClass = EndLevelObj.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> GameOverObj(TEXT("/Game/UI/GameOver/GameOverWidget"));
	GameOverWidgetClass = GameOverObj.Class;

	static ConstructorHelpers::FClassFinder<UUserWidget> LoadingObj(TEXT("/Game/UI/Menu/LoadingScreen"));
	LoadingWidgetClass = LoadingObj.Class;
}


void AS47HUD::DrawHUD()
{
	Super::DrawHUD();

	//Don't delete

	/*
	// Draw very simple crosshair
	// find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);
	// --------CROSSHAIR---------
	//offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition( (Center.X), (Center.Y + 20.0f));
	// draw the crosshair
	FCanvasTileItem TileItem( CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem( TileItem );
	*/
}

void AS47HUD::BeginPlay()
{
	Super::BeginPlay();

	if (nullptr != GEngine)
	{
		UGameUserSettings* settings = GEngine->GameUserSettings;
	}
	
	characterOwner = Cast<AS47PlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	fuelWidgetOpacity = 0.0f;
	showFuel = false;
	timerFuelOpacity = 0.0f;

	if (nullptr != HUDWidgetClass)
	{
		currentHUDWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);

		if (nullptr != currentHUDWidget)
		{
			currentHUDWidget->AddToViewport();
			widgetHudTree = currentHUDWidget->WidgetTree;
		}
	}
	else
	{
		UE_LOG(LogS47HUD, Error, TEXT("currentHUDWidget is nullptr"));
	}

	AS47GameState* gameState = Cast<AS47GameState>(GetWorld()->GetGameState());
	if (nullptr != gameState)
	{
		gameState->OnEndLevel.AddUniqueDynamic(this, &AS47HUD::EnableEndGame);
		gameState->OnGameOver.AddUniqueDynamic(this, &AS47HUD::EnableGameOver);
		gameState->OnPostGeneration.AddUniqueDynamic(this, &AS47HUD::PostGeneration);
		gameState->OnPreGeneration.AddUniqueDynamic(this, &AS47HUD::PreGeneration);
	}
	else
	{
		UE_LOG(LogS47HUD, Error, TEXT("gameState is nullptr"));
	}

	currentPauseMenuWidget = CreateWidget<UUserWidget>(GetWorld(), PauseMenuWidgetClass);
	if (nullptr != currentPauseMenuWidget)
	{
		currentPauseMenuWidget->AddToViewport();
		inGameWidgetTree = currentPauseMenuWidget->WidgetTree;
		currentPauseMenuWidget->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		UE_LOG(LogS47HUD, Error, TEXT("currentPauseMenuWidget is nullptr"));
	}

	if (nullptr != inGameWidgetTree)
	{
		UButton* resumeButton = Cast<UButton>(inGameWidgetTree->FindWidget(TEXT("ResumeButton")));

		if (nullptr != resumeButton)
		{
			resumeButton->OnClicked.AddUniqueDynamic(this, &AS47HUD::ResumeGame);
		}
		else
		{
			UE_LOG(LogS47HUD, Error, TEXT("resumeButton is nullptr"));
		}
	}
	else
	{
		UE_LOG(LogS47HUD, Error, TEXT("inGameWidgetTree is nullptr"));
	}

	if (nullptr != characterOwner)
	{
		characterOwner->onShowInGameMenu.AddUniqueDynamic(this, &AS47HUD::ShowHidePauseMenu);
	}
	else
	{
		UE_LOG(LogS47HUD, Error, TEXT("characterOwner is nullptr"));
	}
}

void AS47HUD::Tick(float DeltaTime)
{
	characterOwner = Cast<AS47PlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	if (nullptr != characterOwner)
	{
		AS47PlayerState* playerState = Cast<AS47PlayerState>(characterOwner->GetPlayerState());

		if (nullptr != playerState)
		{
			if (!playerState->isDead)
			{
				ShowFuelInformation(DeltaTime);

				UpdateAmmo();

				ChangeCrosshairTexture();

				WeaponSelector();
			}
		}
	}
}

// When click on resume button
void AS47HUD::ResumeGame()
{
	GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Orange, *FString::Printf(TEXT("ResumeGame")));
	if (nullptr != characterOwner)
	{
		if (nullptr != currentPauseMenuWidget)
		{
			GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Orange, *FString::Printf(TEXT("cpmw")));

			currentPauseMenuWidget->SetVisibility(ESlateVisibility::Hidden);

			characterOwner->isInGameMenu = false;

			if (nullptr != characterOwner->playerController)
			{
				characterOwner->playerController->bShowMouseCursor = false;
				characterOwner->playerController->bEnableClickEvents = false;
				characterOwner->playerController->bEnableMouseOverEvents = false;

				characterOwner->playerController->SetInputMode(FInputModeGameOnly());
				GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Orange, *FString::Printf(TEXT("cp")));
			}
			else
			{
				UE_LOG(LogS47HUD, Error, TEXT("characterOwner->playerController is nullptr"));
			}
		}
		else
		{
			UE_LOG(LogS47HUD, Error, TEXT("currentPauseMenuWidget is nullptr"));
		}
	}
	else
	{
		UE_LOG(LogS47HUD, Error, TEXT("characterOwner is nullptr"));
	}
}

void AS47HUD::EnableEndGame()
{
	if (nullptr != EndLevelWidgetClass)
	{
		currentEndGameWidget = CreateWidget<UUserWidget>(GetWorld(), EndLevelWidgetClass);

		if (nullptr != currentEndGameWidget)
		{
			currentEndGameWidget->AddToViewport();

			if (nullptr != characterOwner)
			{
				if (nullptr != characterOwner->playerController)
				{
					GetOwningPlayerController()->bShowMouseCursor = true;
					GetOwningPlayerController()->bEnableClickEvents = true;
					GetOwningPlayerController()->bEnableMouseOverEvents = true;
					GetOwningPlayerController()->SetInputMode(FInputModeUIOnly());

					characterOwner->GetCharacterMovement()->MaxWalkSpeed = 0.0f;
					characterOwner->canBeDamaged = false;
				}
				else
				{
					UE_LOG(LogS47HUD, Error, TEXT("characterOwner->playerController is nullptr"));
				}
			}
			else
			{
				UE_LOG(LogS47HUD, Error, TEXT("characterOwner is nullptr"));
			}
		}
		else
		{
			UE_LOG(LogS47HUD, Error, TEXT("currentEndGameWidget is nullptr"));
		}
	}
	else
	{
		UE_LOG(LogS47HUD, Error, TEXT("EndLevelWidgetClass is nullptr"));
	}
}

void AS47HUD::UpdateAmmo()
{
	if (nullptr != characterOwner)
	{
		if (nullptr != characterOwner->GetAttackComponent())
		{
			if (nullptr != characterOwner->GetAttackComponent()->GetCurrentWeapon())
			{
				int totalAmmo = characterOwner->GetAttackComponent()->GetCurrentWeapon()->TotalAmmo;
				int currentAmmo = characterOwner->GetAttackComponent()->GetCurrentWeapon()->CurrentAmmo;

				if (nullptr != currentHUDWidget)
				{
					UTextBlock* text = Cast<UTextBlock>(currentHUDWidget->GetWidgetFromName(FName(TEXT("Ammo_Text"))));

					if (nullptr != text)
					{
						text->SetText(FText::FromString(FString::FromInt(currentAmmo)));

						UImage* imageInfinite = Cast<UImage>(currentHUDWidget->GetWidgetFromName(FName(TEXT("InfiniteLogo"))));

						if (nullptr != imageInfinite)
						{
							if (characterOwner->GetAttackComponent()->GetCurrentWeapon()->weaponName.EqualTo(FText::FromString("Pistol")))
							{
								imageInfinite->SetVisibility(ESlateVisibility::Visible);
								text->SetVisibility(ESlateVisibility::Hidden);
							}
							else if (characterOwner->GetAttackComponent()->GetCurrentWeapon()->weaponName.EqualTo(FText::FromString("Rifle")))
							{
								AS47Rifle* rifle = Cast<AS47Rifle>(characterOwner->GetAttackComponent()->GetCurrentWeapon());

								if (nullptr != rifle)
								{
									text->SetText(FText::FromString(FString::FromInt(currentAmmo) + " - " + FString::FromInt(rifle->currentAmmoBis)));
								}

								imageInfinite->SetVisibility(ESlateVisibility::Hidden);
								text->SetVisibility(ESlateVisibility::Visible);
							}
							else
							{
								imageInfinite->SetVisibility(ESlateVisibility::Hidden);
								text->SetVisibility(ESlateVisibility::Visible);
							}
						}
					}
					else
					{
						UE_LOG(LogS47HUD, Error, TEXT("text is nullptr"));
					}
				}
				else
				{
					UE_LOG(LogS47HUD, Error, TEXT("currentHUDWidget is nullptr"));
				}
			}
			else
			{
				UE_LOG(LogS47HUD, Error, TEXT("characterOwner->GetAttackComponent()->GetCurrentWeapon() is nullptr"));
			}
		}
		else
		{
			UE_LOG(LogS47HUD, Error, TEXT("characterOwner->GetAttackComponent() is nullptr"));
		}
	}
	else
	{
		UE_LOG(LogS47HUD, Error, TEXT("characterOwner is nullptr"));
	}
}

void AS47HUD::ChangeCrosshairTexture()
{
	if (nullptr != widgetHudTree)
	{
		UImage* crosshair = Cast<UImage>(widgetHudTree->FindWidget(TEXT("Crosshair")));

		if (nullptr != crosshair)
		{
			if (nullptr != characterOwner)
			{
				if (nullptr != characterOwner->AttackComponent)
				{
					if (nullptr != characterOwner->AttackComponent->GetCurrentWeapon())
					{
						crosshair->SetBrushFromTexture(characterOwner->AttackComponent->GetCurrentWeapon()->crosshair, false);
					}
					else
					{
						UE_LOG(LogS47HUD, Error, TEXT("characterOwner->AttackComponent->GetCurrentWeapon() is nullptr"));
					}
				}
				else
				{
					UE_LOG(LogS47HUD, Error, TEXT("characterOwner->AttackComponent is nullptr"));
				}
			}
			else
			{
				UE_LOG(LogS47HUD, Error, TEXT("characterOwner is nullptr"));
			}
		}
		else
		{
			UE_LOG(LogS47HUD, Error, TEXT("crosshair is nullptr"));
		}
	}
	else
	{
		UE_LOG(LogS47HUD, Error, TEXT("widgetHudTree is nullptr"));
	}
}

void AS47HUD::WeaponSelector()
{
	if (nullptr != characterOwner)
	{
		if (nullptr != characterOwner->GetAttackComponent())
		{
			UImage* image1 = Cast<UImage>(widgetHudTree->FindWidget(FName(TEXT("Weapon1_Image"))));
			UImage* image2 = Cast<UImage>(widgetHudTree->FindWidget(FName(TEXT("Weapon2_Image"))));
			UImage* image3 = Cast<UImage>(widgetHudTree->FindWidget(FName(TEXT("Weapon3_Image"))));
				
			int nbWeapon = 0;
			for (int i = 0; i < characterOwner->GetAttackComponent()->WeaponOnPlayer.Num(); i++)
			{
				if (nullptr != characterOwner->GetAttackComponent()->WeaponOnPlayer[i])
				{
					nbWeapon++;
				}
			}

			if (nullptr != image1 && nullptr != image2 && nullptr != image3)
			{
				switch (characterOwner->GetAttackComponent()->indexWeapon)
				{
				case 0:
					if (nullptr != image1)
					{
						if (nullptr != characterOwner->GetAttackComponent()->WeaponOnPlayer[0])
						{
							image1->SetOpacity(1.0f);
							image1->SetBrushFromTexture(characterOwner->GetAttackComponent()->WeaponOnPlayer[0]->weaponImage);
						}
						else
						{
							image1->SetOpacity(0.0f);
						}
					}
					if (nullptr != image2)
					{
						if (nullptr != characterOwner->GetAttackComponent()->WeaponOnPlayer[1])
						{
							image2->SetOpacity(1.0f);
							image2->SetBrushFromTexture(characterOwner->GetAttackComponent()->WeaponOnPlayer[1]->weaponImage);
						}
						else
						{
							image2->SetOpacity(0.0f);
						}
					}
					if (nullptr != image3)
					{
						if (nullptr != characterOwner->GetAttackComponent()->WeaponOnPlayer[2])
						{
							image3->SetOpacity(1.0f);
							image3->SetBrushFromTexture(characterOwner->GetAttackComponent()->WeaponOnPlayer[2]->weaponImage);
						}
						else
						{
							image3->SetOpacity(0.0f);
						}
					}
					break;
				case 1:
					if (nullptr != image1)
					{
						if (nullptr != characterOwner->GetAttackComponent()->WeaponOnPlayer[1])
						{
							image1->SetOpacity(1.0f);
							image1->SetBrushFromTexture(characterOwner->GetAttackComponent()->WeaponOnPlayer[1]->weaponImage);
						}
						else
						{
							image1->SetOpacity(0.0f);
						}
					}

					if (nbWeapon == 3)
					{
						if (nullptr != image2)
						{
							if (nullptr != characterOwner->GetAttackComponent()->WeaponOnPlayer[2])
							{
								image2->SetOpacity(1.0f);
								image2->SetBrushFromTexture(characterOwner->GetAttackComponent()->WeaponOnPlayer[2]->weaponImage);
							}
							else
							{
								image2->SetOpacity(0.0f);
							}
						}
						if (nullptr != image3)
						{
							if (nullptr != characterOwner->GetAttackComponent()->WeaponOnPlayer[0])
							{
								image3->SetOpacity(1.0f);
								image3->SetBrushFromTexture(characterOwner->GetAttackComponent()->WeaponOnPlayer[0]->weaponImage);
							}
							else
							{
								image3->SetOpacity(0.0f);
							}
						}
					}
					else
					{
						if (nullptr != image2)
						{
							if (nullptr != characterOwner->GetAttackComponent()->WeaponOnPlayer[0])
							{
								image2->SetOpacity(1.0f);
								image2->SetBrushFromTexture(characterOwner->GetAttackComponent()->WeaponOnPlayer[0]->weaponImage);
							}
							else
							{
								image2->SetOpacity(0.0f);
							}
						}
						if (nullptr != image3)
						{
							image3->SetOpacity(0.0f);
						}
					}
					break;
				case 2:
					if (nullptr != image1)
					{
						if (nullptr != characterOwner->GetAttackComponent()->WeaponOnPlayer[2])
						{
							image1->SetOpacity(1.0f);
							image1->SetBrushFromTexture(characterOwner->GetAttackComponent()->WeaponOnPlayer[2]->weaponImage);
						}
						else
						{
							image1->SetOpacity(0.0f);
						}
					}
					if (nullptr != image2)
					{
						if (nullptr != characterOwner->GetAttackComponent()->WeaponOnPlayer[0])
						{
							image2->SetOpacity(1.0f);
							image2->SetBrushFromTexture(characterOwner->GetAttackComponent()->WeaponOnPlayer[0]->weaponImage);
						}
						else
						{
							image2->SetOpacity(0.0f);
						}
					}
					if (nullptr != image3)
					{
						if (nullptr != characterOwner->GetAttackComponent()->WeaponOnPlayer[1])
						{
							image3->SetOpacity(1.0f);
							image3->SetBrushFromTexture(characterOwner->GetAttackComponent()->WeaponOnPlayer[1]->weaponImage);
						}
						else
						{
							image3->SetOpacity(0.0f);
						}
					}
					break;
				}
			}
			else
			{
				UE_LOG(LogS47HUD, Error, TEXT("image1 or image2 or image3 is nullptr"));
			}
		}
		else
		{
			UE_LOG(LogS47HUD, Error, TEXT("characterOwner->GetAttackComponent() is nullptr"));
		}
	}
	else
	{
		UE_LOG(LogS47HUD, Error, TEXT("characterOwner is nullptr"));
	}
}

void AS47HUD::EnableGameOver()
{
	if (nullptr != EndLevelWidgetClass)
	{
		currentGameOverWidget = CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass);

		if (nullptr != currentGameOverWidget)
		{
			currentGameOverWidget->AddToViewport();

			if (nullptr != characterOwner)
			{
				if (nullptr != characterOwner->playerController)
				{
					GetOwningPlayerController()->bShowMouseCursor = true;
					GetOwningPlayerController()->bEnableClickEvents = true;
					GetOwningPlayerController()->bEnableMouseOverEvents = true;
					GetOwningPlayerController()->SetInputMode(FInputModeUIOnly());
				}
				else
				{
					UE_LOG(LogS47HUD, Error, TEXT("characterOwner->playerController is nullptr"));
				}
			}
			else
			{
				UE_LOG(LogS47HUD, Error, TEXT("characterOwner is nullptr"));
			}
		}
		else
		{
			UE_LOG(LogS47HUD, Error, TEXT("currentGameOverWidget is nullptr"));
		}
	}
	else
	{
		UE_LOG(LogS47HUD, Error, TEXT("EndLevelWidgetClass is nullptr"));
	}
}

// Manage opacity of fuel informations
void AS47HUD::ShowFuelInformation(float DeltaTime)
{
	if (nullptr != widgetHudTree && nullptr != characterOwner)
	{
		UProgressBar* fuelBar = Cast<UProgressBar>(widgetHudTree->FindWidget(TEXT("Fuel_Bar")));

		if (nullptr != fuelBar)
		{
			if (characterOwner->GetPlayerFuelPercentage() >= 1.0f)
			{
				showFuel = false;
			}
			else
			{
				timerFuelOpacity = 0.0f;
				showFuel = true;
			}

			UWidget* canvasPanelFuel = widgetHudTree->FindWidget(TEXT("Canvas_Panel_Fuel"));

			if (nullptr != canvasPanelFuel)
			{
				if (showFuel)
				{
					fuelWidgetOpacity = 255.0f;
					canvasPanelFuel->SetRenderOpacity(fuelWidgetOpacity);
				}
				else
				{
					if (timerFuelOpacity <= 1.0f)
					{
						timerFuelOpacity += DeltaTime / 1.5f;
						fuelWidgetOpacity = FMath::Lerp(1.0f, 0.0f, timerFuelOpacity);
					}

					canvasPanelFuel->SetRenderOpacity(fuelWidgetOpacity);
				}
			}
			else
			{
				UE_LOG(LogS47HUD, Error, TEXT("canvasPanelFuel is nullptr"));
			}
		}
		else
		{
			UE_LOG(LogS47HUD, Error, TEXT("fuelBar or characterOwner is nullptr"));
		}
	}
	else
	{
		UE_LOG(LogS47HUD, Error, TEXT("widgetHudTree is nullptr"));
	}
}

void AS47HUD::ShowHidePauseMenu()
{
	if (nullptr != characterOwner)
	{
		if (characterOwner->isInGameMenu)
		{
			if (nullptr != characterOwner->playerController)
			{
				GetOwningPlayerController()->bShowMouseCursor = true;
				GetOwningPlayerController()->bEnableClickEvents = true;
				GetOwningPlayerController()->bEnableMouseOverEvents = true;

				FVector2D screenSize;
				GEngine->GameViewport->GetViewportSize(screenSize);
				int centreX = screenSize.X / 2;
				int centreY = screenSize.Y / 2;
				//characterOwner->playerController->SetMouseLocation(screenSize.X, screenSize.Y);

				GetOwningPlayerController()->SetMouseLocation(screenSize.X, screenSize.Y);

				if (nullptr != currentPauseMenuWidget)
				{
					currentPauseMenuWidget->SetVisibility(ESlateVisibility::Visible);

					GetOwningPlayerController()->SetInputMode(FInputModeUIOnly());
				}
				else
				{
					UE_LOG(LogS47HUD, Error, TEXT("currentPauseMenuWidget is nullptr"));
				}
			}
			else
			{
				UE_LOG(LogS47HUD, Error, TEXT("characterOwner->playerController is nullptr"));
			}
		}
		else
		{
			if (nullptr != characterOwner->playerController)
			{
				/*characterOwner->playerController->bShowMouseCursor = false;
				characterOwner->playerController->bEnableClickEvents = false;
				characterOwner->playerController->bEnableMouseOverEvents = false;*/

				GetOwningPlayerController()->bShowMouseCursor = false;
				GetOwningPlayerController()->bEnableClickEvents = false;
				GetOwningPlayerController()->bEnableMouseOverEvents = false;

				if (nullptr != currentPauseMenuWidget)
				{
					currentPauseMenuWidget->SetVisibility(ESlateVisibility::Hidden);

					//characterOwner->playerController->SetInputMode(FInputModeGameOnly());
					GetOwningPlayerController()->SetInputMode(FInputModeGameOnly());
				}
				else
				{
					UE_LOG(LogS47HUD, Error, TEXT("currentPauseMenuWidget is nullptr"));
				}
			}
			else
			{
				UE_LOG(LogS47HUD, Error, TEXT("characterOwner->playerController is nullptr"));
			}
		}
	}
	else
	{
		UE_LOG(LogS47HUD, Error, TEXT("characterOwner is nullptr"));
	}
}

void AS47HUD::PostGeneration()
{
	for (TObjectIterator<UUserWidget> Itr; Itr; ++Itr)
	{
		UUserWidget* LiveWidget = *Itr;

		/* If the Widget has no World, Ignore it (It's probably in the Content Browser!) */
		if (!LiveWidget->GetWorld())
		{
			continue;
		}
		else
		{
			LiveWidget->RemoveFromParent();
		}
	}

	characterOwner = Cast<AS47PlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	currentHUDWidget->RemoveFromParent();

	if (nullptr != HUDWidgetClass)
	{
		currentHUDWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);

		if (nullptr != currentHUDWidget)
		{
			currentHUDWidget->AddToViewport();
			widgetHudTree = currentHUDWidget->WidgetTree;
		}
	}
	else
	{
		UE_LOG(LogS47HUD, Error, TEXT("currentHUDWidget is nullptr"));
	}

	currentPauseMenuWidget = CreateWidget<UUserWidget>(GetWorld(), PauseMenuWidgetClass);
	if (nullptr != currentPauseMenuWidget)
	{
		currentPauseMenuWidget->AddToViewport();
		inGameWidgetTree = currentPauseMenuWidget->WidgetTree;
		currentPauseMenuWidget->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		UE_LOG(LogS47HUD, Error, TEXT("currentPauseMenuWidget is nullptr"));
	}

	if (nullptr != characterOwner)
	{
		if (nullptr != characterOwner->GetCharacterMovement())
		{
			characterOwner->GetCharacterMovement()->MaxWalkSpeed = 600.0f;
			characterOwner->canBeDamaged = true;
		}
	}

	if (nullptr != currentLoadingWidget)
	{
		currentLoadingWidget->RemoveFromViewport();
	}

	if (nullptr != GetOwningPlayerController())
	{
		GetOwningPlayerController()->SetInputMode(FInputModeGameOnly());
	}
}

void AS47HUD::PreGeneration()
{
	if (nullptr != LoadingWidgetClass)
	{
		currentLoadingWidget = CreateWidget<UUserWidget>(GetWorld(), LoadingWidgetClass);

		if (nullptr != currentLoadingWidget)
		{
			currentLoadingWidget->AddToViewport();
		}
	}

	if (nullptr != GetOwningPlayerController())
	{
		GetOwningPlayerController()->SetInputMode(FInputModeUIOnly());
	}
}