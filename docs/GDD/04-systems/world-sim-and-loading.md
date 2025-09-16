/docs/GDD/04-systems/world-sim-and-loading.md
# 04 – Systems / World Sim & Loading

> Execution/streaming model for planets (chunks/regions/LODs), time-of-day/seasons, and rails orbits.  
> **World generation inputs & rules live in:** [World Generation](./world-generation.md).

## Planet Topology & Scale
- **[DECISION]** **Cubesphere** planets (six faces projected to a sphere); fully destructible voxel volume.
- **[TENTATIVE] Planet Size Classes (diameter):**
  - **Small Planet:** **25–75 km** — tight travel arcs, fewer biomes, compact progression.
  - **Standard Planet (default):** **100–150 km** — baseline playable scale and biome spread.
  - **Mega Planet:** **150–250 km** — rarer, long-haul logistics set pieces.
  - *(Micro-bodies \<25 km remain moons/asteroids and may use simplified rules.)*
- **Gas Giants:** see dedicated bands in *Gas Giant Planets*; rendered as analytic spheres with layered atmospheres.
- **Seam treatment:** oceans/sea-ice/cloud/fog bands and shader blending to hide face corners/edges.

## Procedural Orbital Generator (Rails; No Set Pieces)
- **Bodies on Rails:** stars, planets, and moons follow **precomputed analytic ephemerides** seeded per world.  
- **Constraints:** apparent-size cap, target period bands (moons minutes; planets hours), eclipse clamp, stability damping.  
- **Outputs:** for each body: `epoch_t0, a, e, i, Ω, ω, M0` (or equivalent); update is **pure function of time** → deterministic, cheap.  
- **Player Craft:** use **Hybrid Rails** (see Vehicles & Physics).  
- **Scheduler:** server maintains an **orbital event queue** (SoI, entries, impacts, burn nodes) and wakes LIVE physics just-in-time.


## Coordinates & Chunking
### Discrete Units (Voxels/Chunks/Regions)
- **[DECISION] Chunk size:** **32×32×32 voxels** (voxel = 1 m → **32 m** per chunk edge).
- **[DECISION] Region size:** **16×16×16 chunks** → **512 m** per region edge; regions are primary IO/save units.
- **[DECISION] Build height:** **2048 m** vertical voxel span per planet (mantlerock → sky cap) = **64 chunks** tall.
- **Altitude Zones:** Above sky cap transitions to **near-orbit** (impostor handoff) with streaming fade.

## Streaming & LOD
- **LOD0 Live:** nearby chunks/entities (players, WI machines).  
- **LOD1 Headless:** factory IR runs in background; no chunk load.  
- **LOD2 Frozen:** paused.  
- Face-edge traversal remaps coordinates to adjacent face; physics/collision handle continuous normals.  
- **Planet impostor** (low-res) for far view; **high-res** tiles stream as you approach.

## Water & Atmosphere
- Spherical **sea level** with seamless water mesh across faces; optional shoreline tessellation.  
- Atmosphere: sky/lighting via solar elevation; re-entry bloom/heat near space; scattering visible on other planets.

## Time of Day & Seasons
- Rotation + axial tilt drive **day/night** and **seasons** per latitude.  
- **Outer Wilds-like** faster orbits (minutes/hours) to keep launch windows fun.

## Loading Budget & Catch-up
- Server **autoload budget** prioritizes: players > WI zones > approaching trains/vehicles.  
- Deterministic **catch-up** on resume/LOD swap; ports are authoritative to prevent dupes.  
- **[OPEN]** Default headless tick step and max catch-up window.

## Save/Load
- Persist planet seeds, face quadtree states, **regions** and changed **chunks**, factory controller snapshots, lab queues.  
- **[OPEN]** Snapshot format (JSON v1 vs binary) and retention.

## Gas Giants
- **No voxel terrain:** treat planet as an **analytic sphere** with **layered atmosphere** (no chunk grid).
- **Size Bands:** see *Gas Giant Planets* for **Mini 300–400 km**, **Standard 400–500 km**, **Large 500–600 km** (**[TENTATIVE]**).
- **Cells vs Chunks:** simulate weather/flow as **spherical cells** rather than voxel chunks. **[OPEN]** Default cell resolution.
- **Active Zones:** load cell neighborhoods around players/vehicles/platforms; factories on platforms tick **WI/CP** as usual.
- **Save/Load:** persist weather seeds and layer parameters rather than terrain deltas.

## Mantlerock Layer
- **[DECISION]** The bottom-most, **unmineable** layer of each solid planet. Represents the mantle’s upper boundary.
- Interacts with construction as a **stable foundation** (perfect support; no collapse checks needed).
- Mining attempts always fail with: *“Mantlerock cannot be mined.”*
- Worldgen: exposes as a thin cap at the lowest elevations of caves/chasms; color tinted per planet.
