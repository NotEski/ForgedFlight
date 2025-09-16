# 05 – Technical / Server Architecture

## Model
- **Authoritative server** with entity replication, prediction/reconciliation, and chunk streaming.  
- **Invisible factory controllers** run on server (background scheduler) and integrate with power/logistics.  
- **Hybrid team research**: personal hidden team by default; expose on team creation; Make Private reversion.

## Voice
- **Proximity chat** optional; radios/suit comms for long range.

## Security & Exposure
- Basic anti‑cheat validation (crafting/power/logistics invariants).  
- Ownership/permissions for zones/teams/ports.
- Faction permissions; per-bay locks; anti-spam rate limits.

**[OPEN]** Protocols/ports; matchmaking/NAT traversal; dedicated vs embedded server split.

## Inventory & Orders
- **Physical-only inventory:** no virtual/subspace. Aggregated views require **scanners**; the server indexes containers → shelves → zones.  
- **Orders** create **jobs** routed via logistics; state changes are **atomic** at depots/ports; items traverse real paths.  
- **Auditing:** per-shelf history to combat desync/dupes; permissions on bays.

## Blueprints & Permissions
- Server-authoritative **costing & collision** checks; atomic item→ghost commits.
- **Team permissions** on blueprint assets (read/place); optional shard-level rate limits to prevent griefing.

## Research Artifacts
- **Minting:** server signs each artifact (mint_id + node_id) to prevent dupe/forgery.
- **Consumption:** signature verified; if node already known, convert to scrap/refund per difficulty.
- **Permissions:** lab UI respects owner/team; output can target chest or a specific player.

## World Settings & Governance
- **World Settings:** `GameMode` (**None / PvE / PvP / PvPvE**, **default: PvPvE**), `StructureDamage`, `Explosives`, `HostileFauna`, `LandClaims`, `RequireWarForClaimDamage`, `RequireBothOnlineForClaimDamage`, `RaidWindows`.  
- **Claims:** server tracks **Claim Anchors** (owner faction, radius, polygon links). Build/damage permissions are evaluated against claims and world settings.  
- **War Declarations:** signed records (attacker, defender, start, expiry). Server enforces **raid windows** and **both-online** requirement when enabled.  
- **Safe Zones:** configurable areas with reduced damage; spawn and markets by default.
- **Flags:** **Pattern Designer** and **Pixel Art Designer** are in-game; server bakes atlases (no external uploads).

**[OPEN]** Policy preset bundles per mode (e.g., PvE = fauna on, PvP off; PvP = fauna off; PvPvE = both on).d

## Orbital Subsystem (Rails)
- **Bodies on rails:** ephemerides seeded per world; server computes positions as a pure function of time.  
- **Craft state:** either **LIVE** (rigid-body) or **RAILS** (`body_id, epoch, elements`).  
- **Events:** SoI boundary, atmo entry, impact, peri/apo, scheduled **burn nodes**.  
- **Authority flow:** client proposes burn (Δv/time); server simulates, clamps, and **commits** → new on-rails elements.  
- **Replication:** RAILS packets are tiny (elements + epoch). LIVE packets include pose/vel/angvel at reduced rates.  
- **Cheat resistance:** burns validated against fuel/mass; delta-time clamped; event ordering deterministic.

## Inventory & Orders
- Physical-only inventory; scanners for aggregation; orders → jobs → ports; audit trails.