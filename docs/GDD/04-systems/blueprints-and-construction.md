# 04 – Systems / Blueprints & Construction Assist

> MVP targets a **Satisfactory‑style blueprint system** (player‑made plans, place as ghosts, then auto‑build) with a path to **Factorio‑like builder bots** later. Early implementation uses **grid‑snapped ghosts** powered by a **Blueprint Table (LV)** and a **Placement Tool**; mid tiers add **Robotic Arms**; late tiers unlock **flying Builder Bots**.

## Goals
- Reduce repetitive placement for factories while **respecting power & materials**.
- Preserve the **WI vs CP** model: construction is **World‑Interacting (WI)** and needs an **Activation Zone**; production remains headless when eligible.
- Preserve **player intent** (exact orientations/settings) via blueprint metadata.

## Tiered Progression
- **LV (Electricity online): Blueprint Table + Placement Tool**
  - Craft **Blueprint Table** (LV). Create/edit blueprint from selected structures or a capture volume.
  - **Placement Tool** projects ghosts in world; consumes **power** during projection & commit.
  - Source items from **connected chests** (adjacent or linked via ports set to “construction supply”).

- **MV: Robotic Arm Builder**
  - Place **Robotic Builder Arm** near blueprint; arm pulls items from **assigned chests/ports** and places blocks/machines.
  - Faster build rate; can apply **settings** (I/O filters, recipes, priorities).

- **HV+: Builder Bots (Flying)**
  - **Builder Bay** spawns **Bots**; bots fetch from **Supply Chests/Ports** and construct ghosts.
  - Multi‑bot concurrency; pathing avoids collisions; obeys **power budget** and **range**.
  - Auto‑apply **saved settings**; can **deconstruct** to recover materials.

## Creation Flow
1. **Capture:** select existing entities/blocks (AABB or brush).  
2. **Scrub & Annotate:** choose which connections/settings to include (recipes, filters, port names).  
3. **Costing:** show item/part bill & **power estimate** for placement/build.  
4. **Save:** stores as a data asset (team‑scoped by default; can be personal).

**[OPEN]** Blueprint size limit at each tier (e.g., LV ≤ 20×20×20, MV ≤ 50³, HV ≤ 100³)?

## Placement & Build Flow
- **Ghosts:** preview shows collisions, terrain cut/fill required, power requirements, and supply status.
- **Commit:** on confirm, create a **Construction Job**:
  - **Mode A (Instant Ghosting):** place all ghosts, then agents build over time.  
  - **Mode B (Staged):** place slices as materials arrive.
- **Materials:** pulled from **connected chests** (adjacent or via designated **Construction Ports**). Later: allow **station lanes**/trains as sources.
- **Power:** placement consumes power (per ghost & per second). Build agents also draw power. **Higher tiers** reduce energy per part and increase speed.

**[OPEN]** Should placement require **all items upfront** or allow **progressive delivery**?

## Construction Agents
- **Robotic Builder Arm (MV):** radius, throughput (parts/min), power draw; places blocks/machines and applies blueprint settings.
- **Builder Bots (HV+):** per‑bot carry capacity, speed, concurrent jobs; docking behavior; repair vs build modes.
- **Deconstruction:** mark selection → bots/arm remove and return parts to designated chests (loss % by difficulty).

**[OPEN]** Bot count per bay and energy per flight minute by tier?

## Settings Replication
- Blueprints carry **recipes, filters, priorities, station names, port links (logical names)**. On placement:
  - Match **logical port names** to nearest compatible physical ports; warn on mismatch.
  - If a station is part of blueprint, re‑run **Dock Profiler** on first dock to cache a new profile.

## Networking & Permissions
- Server‑authoritative validation of footprint, collision, and costs; anti‑dupe via **atomic commit** of items → ghosts.
- **Team permissions:** owner/team‑shared visibility; per‑blueprint **read/place** flags.
- **Rate limiting** to avoid griefing; build quotas per shard if needed.

## UI
- **Blueprint Browser**: tags, search, thumbnails; team vs personal.  
- **Placement HUD**: power draw, items available/missing, ETA, collisions.  
- **Agent Panel**: arm/bay status, queue, throughput, priority.  
- **Keybinds**: rotate, mirror, snap toggle, plan layers.

## Integration Points
- **Factories & Logistics:** construction is **WI** (requires area load). Materials can flow via **Ports**; consider a **Construction Port** type.
- **Power & Voltage:** per‑tier energy costs & speeds. Over/undervolt feedback as usual.
- **Background Ticking:** Construction Jobs exist only in **live zones**; **no headless construction** (by design).
- **Server Architecture:** permissions/rate limiting; snapshot construction queues for saves.

## Difficulty Hooks
- **Easy:** lower power cost, faster agents, higher material refund on deconstruct.  
- **Ultra:** higher costs, stricter collisions, lower refunds.

## Acceptance (MVP)
- Create blueprint, place ghosts, robotic arm builds from chest supply, settings applied, all server‑authoritative, no dupes.
- Visual feedback on power/material shortfalls and collisions.

**TODO:** Define JSON schema (blueprint asset), including entity IDs, local transforms, settings payloads, port logical names.  
**[OPEN]** Cross‑save sharing of blueprints (import/export)?
