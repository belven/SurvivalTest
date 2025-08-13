// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class SurvivalTestEditorTarget : TargetRules
{
	public SurvivalTestEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_5;
        ExtraModuleNames.Add("SurvivalTest");
        bLegacyParentIncludePaths = true;
        WindowsPlatform.bStrictConformanceMode = true;
        bValidateFormatStrings = true;
    }
}
