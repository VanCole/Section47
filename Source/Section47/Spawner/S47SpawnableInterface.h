// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/Interface.h"
#include "S47SpawnableInterface.generated.h"

/**
 * 
 */
UINTERFACE()
class SECTION47_API US47SpawnableInterface : public UInterface
{
	GENERATED_BODY()
};


class SECTION47_API IS47SpawnableInterface
{
	GENERATED_IINTERFACE_BODY()

public:
	virtual int32 GetWeight() { return 0; }
};
