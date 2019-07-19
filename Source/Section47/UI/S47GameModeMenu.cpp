// Fill out your copyright notice in the Description page of Project Settings.

#include "S47GameModeMenu.h"
#include "S47MenuHud.h"

AS47GameModeMenu::AS47GameModeMenu() : Super()
{
	// use our custom HUD class
	HUDClass = AS47MenuHud::StaticClass();
}