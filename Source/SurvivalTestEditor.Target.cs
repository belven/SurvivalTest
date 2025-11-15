// Copyright Epic Games, Inc. All Rights Reserved.
using UnrealBuildTool;

public class SurvivalTestEditorTarget : TargetRules
{
	public SurvivalTestEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
        ExtraModuleNames.Add("SurvivalTest");
        bLegacyParentIncludePaths = true; // TODO update this - would require a lot of changes
        WindowsPlatform.bStrictConformanceMode = true;
        bValidateFormatStrings = true;
    }
}
