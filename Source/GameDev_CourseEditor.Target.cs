// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class GameDev_CourseEditorTarget : TargetRules
{
	public GameDev_CourseEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V4;

		ExtraModuleNames.AddRange(new string[] { "GameDev_Course" });
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_3;
    }
}
