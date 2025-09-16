# 04 – Systems / Factories & Logistics

## Factory Categories
- **World‑Interacting (WI):** miners, block placers, physics spawners → require **Activation Zone** (chunks loaded).  
- **Contained Processing (CP):** smelters/assemblers/refineries → **headless** via invisible **Factory Controllers** (no chunk load).  
- **Hybrid:** WI nodes request loading; CP portion stays headless.

## Invisible Factory Controllers
- Auto‑created when a machine first runs; discover zone via belts/pipes/power graph.  
- Compile **headless IR**: machines → proxies (rates/buffers), belts/pipes → capacity+latency edges.  
- Run on server **background scheduler** with adaptive step; **snapshot** and **catch‑up** on resume/LOD swap.  
- **Ports** (Item/Fluid/Power/Train) bridge to global networks; power gets a **budget** from Energy Solver.

## Logistics
- **Belts/Chutes/Pipes** modeled as capacity+latency in headless.  
- **Trains:** stations expose **lanes** (Item/Fluid/Power). Player‑built trains have **IO manifolds**; a **Dock Profiler** caches a per train×station **dock profile** (throughput & dwell).  
- **Virtual trains** move cargo headless; trains **spawn** near players.

## UI/Debug
- Zone overlay: LOD, throughput, buffers, power, warnings.  
- Lines UI: schedules, block occupancy, virtual vs spawned.

**[OPEN]** Throughput targets for belts/pipes and baseline train lane capacities?  

## Blueprints & Construction Assist
See: [Blueprints & Construction Assist](./blueprints-and-construction.md).

- **MVP:** LV **Blueprint Table** + **Placement Tool** creates/places ghosts; build consumes **power** and **items** from connected chests.
- **MV:** **Robotic Builder Arm** builds ghosts; applies saved settings.
- **HV+:** **Builder Bots** from **Builder Bay** fetch items and construct; late‑game deconstruction & repair.
- **WI constraint:** construction is **World‑Interacting** and requires an **Activation Zone** (no headless building).

## Labs & Logistics
- Labs are **Contained Processing**; eligible for **headless ticking**.
- Inputs via belts/pipes; **Lab Link Cable** forms research networks (data bus).
- Outputs are **Research Artifacts** to ports/chests; optionally player-targeted.

## UI/Debug
- Zone overlay: LOD, throughput, buffers, power, warnings.
- Lines UI: schedules, block occupancy, virtual vs spawned.

## Mechanical Power Transport (Contraptions)
- **Shafts/Geartrains/Bands:** explicit block chains carry **mechanical power** between machines/contraptions; losses per joint **[OPEN]%**.
- **Clutches & Gearboxes:** route power, split lines, engage/disengage loads; allow ratio changes (rpm↔torque).
- **Motor/Generator Stations:** convert **Mechanical↔DC** where needed (see Power doc).  
- **Headless Policy:** purely mechanical networks that drive **WI** machines require zone load; CP processing can remain headless behind a motor/generator boundary.

## Atmospheric Logistics (Gas Giants)
- **Collection Only:** rigs/collectors run as **WI**; minimal **CP** allowed for compression/filtration and **canisterization**.
- **Standard Gas/Aerosol Canisters:** portable container SKUs (**[OPEN]** S/M/L) that flow on belts, ports, and rocket cargo.
- **Rocket Logistics Port (Platform):** designated node that batches canisters and dispatches **automated rockets** to a target **Factory Port** on a solid planet/station.
- **Return Flow:** empty canisters return via rockets to platforms (closed loop).
- **Headless Policy:** collection rigs are **WI** (require loaded cell neighborhood). **[OPEN]** Option: allow low-fidelity headless yield when platform has an active sensor suite.

## Warehousing & Orders
- **Warehouse Zones** with shelves/racks and **Terminals** (post-scanner) show **real stock** and **locations**.  
- **Orders**: create pick/pack/ship jobs between depots; jobs are executed by belts/vehicles/trains/airships/rockets.  
- The UI aggregates counts but **does not create subspace storage**; every item lives somewhere.  
- **Kit Racks** integrate with Orders to assemble build kits automatically once tech is unlocked.
