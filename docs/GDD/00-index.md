# Forged Flight: From Anvil to Orbit — Game Design Document

This is the **source-of-truth GDD** compiled from this chat. It is **living**: whenever decisions change here, this index and the changelog are updated. Use relative links.

- Project: **Forged Flight: From Anvil to Orbit**  
- Status: **Working Draft**  
- Version: **v0.3.12**  
- Updated: **2025-09-10**

## Table of Contents
- [01 – Vision](./01-vision.md)
- [02 – Core Pillars](./02-core-pillars.md)
- [03 – Gameplay Loops](./03-gameplay-loops.md)
- 04 – Systems
  - [Power & Energy (Mechanical/Electrical)](./04-systems/power-voltage.md)
  - [Factories & Logistics](./04-systems/factories-logistics.md)
  - [World Sim & Loading](./04-systems/world-sim-and-loading.md)
  - [Gas Giant Planets](./04-systems/planet-gas-giants.md)
  - [Vehicles & Physics](./04-systems/vehicles-and-physics.md)
  - [Tech Trees](./04-systems/tech-trees.md)
  - [Blueprints & Construction Assist](./04-systems/blueprints-and-construction.md)
  - [Storage & Inventories](./04-systems/storage-and-inventories.md)
  - [Warfare & Defense](./04-systems/warfare-and-defense.md)
  - [Identity, Factions & Flags](./04-systems/identity-and-factions.md)
- 05 – Technical
  - [Rendering (bgfx)](./05-technical/rendering-bgfx.md)
  - [UI (ImGui)](./05-technical/ui-imgui.md)
  - [Content Pipeline & Tools](./05-technical/content-pipeline.md)
  - [Server Architecture](./05-technical/server-architecture.md)
  - [Background Factory Ticking](./05-technical/background-factory-ticking.md)
- [99 – Glossary](./99-glossary.md)

## Changelog
- **2025-09-10 — v0.3.12**: Documentation consistency update — converted remaining torus world references to cube-sphere worlds in Vision (01), Core Pillars (02), and README to align with v0.2.0 architectural decision. Updated repo README with proper build instructions and GDD links.
- **2025-09-05 — v0.3.11**: Documentation Parity, Added missing docs: Storage & Inventories; Content Pipeline & Tools. Background Ticking, UI, Logistics, Glossary. Documented Procedural Guardrails in World Sim, Gas Giants, Rendering. Corrected index version/date and added links.
- **2025-09-08 — v0.3.9**: Added dedicated **World Generation** spec; cross-linked Sim/Rendering. Included low-res **Worldmap Bake** pipeline hook.
- **2025-09-07 — v0.3.8**: Adopted **double precision** and a **Hybrid Rails** space model: LIVE under thrust, **on-rails** while coasting; planets/moons also **on rails** from seeded ephemerides. Added orbital scheduler/events, SoI handoffs, and save/replication rules. Updated Vehicles & Physics, World Sim, Rendering, Server Architecture, Background Ticking, Glossary.
- **2025-09-07 — v0.3.7**: Removed “set piece” mentions; added **procedural guardrails** (apparent-size cap, orbital spacing, exposure clamp). Updated World Sim, Gas Giants, and Rendering.  
- **2025-09-07 — v0.3.6**: **[TENTATIVE]** Planet size classes set — **Small 25–75 km**, **Standard 100–150 km**, **Mega 150–250 km**; Gas giants **Mini 300–400 km**, **Standard 400–500 km**, **Large 500–600 km**. Updated World Sim, Gas Giants, Rendering, and Glossary.
- **2025-09-05 — v0.3.5**: Default **GameMode = PvPvE**. Expanded Flags/Banners with **Pattern Designer** + **Pixel Art Designer**; require **separate flag & banner** assets. Updated Warfare/Identity/Server/UI.
- **2025-09-05 — v0.3.4**: Replaced flag PNG uploads with an in-game **Pixel Art Flag Designer**. Added **Game Modes** (None/PvE/PvP/PvPvE) influencing warfare/fauna. Updated Server/UI/Glossary.
- **2025-09-05 — v0.3.3**: Warfare set to **Low Priority** with stubs; added **World Settings** toggles, **Factions & Claims**, **Flag Builder**; PvE **hostile fauna** option. Server/Tech/UI/Glossary updated.
- **2025-09-05 — v0.3.2**: Adopted **constant conversion** for generators/alternators with **class caps (P_max)** and **cut-in RPM**; updated Power & Energy, UI, Tech Trees, and Glossary.
- **2025-09-05 — v0.3.1**: Replaced electrical tiers with **real units** (V/A/Ω/W) and formalized **mechanical τ/RPM** model. Added generator/motor equations and UI tools; updated Tech Trees and Glossary.
- **2025-09-05 — v0.3.0**: Added **Storage & Inventories** (clay vessels → crates/barrels → silos/tanks → warehouses). **Ghost planning** from Day 1; **auto-builders** post-Computers. Warehousing/Orders + physical-only inventory threaded through docs.
- **2025-09-05 — v0.2.9**: Integrated **Contraptions** into **Power (rotational MR)**, **Logistics**, **Vehicles**, and **Tech Trees**.
- **2025-09-05 — v0.2.8**: Added **Contraptions** concept (freeform moving assemblies) distinct from multiblocks.
- **2025-09-05 — v0.2.7**: Gas giants clarified as **collection-only** worlds. Added Canisters, Rocket Logistics Port, and off-world processing flow.
- **2025-09-05 — v0.2.6**: Added **Gas Giant Planets** system: layered, semi-volumetric clouds; floating platforms; airship/plane focus.
- **2025-09-05 — v0.2.5**: Adopted **Mantlerock** as unmineable bottom layer (replacing bedrock).
- **2025-09-05 — v0.2.4**: Set **chunk size** 32×32×32, **region** 16×16×16 chunks (~512 m), **build height** 2048 m.
- **2025-09-05 — v0.2.3**: Locked **world scale** to **1 unit = 1 meter** and **voxel size = 1 m**.
- **2025-09-05 — v0.2.2**: Added **faceted/no-curves** art style; references (*Cobblemon*, *Create*, *Immersive Engineering*).
- **2025-09-05 — v0.2.1**: Added **Content Pipeline & Tools** (Blockbench → glTF).
- **2025-09-05 — v0.2.0**: Switched to **cubesphere planets**; set diameter scale (5–100 km); updated World Sim/Rendering/Glossary.
- **2025-09-04 — v0.1.5**: Switched to **per-player research** with **shareable artifacts** (Books/Chips/Cores). Added **Research & Labs** doc and threaded model into Tech Trees, Server Architecture, Background Ticking, UI, Logistics, and Glossary.
- **2025-09-04 — v0.1.4**: Added **Blueprints & Construction** system (LV blueprint table + placement tool, MV robotic builder arm, HV builder bots) and threaded into Tech Trees, Logistics, Background Ticking, UI, Server Architecture, and Glossary.
- **2025-09-04 — v0.1.3**: Added **Tech Trees** document and threaded milestone gates across Metallurgy, Energy, Transportation, Electronics, Chemistry, Agriculture, Astronomy, Rocketry, and Military.
- **2025-09-04 — v0.1.2**: Updated **Milestone Cadence** (Copper → Bronze → Iron → Steel → Steam → Electricity → Trains → Oil → Cars/Trucks → Airships → Planes → Computers → Nuclear (optional) → Space age → Fusion → Off-planet research → Gate construction).
- **2025-09-04 — v0.1.1**: Set **session length** target to **60–120 min** per milestone (longer-burn intent) and **pace to first orbit** to **25+ hours (Normal)**; initial milestone ladder created.
- **2025-09-04 — v0.1.0**: Initial scaffold compiled from chat: pillars, loops, torus↔sphere, seasons/day-night, difficulty, crafting (interim), tech trees, GregTech-inspired power tiers, invisible factory controllers + headless zones, trains with lane/manifold dock profiling, UI/controls, and multiplayer research model.

## Protocol
- **[DECISION]** mark final decisions.  
- **[OPEN]** ask specific questions where details are missing.  
- **TODO:** concrete follow-ups.  
- Use H1 per file, scannable bullets, and keep numbers traceable to chat.