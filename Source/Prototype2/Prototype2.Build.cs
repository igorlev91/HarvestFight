// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Prototype2 : ModuleRules
{
	public Prototype2(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "EnhancedInput", "OnlineSubsystemSteam", "OnlineSubsystem", "OnlineSubsystemUtils", "UMG", "Navmesh", "NavigationSystem", "Niagara", "NiagaraCore", "SlateCore", "Slate"});
		PrivateDependencyModuleNames.AddRange(new string[] { "WhatTheFlock" });
		//, "JsonUtilities","Json", "Sockets",  "HTTP"
		/*DynamicallyLoadedModuleNames.AddRange(new string[] { "WinHttp" });*/
	}
}
