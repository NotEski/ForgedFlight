# 04 – Systems / Warfare & Defense  [PRIORITY: Low]

> **Scope:** Optional, **server-toggleable** warfare and PvE defense. Designed to be co-op friendly; PvP is opt-in via **war declarations** and **raid windows**. This doc is a **stub** until core systems are implemented.

## Design Goals
- **Engineering-first:** weapons are machines (contraptions + multiblocks) tied into power, logistics, and sensors.
- **Consent & Safety:** worlds can disable PvP/structure damage entirely. Claimed land is safe unless a **war** is declared and both sides are online (default).
- **PvE Option:** “biter-like” **hostile fauna** raids (name TBD) provide pressure for defense in PvE servers.

## World Settings (Toggles)
(Use Game Mode for coarse behavior; toggles below allow advanced overrides.)
- `PvP`: **Off / On** (default: **Off**).  
- `StructureDamage`: **none / limited / full** (default: **limited**; claims protected unless at war).  
- `Explosives`: **off / mining-only / defense-only / all** (default: **mining-only**).  
- `HostileFauna`: **off / low / normal / high** (default: **low**).  
- `LandClaims`: **enabled / disabled** (default: **enabled**).  
- `RequireWarForClaimDamage`: **true/false** (default: **true**).  
- `RequireBothOnlineForClaimDamage`: **true/false** (default: **true**).  
- `RaidWindows`: **off / scheduled** with server TZ & schedule blocks (default: **scheduled**).

## Game Modes (World Setting)
- **None (Decorative):** Warfare systems can be built/tested (turrets, ammo chains) but **deal no damage** to structures/players/NPCs. Use for engineering displays and range tests.  
- **PvE:** Factorio-like pressure from **Hostile Fauna**. No player-vs-player damage; structure damage only from fauna (per rules).  
- **PvP:** Team-vs-team warfare; **no fauna** spawns. Claims protect until **War Declared** and **Both Online** if enabled.  
- **PvPvE:** Teams can fight each other **and** defend against fauna raids.

**[DECISION]** Default = **PvPvE** on survival servers; creators can switch to **None** for peaceful worlds.

### Advanced Overrides (Optional)
Servers may override mode behaviors with knobs below (intended for fine-tuning, not replacing the mode):
- `StructureDamage`: **none / limited / full** (defaults derived from mode).  
- `Explosives`: **off / mining-only / defense-only / all** (defaults derived from mode).  
- `HostileFauna`: **off / low / normal / high** (ignored in PvP; forced off in None).  
- `RequireWarForClaimDamage`: **true/false** (PvP & PvPvE default **true**).  
- `RequireBothOnlineForClaimDamage`: **true/false** (PvP & PvPvE default **true**).  
- `RaidWindows`: **off / scheduled** (only relevant if fauna or wars can damage claims).

## Factions & Claims
- **[DECISION]** Rename “teams” to **Factions** (player-named); alt options: **Settlements**, **Outfits**.  
- **Claim Anchors:** a placed structure that defines a **claim radius**; anchors can be linked to form larger areas.  
- **Rules:** only the owning **Faction** can modify inside claims; outsiders need **build permission** or an active **war**.  
- **Upkeep:** optional upkeep (materials/power) to prevent spam. **[OPEN]** exact costs and decay rules.  
- **Flags & Identity:** Faction **name**, **flag**, and **banner**. See Identity & Flags doc.

## PvE: Hostile Fauna (Biter-like)  [Stub]
- **Triggers:** pollution/heat/noise near player factories; density scales with **HostileFauna** setting.  
- **Behavior:** raids target exposed infrastructure; avoid claims with strong defenses; retreat on heavy losses.  
- **Drops/Rewards:** organic matter, rare glands/catalysts **[OPEN]**.  
- **Tuning:** server multipliers; per-biome species table **[OPEN]**.

## Weapons & Defense (Stubs)
- **Black Powder → Field Guns → Mechanized Turrets** progression (see Tech Trees).  
- **Ammunition chain** via Metallurgy + Chemistry (propellant/explosive/fuzes).  
- **Defenses:** walls, armor plates, blast berms, sandbags; **sensors** (radar/optics) for targeting.  
- **Damage Model:** simplified **PEN vs AR**, blast overpressure; friendly-fire always on.

## UI & Moderation
- **World Settings Panel:** live view + admin controls for above toggles.  
- **Claim Overlay:** shows claim rings, ownership, safe zones, war status.  
- **Combat Logs:** redacted summary for disputes (who fired, where, with what).  
- **Anti-grief:** structure damage multipliers, explosive whitelists, cooling-off timers.

**TODO:** Balance tables (PEN/AR), fauna taxonomy, claim anchor costs, war/raid UX, logs.  
**[OPEN]** Default PvP policy presets per world; exact fauna naming (avoid “Biters” term).
