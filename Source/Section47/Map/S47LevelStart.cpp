// Fill out your copyright notice in the Description page of Project Settings.

#include "S47LevelStart.h"
#include "Engine/World.h"
#include "Section47.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"

AS47LevelStart::AS47LevelStart()
	: Super()
{
	GetCapsuleComponent()->SetCapsuleSize(50.0f, 100.0f);
}

AS47LevelStart * AS47LevelStart::GetFirstLevelStart(UWorld* _World)
{
	AS47LevelStart* LevelStart = nullptr;
	if (nullptr != _World)
	{
		TArray<AActor*> Starts;
		UGameplayStatics::GetAllActorsOfClass(_World, AS47LevelStart::StaticClass(), Starts);
		if (Starts.Num() > 0)
		{
			LevelStart = Cast<AS47LevelStart>(Starts[0]);
		}
		else
		{
			UE_LOG(LogS47Level, Warning, TEXT("No LevelStart found in the world"));
		}
	}
	else
	{
		UE_LOG(LogS47Level, Warning, TEXT("World is NULLPTR"));
	}

	return LevelStart;
}
