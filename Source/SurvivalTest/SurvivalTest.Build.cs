// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SurvivalTest : ModuleRules
{
    public SurvivalTest(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG", "Slate", "SlateCore", "NavigationSystem", "AIModule" });
        bLegacyParentIncludePaths = true;
        CppStandard = CppStandardVersion.Default;
        bValidateFormatStrings = true;
    }
}