A memo to save time setting up proper colors for 2D / NPR games.
-- Tested with UE5.1 --


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

