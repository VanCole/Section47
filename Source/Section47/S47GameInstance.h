// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "UObject/ObjectMacros.h"
#include "S47GameInstance.generated.h"

/**
 *
 */
UCLASS()
class SECTION47_API US47GameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	US47GameInstance(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable)
	static FString GetRandomTip();

	template<typename T>
	static void Shuffle(TArray<T>& _array)
	{
		for (int i = 0; i < _array.Num(); i++)
		{
			int newIndex = m_random.RandRange(0, _array.Num() - 1);
			_array.Swap(i, newIndex);
		}
	}

	UPROPERTY(EditAnywhere)
	bool blockEnemySpawn = false;

private:
	static FRandomStream m_random;
};
