/docs/GDD/04-systems/identity-and-factions.md
# 04 – Systems / Identity, Factions & Flags

> Player groups are called **[DECISION] Factions** (alt: Settlements, Outfits). Each player starts with a **personal faction** (hidden) and can create/join named factions with **flags/banners** and **claims**.

## Faction Basics
- **Names:** player-defined; profanity filter **[OPEN]**.  
- **Membership:** owner, admins, members; roles map to build/research/claim permissions.  
- **Research:** as specified — **personal research** with **shareable artifacts**; joining/leaving preserves personal progress.

## Flags & Banners
Two authoring paths; both are **in-game** (no uploads):

### 1) Pattern Designer (Minecraft-inspired, expanded)
- **Layerable patterns** (stripes, saltires, chevrons, canton, circle, lozenge, cross, gradient bands, etc.).  
- **Abstract shapes** (triangles, polygons, gears, stars) that can be **moved/scaled/rotated** per layer.  
- **Large palette** and **color-blind-safe defaults**.  
- **Ordering & blending** controls (normal, multiply, overlay) **[OPEN]**.

### 2) Pixel Art Designer
- Grid canvas (e.g., **96×64 px**), layers, palette lock, symmetry/mirroring, stamps, dithering.  
- Tools: pencil, line/rect/circle, fill, dither, select/move, mirror.  
- Library: reusable patterns/emblems; save/load faction presets.

### Separate Assets (important)
- **Flags** (waving fabric) and **Banners** (hanging/tall) are **distinct**:  
  - You **must design both** for best results; conversions are **not automatic**.  
  - Each variant stores its own composition (pattern stacks or pixel layers).  
- Outputs become **banners** (world deco), **decals** (vehicles/equipment), and **UI emblems**; server bakes atlases.

## Claims
- **Claim Anchor** multiblock: center + radius; materials + optional power upkeep.  
- **Linking:** anchors can form larger polygons; holes excluded.  
- **Permissions:** gated by role; temporary guest passes per player.  
- **War Interaction:** claim damage requires **war declared** and **both factions online** (defaults; toggleable).

## Governance (Optional, Later)
- **Bylaws:** tax rate for shared depots, etc.  
- **Elections/Rotation:** choose owner/admins; optional war-vote.  
- **Diplomacy:** alliances, truces, war declarations; shared chat channels.

**TODO:** Roles/permissions matrix; claim cost/upkeep; pattern set & palette sizes.  
**[OPEN]** Final canvas resolution(s); blending modes allowed; default color palette.
