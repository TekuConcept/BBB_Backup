A memo to save time setting up pixel-perfect colors for 2D / NPR games.

*-- Tested with UE5.1 --*

**Project Settings:**
- Anti-Aliasing Method: Fast Approximate Anti-Aliasing (FXAA)
- Auto Exposure: Disabled

**Editor:**
Under "Show," disable the following:
- Post Processing
  -  Eye Adaptation
  -  Tonemapper
- Set 'Lit' to 'Unlit'

**Game:**
1. Create an actor class in C++ called something like "GameViewEffect"
2. Under the function, `BeginPlay`, add the following snippet:
```
UWorld* world = GetWorld();
UGameViewportClient* viewport = world->GetGameViewport();
FEngineShowFlags& showFlags = *viewport->GetEngineShowFlags();

ApplyViewMode(EViewModeIndex::VMI_Unlit, /*perspective=*/false, showFlags);
showFlags.EyeAdaptation = 0;
showFlags.Tonemapper = 0;
```
3. Add the actor to the world, and done!
