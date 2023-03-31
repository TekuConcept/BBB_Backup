A memo to save time setting up proper colors for 2D / NPR games.
-- Tested with UE5.1 --

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
In a C++ Actor, under `BeginPlay`, add:
```
UWorld* world = GetWorld();
UGameViewportClient* viewport = world->GetGameViewport();
FEngineShowFlags& showFlags = *viewport->GetEngineShowFlags();

ApplyViewMode(EViewModeIndex::VMI_Unlit, /*perspective=*/false, showFlags);
showFlags.EyeAdaptation = 0;
showFlags.Tonemapper = 0;
```
Then add the actor to the world.

**TODO**: Game section still needs troubleshooting...

---

* Add the following to _[Project]/Config/DefaultEngine.ini_
```
[/Script/Engine.RendererSettings]
...
r.TonemapperGamma = 0
r.TonemapperFilm = 0
r.Tonemapper.Quality = 0
r.ToneCurveAmount = 0
r.AntiAliasingMethod=1
```


* Disable 'sRGB' under _Texture_ for each texture asset


* Post-Processing
1. 'Quickly add to the project' > Volumes > PostProcessVolume
2. Set: Global
  2.1. Saturation: checked & set to 1.3 (alt 1.6)
  2.2. Contrast: checked & set to 0.85
  2.1. Gamma: checked & set to 1
  2.2. Gain: checked & set to 0.55 (alt 0.6)
3. Set: Post Process Volume Settings > Infinite Extent (Unbound): Checked

