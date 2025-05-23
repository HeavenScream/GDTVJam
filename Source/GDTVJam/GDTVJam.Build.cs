// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GDTVJam : ModuleRules
{
	public GDTVJam(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "GameplayTags", "GameplayAbilities", "GameplayTasks", "AIModule", "NavigationSystem" });
	}
}
