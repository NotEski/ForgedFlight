/docs/GDD/05-technical/rendering-bgfx.md
# 05 – Technical / Rendering (bgfx)

## Goals
- Stable **cubesphere** terrain rendering (voxel on curved surface) + atmosphere + space impostor.
- Crisp **pixel-art UI** overlays.
- Smooth near-surface ↔ orbital transition (fade/haze), with seam hiding via oceans/fog/ice.

## Precision & Coordinate Frames
- **1 unit = 1 m**; physics/world state in **double precision**.  
- **Floating origin** near active craft; renderer uses high/low-frequency split (origin-relative matrices + far-scene impostors).  
- Frames: **Star-centric** (ephemerides), **Planet-centric inertial** near worlds, **Craft-relative** for cockpit effects.

## Planet Scale References  **[TENTATIVE]**
- **Small Planet:** **25–75 km** diameter  
- **Standard Planet (default):** **100–150 km**  
- **Mega Planet:** **150–250 km**  
- **Gas Giants:** **Mini 300–400 km**, **Standard 400–500 km**, **Large 500–600 km**

> Rendering notes:
> - Keep **angular size** reasonable from neighboring bodies to avoid sky domination; clamp impostor brightness/size as needed.
> - Gas giants are **analytic spheres**; size increases have minimal perf impact vs voxel planets.

## Pipeline Sketch
- Planet as **six face meshes** (quadtree LOD) projected to sphere; normals corrected per-face.
- **Voxel extraction** near player (chunk bands) blended into face meshes (clip/overlay decals for mines/structures).
- **Planet impostor** (low-res cubemap/signed-distance shell) for far view; swap to high-res at altitude threshold.
- Atmosphere scattering & sun/sky from solar elevation; ring/asteroid instancing for gas giants.
- **No Curved Geometry in assets**: faceted forms; only planets are curved.

## Shaders & Assets
- **Models:** Blockbench (.bbmodel) → **glTF 2.0 (.glb)**; textures to **KTX2**.
- **Textures:** low-res, atlas-friendly; tri-planar seam blending for terrain.

## Cameras & Precision
- **[DECISION]** Scale: **1 unit = 1 meter**.
- Planet-centric double precision; **floating origin** around player.
- Orbital camera uses planet-relative frames; transition masked with cockpit/visor haze.

## Terrain Extraction & Budgets
- **Chunk size:** **32×32×32** voxels; active band = a few shells around player.
- **Region:** **16×16×16 chunks** (~512 m edge) for mesh build/saveload batching.
- **Sky cap:** **2048 m** build height; above this, switch toward near-orbit impostor.
- **[OPEN]** Mesh cache limits; target draw-call/VRAM budgets by region.

## Gas Giant Rendering
- **Cloud Layers:** 3–5 concentric shells with 3D noise; lightweight ray-march; impostor at distance.
- **Band Flow:** per-lat band UV scroll; storm textures + lightning flashes.
- **Performance:** quality tier toggles; fallback to layered fog on low-end. **[OPEN]** Step counts & LOD ranges.

## Worldmap Bake (for Impostors & UI)
- Input: low-res per-face textures from **World Generation** (height/biome/water/snow).  
- Build a **planet impostor** atlas for far view; pack a **minimap** overlay texture set.  
- Streaming: swap impostor → high-res tiles near camera altitude; cache bakes per planet seed/version.  
- **[OPEN]** Face resolution (e.g., 1024²), compression (KTX2), update cadence.