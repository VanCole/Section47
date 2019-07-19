// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;


public class Section47 : ModuleRules
{
	public Section47(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "UMG", "Slate", "SlateCore", "GameplayTasks", "NavigationSystem" });
	}
}
