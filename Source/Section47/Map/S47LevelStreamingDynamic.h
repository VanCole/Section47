// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Engine/LevelStreaming.h"
#include "S47LevelStreamingDynamic.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class SECTION47_API US47LevelStreamingDynamic : public ULevelStreaming
{
	GENERATED_UCLASS_BODY()

	/** Whether the level should be loaded at startup																			*/
	UPROPERTY(Category = LevelStreaming, EditAnywhere)
	uint32 bInitiallyLoaded : 1;

	/** Whether the level should be visible at startup if it is loaded 															*/
	UPROPERTY(Category = LevelStreaming, EditAnywhere)
		uint32 bInitiallyVisible : 1;

	/**
	* Stream in a level with a specific location and rotation. You can create multiple instances of the same level!
	*
	* The level to be loaded does not have to be in the persistent map's Levels list, however to ensure that the .umap does get
	* packaged, please be sure to include the .umap in your Packaging Settings:
	*
	*   Project Settings -> Packaging -> List of Maps to Include in a Packaged Build (you may have to show advanced or type in filter)
	*
	* @param LevelName - Level package name, ex: /Game/Maps/MyMapName, specifying short name like MyMapName will force very slow search on disk
	* @param Location - World space location where the level should be spawned
	* @param Rotation - World space rotation for rotating the entire level
	* @param bOutSuccess - Whether operation was successful (map was found and added to the sub-levels list)
	* @return Streaming level object for a level instance
	*/
	UFUNCTION(BlueprintCallable, Category = LevelStreaming, meta = (DisplayName = "Load Level Instance (by Name)", WorldContext = "WorldContextObject"))
		static US47LevelStreamingDynamic* LoadLevelInstance(UObject* WorldContextObject, FString LevelName, FVector Location, FRotator Rotation, bool& bOutSuccess);

	UFUNCTION(BlueprintCallable, Category = LevelStreaming, meta = (DisplayName = "Load Level Instance (by Object Reference)", WorldContext = "WorldContextObject"))
		static US47LevelStreamingDynamic* LoadLevelInstanceBySoftObjectPtr(UObject* WorldContextObject, TSoftObjectPtr<UWorld> Level, FVector Location, FRotator Rotation, bool& bOutSuccess);



	UFUNCTION(BlueprintCallable, Category = LevelStreaming, meta = (DisplayName = "Load Level Instance (by Room Data)", WorldContext = "WorldContextObject"))
	static US47LevelStreamingDynamic* Load(UObject* WorldContextObject, class US47RoomData* Data, FVector Location, FRotator Rotation);
	UFUNCTION(BlueprintCallable, Category = LevelStreaming, meta = (DisplayName = "Unload Level Instance", WorldContext = "WorldContextObject"))
	static void Unload(UObject* WorldContextObject, US47LevelStreamingDynamic* Instance);


	//~ Begin UObject Interface
	virtual void PostLoad() override;
	//~ End UObject Interface

	//~ Begin ULevelStreaming Interface
	virtual bool ShouldBeLoaded() const override { return bShouldBeLoaded; }
	//~ End ULevelStreaming Interface

	virtual void SetShouldBeLoaded(bool bShouldBeLoaded) override;

	UFUNCTION(BlueprintCallable)
	void OnLevelDynamicUnloaded();

	bool IsLevelUnloaded() { return bIsUnloaded; }

public:
	// Counter used by LoadLevelInstance to create unique level names
	static int32 UniqueLevelInstanceId;
private:

	uint32 bIsUnloaded : 1;

	static US47LevelStreamingDynamic* LoadLevelInstance_Internal(UWorld* World, const FString& LongPackageName, FVector Location, FRotator Rotation, bool& bOutSuccess);
};
