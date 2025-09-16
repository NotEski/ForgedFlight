# 04 – Systems / Tech Trees

> The **Main Tech Tree** tracks progress across all domain trees and gates milestone progression. Research is **accessed from inventory** by default; early tiers are **player-driven** (no lab required). Mid/late tiers require **specialized Labs**. Team research uses the **hybrid model** (personal hidden team by default; exposable team with shared progress; Make Private if owner-only).

## Research Access & Labs
- **Per-player trees**; knowledge is private by default.
- **Sharing via artifacts:** craft **Books/Chips/Cores** that encode a node; using one unlocks it for the consumer.
- **Early game:** inventory/Research Table produces **Books** for simple nodes.
- **Labs:** mid/late nodes require Labs that **output artifacts**; link labs with **Lab Link Cable**; expansion modules speed research.
- Research UI is always available from inventory; labs exist to **produce artifacts** faster and unlock gated nodes.

**TODO:** Finalize lab build costs/slots and link efficiency curve.  
**[OPEN]** Which nodes remain inventory-only vs lab-only?

---

## Metallurgy Tree
**Tiers:** Copper → Bronze → Iron → Steel → (Glass making integrated here).  
**Notes:** Hazards (lead, arsenic) present; fragile materials only affect **tools/vehicles**, not factory blocks.

**Gates & Unlocks**
- **Copper Age** → basic metal tools; **blacksmithing start**; simple furnace & kiln.
- **Bronze Age** → **first automation** pieces: water wheel, wooden/bronze shafts & gears; ore processing; anvil upgrades.
- **Iron Age** → stronger tools; mechanical transmission upgrades; early rails for minecarts.
- **Steel Age** → high‑strength frames; rails & structural parts for trains/vehicles; boiler/pressure‑rated parts.
- **Glass making** → integrated around Iron/Steel for gauges, bulbs, windows.

**Dependencies**
- Feeds **Energy** (steam pressure parts), **Transportation** (rails/frames), **Electronics** (wires, bulbs), **Rocketry** (tanks, engines).

**[OPEN]** Lock the **bronze alloy set** (tin/arsenic/bismuth variants) and their hazard flags.  

---

## Energy & Power Tree
**Tiers:** Hand/Animal → **Water Wheel (mechanical)** → **Steam (mechanical)** → **Electricity (Pulling electricity from mechanical)** → **Nuclear** → **Fusion**.
See [Power & Voltage](./power-voltage.md) for rules.

**Gates & Unlocks**
- **Mechanical:** water wheels, treadmills, steam engines, governors, flywheels.  
- **Electrical:** **Alternator Classes** (Mk.I/Mk.II/Mk.III) using constant conversion `Pₑ = η·Pₘ` capped by **P_max**, bus regulators (12/24/48 V), batteries, meters.  
- **Motors:** DC motors (specs in V/A/Ω, τ/RPM), drives/inverters later.

**Dependencies**
- **Steam** requires **Steel** (pressure). **Electricity** benefits from **Chemistry** for insulation/oils. **Nuclear/Fusion** require **Electronics** & **Chemistry**.

**[OPEN]** Confirm tier order **Steam → Electricity → Trains → Oil** as per milestones (Trains may still *depend* on Steam+Steel).
**[OPEN]** Set `P_max`, `η`, and cut-in RPM per alternator class; decide default bus preset (24 V vs 48 V).
---

## Transportation Tree
**Tiers:** Horse riding/taming → Carts → Canoes/Kayaks → **Trains** → **Cars & Trucks** → **Airships** → **Planes**.  
*(Rockets are handled in Rocketry.)*

**Gates & Unlocks**
- **Animal Carts/Minecarts/Boats** (pre‑steam): local haulage; teaches routes.
- **Trains** (bulk logistics): **Station Lanes + Dock Profiling**; headless virtual runs when unloaded.
- **Cars & Trucks**: physics vehicles; roads; depots.
- **Airships**: buoyant lift; slow but terrain‑agnostic.
- **Planes**: fast logistics; runways & fuel grades.

**Dependencies**
- **Trains** require **Steam** + **Steel**.
- **Cars/Trucks/Airships/Planes** require **Oil** (fuels) + **Electricity** (control systems).

**[OPEN]** Fuel grades and maintenance (wear) per difficulty.

---

## Electronics Tree
**Tiers:** Resistors/Diodes → Simple Logic → Microcontrollers → **Computers**.  
Provides control, automation, sensors, and GUIs.

**Gates & Unlocks**
- **Sensors & Encoders:** rpm, torque, position; limit switches.  
- **Servo Drivers/Motor Controllers:** tie **DC** control into **mechanical** mechanics.  
- **Automation:** PID governors, programmable sequence blocks.
- **Scanners & Terminals** (warehouse indexing, network stock).  
- **AS/RS Stacker Crane** (post-computers).  
- **Robotic Builder Arm** and **Builder Bots** **(post-computers)**.

**Blueprint support:** Planning Ghosts always available; agents unlock post-computers.

**Dependencies**
- Needs **Metallurgy** (wires), **Chemistry** (plastics), **Energy** (stable grid).  
- Enables **Rocketry/Astronomy** UIs and **Off‑planet research** consoles.

**[OPEN]** Decide scripting model (visual blocks vs Lua‑like).

---

## Chemistry & Materials Tree
**Tiers:** **Oil → Plastics → Composites/Alloys → Propellants**.

**Gates & Unlocks**
- **Oil** → pumpjacks/fracking; basic distillation; lubricants.
- **Plastics** → casings, insulation, optics.
- **Composites** → lightweight frames/skins (feeds Vehicles, Rockets).
- **Propellants** → fuels/oxidizers; later exotic fuels for Fusion/Rockets.
- - **Gas Canisters & Sorbents** → standardized containers and sorbent media for gas giants; unlock **compressors/filters** (limited CP).

**Dependencies**
- Feeds **Energy** (turbines, insulation), **Electronics** (polymers), **Transportation** (fuels), **Rocketry** (propellants).

**[OPEN]** Environmental rules (spills, toxicity) per difficulty.

---

## Agriculture, Horticulture & Husbandry
**Tiers:** Foraging → Domestication → Irrigation → Greenhouses → Automation.

**Gates & Unlocks**
- Food security to minimize survival tax; animal power (pre‑mechanical).
- Weather/season hooks (planet‑wide seasons; later split hemispheres optional).

**Dependencies**
- Supports **Survival loop** and early **Energy** (animal power).

**[OPEN]** Crop list & seasonal effects (kept light).

---

## Astronomy & Space Research
**Tiers:** Telescopes → Satellites → Planetary Survey → **Off‑planet Research** (multi‑tier sci‑fi) → Gate Theory.

**Gates & Unlocks**
- **Satellites**: LOS‑based map/sensor feeds (space view requires satellite).  
- Planet discovery/orbits; fast **Outer‑Wilds‑style** orbital periods.
- **Off‑planet research** tiers: exotic materials, advanced fuels, alien physics.

**Dependencies**
- Requires **Rocketry** for launch capability; needs **Electronics** for data links.

**[OPEN]** Number of off‑planet tiers and their themes.

---

## Rocketry & Spaceflight
**Tiers:** Solid Boosters → Liquid Fuel → Guidance & Staging → **Orbital Access (Space age)** → Stations/Interplanetary → Gate Construction.

**Gates & Unlocks**
- Build rockets from parts; realistic atmospheres & re‑entry visuals.
- Logistics rockets; orbital stations; automated launches.
- **Rocket Logistics Port (Platform)** for gas giants; **Factory Port** on solid planets/stations for receipt of canisters.
- **Gate Construction**: cross‑tree mega‑project.

**Dependencies**
- Heavy on **Metallurgy**, **Chemistry**, **Energy**, **Electronics**, **Astronomy**.

**[OPEN]** Simplified orbital model parameters and launch window pacing.

---

## Warfare & Defense Tree  [PRIORITY: Low]
**Tiers:** Black Powder → Breech & Field Guns → Mechanized Turrets → High Explosives → Rockets/Missiles *(optional late)*

**Gates & Unlocks**
- **Black Powder:** muzzle-load cannons; solid shot/HE; basic fuzes.  
- **Breech & Field Guns (Steam):** breech mechanisms, recoil slides, shrapnel shells, early sights.  
- **Mechanized Turrets (Electricity):** powered traverse/elevation, stabilizers, rangefinders; HMG/autocannon.  
- **High Explosives (Chemistry):** dynamite, HE/HEI shells, shaped charges; safe storage rules.  
- **Rockets/Missiles (Rocketry/Electronics):** unguided rockets; **[OPEN]** guided.

**Dependencies**
- **Metallurgy:** barrels, armor plates, reinforced frames.  
- **Chemistry:** propellants, explosives, fuzes.  
- **Electronics:** sensors, fire control, stabilizers.  
- **Energy:** motors, alternators for turret drives.  
- **Storage/Logistics:** ammo crates, magazines, depots.

**World Toggles / Consent**
- PvP default: **Off**; `StructureDamage: limited`.  
- Claims protected unless **War Declared** and **RequireBothOnline = true** (defaults).  
- PvE **Hostile Fauna** available as pressure source (see Warfare & Defense).

**[OPEN]** Server default policy presets; exact unlocks per milestone; fauna naming.


---

## Milestone Gates Matrix

| Milestone | Metallurgy | Energy | Transportation | Electronics | Chemistry | Rocketry | Astronomy | Notes |
|---|---|---|---|---|---|---|---|---|
| Copper Age | Copper tools, kiln | — | — | — | — | — | — | Start blacksmithing |
| Bronze Age | Bronze, gears | **Water wheel** | Carts/boats | — | — | — | — | First automation |
| Iron Age | Iron tools | — | Early rails | — | — | — | — | — |
| Steel Age | Steel frames | Boiler parts | Rails ready | — | — | — | — | Enables pressure vessels |
| Steam | — | **Steam** online | **Trains** enabled | — | — | — | — | Bulk logistics |
| Electricity | Conductors | **Generators/VT grid** | Depots | **LV Blueprint Table** | Lubes/insul. | — | — | Grid+machines + Blueprints MVP |
| Trains | Rails | Fuel/steam grid | **Stations + Dock profiling** | Signals | Fuel grades | — | — | Headless lines + MV Robotic Builder Arm |
| Oil | — | Turbines (opt) | Fuel supply | — | **Distillation** | — | — | Fuels/lubricants |
| Cars & trucks | Chassis | Engines | **Vehicles** | ECUs | Fuels | — | — | Physics vehicles |
| Airships | Frames | Engines | **Airships** | Nav | Fuels | — | — | Buoyant lift |
| Planes | Airframes | Engines | **Planes** | Avionics | Fuels | — | — | Fast aerial |
| Computers | — | Stable grid | Schedules | **Computers** | Plastics | — | — | Advanced automation + HV Builder Bots |
| Nuclear (opt) | Alloys | **Nuclear** | — | Controls | Coolants | — | — | Optional branch |
| Space age | Tanks/engines | High power | — | Guidance | Propellants | **Orbit** | **Satellites** | Orbital access |
| Fusion | Alloys | **Fusion** | — | Controls | Exotic fuels | — | — | High‑end power |
| Off‑planet research | Exotic alloys | Space power | — | Comms | Exotic chem | Interplanet. | **Off‑planet tiers** | Sci‑fi progression |
| Gate construction | Mega alloys | Massive power | — | Control cores | Exotic matter | Launch/assembly | Survey | Final objective |

**TODO:** Fill exact per‑milestone quest goals and “done” checks.  
**[OPEN]** Which milestones should hard‑gate tech (must‑have) vs soft‑gate (recommended)?
