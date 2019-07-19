// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/NavigationObjectBase.h"
#include "S47LevelStart.generated.h"

/**
 * 
 */
UCLASS()
class SECTION47_API AS47LevelStart : public ANavigationObjectBase
{
	GENERATED_BODY()
	
public:
	AS47LevelStart();

	static AS47LevelStart* GetFirstLevelStart(UWorld* _World);
};
