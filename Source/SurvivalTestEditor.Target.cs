// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class SurvivalTestEditorTarget : TargetRules
{
	public SurvivalTestEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.Add("SurvivalTest");
        bLegacyParentIncludePaths = true;
        CppStandard = CppStandardVersion.Default;
        WindowsPlatform.bStrictConformanceMode = true;
        bValidateFormatStrings = true;
    }
}
