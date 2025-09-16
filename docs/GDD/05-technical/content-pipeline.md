# 05 – Technical / Content Pipeline & Tools
## Modeling
- **Blockbench** → **glTF 2.0 (.glb)**.
- Scale: **1 unit = 1 m**.
- Faceted/flat shading; no curved geometry (planets excepted).

## Textures
- Compressed **KTX2**; low-res for pixel look; palette anchored across UI.

## Import Validation
- Model Viewer checks: unit scale, origin, collision, material slots, LODs.
- Auto-bake thumbnails.

## File Layout
- `/assets/models`, `/assets/textures`, `/assets/blueprints`, `/assets/ui`.
- Naming: `family_name_variant_v###.glb`.

## Terrain & Planets
- Worldmap bake created during worldgen; per-face textures referenced by renderer.

[OPEN] Exact texture budgets; tri-planar weight tuning; asset hot-reload.