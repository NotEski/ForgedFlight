/docs/GDD/04-systems/world-generation.md
# 04 – Systems / World Generation

> Design spec for **procedural planet generation**. Covers inputs, classes, cubesphere mapping, climate/biomes, ore/hazard placement, caves, spawn rules, and low-res worldmap bakes. Execution/streaming lives in *World Sim & Loading*; rendering details in *Rendering (bgfx)*.

## Goals & Principles
- **Deterministic from a seed** (plus worldgen version): the same inputs reproduce the same world.
- **Readable realism:** geology/climate that feel plausible, not fantasy.
- **Playable first:** safe starts, reachable resources, and clear progression hooks.
- **Budget-aware:** generation compatible with 32³ chunks, 16³ regions, and our streaming budgets.

## Inputs & Versioning
- **World Seed** (u64), **Worldgen Version** (semver), **Feature Flags** (e.g., HostileFauna).  
- **Planet Template** (class + parameters) per body.  
- Persist `{seed, version, template params}` in save header.

## Planet Classes & Sizes  *(diameter; [TENTATIVE])* 
- **Small Planet:** 25–75 km  
- **Standard Planet (default):** 100–150 km  
- **Mega Planet:** 150–250 km  
- **Gas Giants:** Mini 300–400 km, Standard 400–500 km, Large 500–600 km (no voxel terrain; see *Gas Giant Planets*).

## Cubesphere Mapping
- Six cube faces projected to a sphere; each face has a quadtree LOD.  
- **Evaluation space:** do noise in **cube-local** space to avoid pole stretching; project to sphere for normals.  
- **Seams:** blend across face borders; additional tri-planar blend for materials.  
- See *World Sim & Loading* for streaming/LODs.

## Heightfield & Materials (Solids)
- **Base height** = f(lat, long, seed) via layered noise (ridge/FBM/warp).  
- **Material strata**: soil → sedimentary → igneous/metamorphic → **Mantlerock** (unmineable).  
- **Water level:** spherical sea level; carve shorelines; optional rivers **[OPEN]**.

## Climate Model
- Inputs: latitude, elevation, ocean distance, prevailing wind bands, tilt/season.  
- Outputs: temperature, humidity, precipitation likelihood, **biome key**.  
- **Seasons:** axial tilt + orbit drive temperature bands and precipitation over time.

## Biomes  **[OPEN]**
- Draft list (TBD): polar ice, tundra, taiga, temperate forest, grassland, desert, savanna, tropical forest, wetlands, alpine/rock, volcanic.  
- Each biome defines: flora set, fauna (if enabled), soil/ore visibility tweaks, weather ranges.

## Caves & Voids
- **Primary caverns:** 3D noise tunnels with occasional chambers.  
- **Secondary features:** lava tubes, karst pockets near sedimentary bands.  
- Connectivity target **[OPEN]** (e.g., 0.4–0.6 of chunks intersecting a tunnel network at depth).

## Ores & Resources
- **Finite** deposits in veins/fields; **late-game renewable** processes exist but are costlier (per earlier design).  
- Placement rules:
  - **Veins:** per-material frequency, depth band, cluster size/variance.  
  - **Placers:** surface scatter for plants/clays/salts.  
  - **Conflict resolution:** ensure early-game path (copper/stone/fuel/water) within **N** km of spawn **[OPEN]**.  
- **Hazards:** tag blocks (e.g., **lead**, **arsenic**) for toxicity; exposure/processing penalties handled by survival/chemistry systems.

## Spawn Rules
- Prefer **Standard** planets. Pick temperate biome near fresh water, gentle terrain, and accessible stone/wood/ores.  
- Crash-landing intro **[OPEN]** (cinematic yes/no).  
- Guarantee “first-hour kit” radius by sampling nearby regions until constraints met.

## Weather & Clouds (Solids)
- Weather fields driven by climate; precipitation spawns where cloud accumulation exceeds threshold.  
- **Dynamic weather** is **Very Low Priority** (documented elsewhere); keep generators pluggable.

## Gas Giants (Summary)
- No voxel terrain. **Layered atmosphere** with bands/storms; procedural **cells** drive winds.  
- Only **collection factories** allowed (compression/filtration/canisters).  
- Full details in *Gas Giant Planets*.

## Low-Res Worldmap Bake
- Bake a **low-res faceted map** per planet for:
  - **Distant impostor albedo/normal hints**  
  - **UI maps/minimap overlays**  
- Contents: height (min/max normalized), biome index, water mask, snow/ice mask.  
- Resolution **[OPEN]** (e.g., 1024² per face).  
- Re-bake triggers: new seed, worldgen version change, major biome parameter change.

## Procedural Guardrails (Skies/Orbits)
- Enforce **apparent-size cap** of neighbor bodies **[OPEN]°** from ground.  
- **Period bands:** moons minutes; planets hours **[OPEN]**; clamp eclipses **[OPEN]**.  
- See *World Sim & Loading* for rails ephemerides.

## Performance Targets  **[OPEN]**
- Gen time per region (ms), VRAM budgets for impostor maps, mesh cache sizes.

## Acceptance (MVP)
- Given a seed, generate: cubesphere terrain, climate/biomes, water, caves, ores, and a valid **spawn** meeting constraints.  
- Bake worldmap; seams hidden; orbits respect guardrails.

**TODO:** Biome catalog, ore tables, spawn radii/guarantees, river algorithm choice, guardrail constants, bake resolution.
