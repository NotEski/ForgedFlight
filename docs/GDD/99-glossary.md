# 99 – Glossary
 
 **Mechanical Tier:** Rating for shafts/gears/bands, defines max torque and RPM before failure.
 **Gauge (Mechanical/Electrical):** Physical size/rating of cable or shaft, determines max safe throughput.
 **Stall (Mechanical):** Failure mode when torque demand exceeds rating; system stops moving.
 **Shear (Mechanical):** Failure mode when torque exceeds material strength; shaft/gear breaks.
 **Slip (Mechanical):** Failure mode when load exceeds friction; gear/shaft loses sync.
- **Dock Profile:** Cached mapping of station **lanes** to train **manifolds** with effective throughput and dwell times.  
- **Headless IR:** Intermediate representation of a factory using proxies and capacity/latency edges.  
- **LOD:** Level of Detail: Live, Headless, Frozen.  
- **EU / PU:** Abstract energy unit (EU) and power rate (PU/s).
- **Blueprint:** Saved plan containing entities, transforms, and settings; can be placed as ghosts and auto-built by agents.
- **Ghost:** Non-functional preview of an entity/block awaiting construction.
- **Builder Arm/Bots:** Construction agents that place from supply chests/ports; bots are flying late-tier workers.
- **Construction Port:** Port designated as an IO source/sink for blueprint construction materials.
- **Research Artifact:** Book/Chip/Core item encoding a research node; consuming unlocks for one player.
- **Lab Link Cable:** Data link connecting labs into a network to pool throughput.
- **Lab Network:** Connected labs sharing a queue and throughput for research production.
- 
- **Faction:** Player group (formerly "team"); owns claims, flags, and permissions. Name/flag configurable.
- **Claim Anchor:** Placed structure defining a protected territory radius; anchors can be linked to form larger claimed areas.
- **War Declaration:** Mutual or unilateral state allowing claim damage between factions under server rules (raid windows, both-online).
- **Raid Window:** Scheduled period when claim damage is allowed; outside it, structure damage is suppressed.
- **Hostile Fauna:** PvE adversaries (biter-like). Spawn/raid based on pollution/heat/noise and server difficulty.

- **Pixel Art Flag Designer:** In-game editor to create faction flags/banners as pixel art (grid-based), producing banners and decals without external uploads.
- **Game Mode:** World-wide combat configuration: **None (decorative only)**, **PvE (fauna raids)**, **PvP (team warfare only)**, **PvPvE (both).**


- **Small Planet:** Playable solid world of **25–75 km** diameter. Compact travel and limited biome spread. *(Tentative)*
- **Standard Planet:** Baseline playable solid world of **100–150 km** diameter. *(Tentative)*
- **Mega Planet:** Large solid world of **150–250 km** diameter. *(Tentative)*
- **Mini Gas Giant:** Gas giant of **300–400 km** diameter. *(Tentative)*
- **Standard Gas Giant:** Gas giant of **400–500 km** diameter. *(Tentative)*
- **Large Gas Giant:** Gas giant of **500–600 km** diameter. *(Tentative)*

- **On-Rails (Orbit):** Analytic two-body propagation defined by Keplerian elements; no continuous physics tick.  
- **Osculating Elements:** `{a, e, i, Ω, ω, M0/ν0}` describing the instantaneous Keplerian orbit at an epoch.  
- **Patched Conics:** Approximation that switches the reference body at Sphere-of-Influence boundaries.  
- **SoI (Sphere of Influence):** Region where one body’s gravity dominates for trajectory calculation.  
- **Epoch (t0):** Time at which the stored elements are valid; future positions are a pure function of `(t - t0)`.  
- **LIVE vs RAILS:** LIVE = rigid-body physics under thrust/proximity/entry; RAILS = coasting analytic arc.  
- **PCI (Planet-Centric Inertial):** Local inertial frame used near a planet for flight/physics.
