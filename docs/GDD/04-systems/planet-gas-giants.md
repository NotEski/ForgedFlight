/docs/GDD/04-systems/planet-gas-giants.md
# 04 – Systems / Gas Giant Planets

> **Design:** A **gas giant** is a **landless** planet type. There is **no mineable terrain**. Gameplay happens within **semi-volumetric cloud layers** that get **denser with depth**. It runs a simplified simulation (no voxel grid) and is primarily about **aerostat/aircraft operations**, **atmospheric resource harvesting**, and **hazard navigation**.

## Goals
- Offer a distinct loop from solid planets: **fly, float, skim, and harvest** rather than dig/build on ground.
- Be technically simpler: **single analytic sphere** + layered atmosphere; no chunked voxel terrain.
- Create memorable set pieces (bands, storms, lightning, auroras) with **fast orbital pacing**.

## Size Bands  **[TENTATIVE]**
- **Mini Gas Giant:** **300–400 km** diameter.  
- **Standard Gas Giant:** **400–500 km** diameter.  
- **Large Gas Giant:** **500–600 km** diameter.

## Topology & Scale
- Analytic **sphere** (cubesphere render shell), **no voxel terrain**.
- Vertical extent subdivided into **cloud layers** (L1…Ln) with increasing **density** and **pressure** toward the center.
- **Surface building:** not allowed; construction is on **floating platforms**.

## Simulation (Overview)
- **Layers:** Each layer defines (ρ density, T temperature, v_wind zonal/meridional, q_haze/aerosol). **[OPEN]** Final layer count (e.g., 5–7).
- **Bands & Storms:** Latitude bands with opposing winds; embedded storms (vortices) and lightning cells.
- **Vertical Profile:** Density/temperature curves (barometric-like). Set **safe altitude window** for gameplay. **[OPEN]** Exact curves.
- **Hazards:** Turbulence, lightning, acid rain (if present), high wind shear, hypoxia/pressure. Scaled by difficulty.
- **Resources:** Atmospheric gases (H₂, He, N₂, CH₄, NH₃, CO₂…), aerosols. **[OPEN]** Which to expose at MVP.

## Rendering
- **Semi-volumetric clouds:** shell-layer quads with 3D noise and cheap ray-march per-fragment; far distance uses impostor.  
- **Bands/Storms:** animated UV flows; high-albedo storm tops; lightning billboards + volumetric flashes.  
- **Altitude Haze:** density-based fog and color shift by layer.  
- **[OPEN]** Quality tiers for low-end GPUs.

## Gameplay Rules
- **No ground:** cannot place regular blocks in free air.  
- **Floating Structures:** place **Sky Platform Frames** (blueprintable) that provide buoyancy via **lifting gas cells**; expansions add anchors, mooring masts, pads.
- **Factory Policy (Important):** Gas giants are for **collection only**. On-planet factories are **physics contraptions/rigs** (WI): scoops, sorbent beds, aerosol harvesters, compressors. **Heavy processing/refining is disallowed** here—materials must be shipped to solid planets or stations.
  - **Allowed CP:** limited **compression/filtration** and **canisterization** (seal into standard **Gas Canisters**).
  - **Disallowed CP:** smelting, cracking, large-scale synthesis/refining.
- **Collectors & Rigs:** tethered skimmers, trawlers, intake balloons, VTOL skims; built modules that physically interact with winds/density.
- **Logistics:** output is sealed **Gas/Aerosol Canisters** routed to a **Rocket Logistics Port** on the platform for automated dispatch to designated off-world factories. **No trains**.
- **Vehicles:** airships, planes, VTOL craft, rocket hoppers; equipment for lightning protection and de-icing.  
- **Survival:** temperature, pressure, O₂; suit/vehicle seals required; staging via platforms.

## Integration
- **Transportation:** Airships/Planes milestones become **requirements** here.  
- **Chemistry:** lifting gases, corrosion-resistant materials, fuels from atmospheric harvest.  
- **Electronics:** sensors, lightning protection, autopilots.  
- **Rocketry/Astronomy:** high-altitude launch pads on platforms; satellites map bands/storms for routing.

## Standard Canisters
- Unified container items produced by collectors/compressors on platforms; compatible with belts/ports and rocket cargo.
- Sizes: **[OPEN]** S / M / L (define mass/volume/pressure caps). Consider **iso-connector** for all machines.
- Contents types: gases (H₂/He/N₂/CH₄/NH₃/CO₂), aerosols/haze. **[OPEN]** Exact MVP list.
- Safety: overpressure failsafe; difficulty scales leakage/damage.

## Difficulty Hooks
- **Easy:** gentler winds, fewer storms, generous lift.  
- **Normal:** baseline profiles.  
- **Hard/Ultra:** stronger shear/storm frequency; lightning damage; stricter lift margins.

## Acceptance (MVP)
- Enter a gas giant; see multi-layer clouds and bands; deploy a **Sky Platform Frame**; operate an **airship**; harvest a gas; survive storms with readable feedback.

**TODO:** Define platform part list (cells, frames, mooring, pad, power), atmospheric harvester module, and minimal resource table.  
**[OPEN]** Are **mining drones** allowed (free-fall skimmers) or keep to crewed craft only at MVP?
