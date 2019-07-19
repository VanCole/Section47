// Fill out your copyright notice in the Description page of Project Settings.

#include "S47GameInstance.h"

FRandomStream US47GameInstance::m_random(FMath::Rand());

US47GameInstance::US47GameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FString US47GameInstance::GetRandomTip()
{
	static TArray<FString> tips =
	{
		FString(TEXT("To kill enemies, you have to shoot <img id=\"LeftClick\"/>.")),
		FString(TEXT("Be careful, the fire burns you.")),
		//FString(TEXT("Braziers burn only your eyes.")),
		FString(TEXT("Healing rooms heal you!")),
		FString(TEXT("Run <img id=\"KeyZ\"/><img id=\"KeyQ\"/><img id=\"KeyS\"/><img id=\"KeyD\"/>, jump <img stretch=\"ScaleToFitY\" width=\"90\" id=\"KeySpace\"/> and dash <img stretch=\"ScaleToFitY\" width=\"60\" id=\"KeyShift\"/>.")),
		FString(TEXT("Quickly pressing dash <img stretch=\"ScaleToFitY\" width=\"60\" id=\"KeyShift\"/> after a jump <img stretch=\"ScaleToFitY\" width=\"90\" id=\"KeySpace\"/> perform a super jump!")),
		FString(TEXT("Grenade launcher can explode itself <img id=\"RightClick\"/>!")),
		FString(TEXT("Each weapon has a secondary fire <img id=\"RightClick\"/> (except the pistol).")),
		FString(TEXT("The flashlight <img id=\"MiddleClick\"/> lights up your way!")),
		FString(TEXT("Pick up bonuses at your own risk!")),
		//FString(TEXT("You are weaker than Doomguy.")),
		//FString(TEXT("This game is realist, even if we fight against zombies.")),
		//FString(TEXT("Crick boum, crick boum.")),
		FString(TEXT("You can't carry more than 3 weapons.")),
		FString(TEXT("The pistol can't be thrown.")),
		FString(TEXT("Changing weapon is useful with <img id=\"Key1\"/><img id=\"Key2\"/><img id=\"Key3\"/> or <img id=\"Scroll\"/>.")),
		FString(TEXT("You can kick <img id=\"KeyV\"/> enemies ass.")),
		FString(TEXT("Accessing in game menu <img id=\"KeyEscape\"/> doesn't pause the game.")),
		FString(TEXT("Don't forget to watch behind you!")),
		FString(TEXT("A door can hide another one.")),
		//FString(TEXT("You can enlarge your screen in settings.")),
		FString(TEXT("The Zombie is the weakest enemy.")),
		//FString(TEXT("The Flyers fly.")),
		FString(TEXT("Enemies are a threat only in large number.")),
		FString(TEXT("You will be submerged easily by enemies.")),
		FString(TEXT("Shotgun has an automatic mode <img id=\"RightClick\"/>.")),
		FString(TEXT("You can prefire your Minigun <img id=\"RightClick\"/>.")),
		FString(TEXT("You can explode Rocket launcher missiles <img id=\"RightClick\"/> before impact.")),
		FString(TEXT("The Rifle has 2 barrels. Shoot with <img id=\"LeftClick\"/> and <img id=\"RightClick\"/>.")),
		FString(TEXT("Lasergun has a zoom <img id=\"RightClick\"/>."))
	};

	static int index = tips.Num() - 1;

	if (index >= tips.Num() - 1)
	{
		index = 0;
		Shuffle(tips);
	}
	else
	{
		index++;
	}

	return tips[index];
}
