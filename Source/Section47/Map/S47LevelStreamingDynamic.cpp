// Fill out your copyright notice in the Description page of Project Settings.

#include "S47LevelStreamingDynamic.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "Engine.h"
#include "Section47.h"
#include "S47RoomData.h"

#define LOCTEXT_NAMESPACE "World"

int32 US47LevelStreamingDynamic::UniqueLevelInstanceId = 0;

US47LevelStreamingDynamic::US47LevelStreamingDynamic(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void US47LevelStreamingDynamic::PostLoad()
{
	Super::PostLoad();

	// Initialize startup state of the streaming level
	if (GetWorld()->IsGameWorld())
	{
		bShouldBeLoaded = bInitiallyLoaded;
		SetShouldBeVisible(bInitiallyVisible);
	}
}

void US47LevelStreamingDynamic::SetShouldBeLoaded(const bool bInShouldBeLoaded)
{
	if (bInShouldBeLoaded != bShouldBeLoaded)
	{
		bShouldBeLoaded = bInShouldBeLoaded;
		if (UWorld* World = GetWorld())
		{
			World->UpdateStreamingLevelShouldBeConsidered(this);
		}
	}
}


void US47LevelStreamingDynamic::OnLevelDynamicUnloaded()
{
	//UE_LOG(LogS47Level, Warning, TEXT("End unload level: %s"), *GetWorldAssetPackageName());
	UWorld* World = GetWorld();
	if (nullptr != World)
	{
		//UE_LOG(LogS47Level, Warning, TEXT("Remove instance from world"));
		World->RemoveStreamingLevel(this);
	}
	bIsUnloaded = true;
}

US47LevelStreamingDynamic* US47LevelStreamingDynamic::LoadLevelInstance(UObject* WorldContextObject, const FString LevelName, const FVector Location, const FRotator Rotation, bool& bOutSuccess)
{
	bOutSuccess = false;
	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return nullptr;
	}

	// Check whether requested map exists, this could be very slow if LevelName is a short package name
	FString LongPackageName;
	bOutSuccess = FPackageName::SearchForPackageOnDisk(LevelName, &LongPackageName);
	if (!bOutSuccess)
	{
		return nullptr;
	}

	return LoadLevelInstance_Internal(World, LongPackageName, Location, Rotation, bOutSuccess);
}

US47LevelStreamingDynamic* US47LevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(UObject* WorldContextObject, const TSoftObjectPtr<UWorld> Level, const FVector Location, const FRotator Rotation, bool& bOutSuccess)
{
	bOutSuccess = false;
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return nullptr;
	}

	// Check whether requested map exists, this could be very slow if LevelName is a short package name
	if (Level.IsNull())
	{
		return nullptr;
	}

	return LoadLevelInstance_Internal(World, Level.GetLongPackageName(), Location, Rotation, bOutSuccess);
}

US47LevelStreamingDynamic * US47LevelStreamingDynamic::Load(UObject * WorldContextObject, US47RoomData * Data, FVector Location, FRotator Rotation)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (nullptr == World)
	{
		UE_LOG(LogS47Level, Error, TEXT("Failed to load LevelStreamingDynamic: World is null"));
		return nullptr;
	}
	if (nullptr == Data)
	{
		UE_LOG(LogS47Level, Error, TEXT("Failed to load LevelStreamingDynamic: Data is null"));
		return nullptr;
	}

	bool success = false;
	US47LevelStreamingDynamic* Instance = US47LevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(World, Data->level, Location, Rotation, success);

	if (!success)
	{
		UE_LOG(LogS47Level, Error, TEXT("Failed to load LevelStreamingDynamic: Unknown reason"));
		return nullptr;
	}

	return Instance;
}

void US47LevelStreamingDynamic::Unload(UObject * WorldContextObject, US47LevelStreamingDynamic * Instance)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (nullptr == World)
	{
		UE_LOG(LogS47Level, Error, TEXT("Failed to unload LevelStreamingDynamic: World is null"));
		return;
	}
	if (nullptr == Instance)
	{
		UE_LOG(LogS47Level, Error, TEXT("Failed to unload LevelStreamingDynamic: Instance is null"));
		return;
	}

	// Prefix to remove
	FString LevelName = Instance->GetWorldAssetPackageName();
	const FString PackagePath = FPackageName::GetLongPackagePath(LevelName);
	FString LevelPackageName = PackagePath + TEXT("/") + World->StreamingLevelsPrefix;

	LevelName.RemoveFromStart(LevelPackageName, ESearchCase::IgnoreCase);
	FLatentActionInfo LatentInfo(0, 2222, TEXT("OnLevelDynamicUnloaded"), Instance);
	UGameplayStatics::UnloadStreamLevel(World, *LevelName, LatentInfo, false);
}

US47LevelStreamingDynamic* US47LevelStreamingDynamic::LoadLevelInstance_Internal(UWorld* World, const FString& LongPackageName, const FVector Location, const FRotator Rotation, bool& bOutSuccess)
{
	// Create Unique Name for sub-level package
	const FString ShortPackageName = FPackageName::GetShortName(LongPackageName);
	const FString PackagePath = FPackageName::GetLongPackagePath(LongPackageName);
	FString UniqueLevelPackageName = PackagePath + TEXT("/") + World->StreamingLevelsPrefix + ShortPackageName;

	//UE_LOG(LogS47Level, Warning, TEXT("Unique Id: %d"), UniqueLevelInstanceId);

	UniqueLevelPackageName += TEXT("_LevelInstance_") + FString::FromInt(++UniqueLevelInstanceId);

	// Setup streaming level object that will load specified map
	US47LevelStreamingDynamic* StreamingLevel = NewObject<US47LevelStreamingDynamic>(World, US47LevelStreamingDynamic::StaticClass(), NAME_None, RF_Transient, NULL);
	StreamingLevel->SetWorldAssetByPackageName(FName(*UniqueLevelPackageName));
	StreamingLevel->LevelColor = FColor::MakeRandomColor();
	StreamingLevel->SetShouldBeLoaded(true);
	StreamingLevel->SetShouldBeVisible(true);
	StreamingLevel->bShouldBlockOnLoad = false;
	StreamingLevel->bInitiallyLoaded = true;
	StreamingLevel->bInitiallyVisible = true;
	// Transform
	StreamingLevel->LevelTransform = FTransform(Rotation, Location);
	// Map to Load
	StreamingLevel->PackageNameToLoad = FName(*LongPackageName);

	// Add the new level to world.
	World->AddStreamingLevel(StreamingLevel);

	bOutSuccess = true;
	return StreamingLevel;
}