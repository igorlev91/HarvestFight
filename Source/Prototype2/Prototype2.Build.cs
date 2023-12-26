// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Prototype2 : ModuleRules
{
	public Prototype2(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput","OnlineSubsystem", "OnlineSubsystemUtils", "UMG" });
		DynamicallyLoadedModuleNames.AddRange(new string[] {  "OnlineSubsystemSteam" });
	}
}
