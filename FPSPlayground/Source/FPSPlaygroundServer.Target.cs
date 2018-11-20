using UnrealBuildTool;
using System.Collections.Generic;

[SupportedPlatforms(UnrealPlatformClass.Server)]
public class FPSPlaygroundServerTarget : TargetRules
{
       public FPSPlaygroundServerTarget(TargetInfo Target) : base(Target)
       {
        Type = TargetType.Server;
        BuildEnvironment = TargetBuildEnvironment.Shared;
        ExtraModuleNames.Add("FPSPlayground");
       }
}
