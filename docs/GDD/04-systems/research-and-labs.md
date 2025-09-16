# 04 – Systems / Research & Labs

> **Design:** Research is **per-player and private**. You share knowledge by crafting **research artifacts**—early as **Books**, later as **Chips**, late-game as **Cores**. Labs consume inputs + power to **produce these artifacts**; using one unlocks that node **for the consuming player**.

## Goals
- Individual ownership of knowledge; **opt-in sharing** via tradeable items.
- Keep early friction low (inventory/Research Table), push heavier topics into **Labs**.
- Support **multiple labs**: link them with a cable to pool throughput; scale labs with **expansion modules**.

## Artifact Types (by tier)
- **Books (paper)** — Copper/Bronze/Iron; crafted at **Research Table** or Metallurgy/Energy Labs.
- **Chips (data)** — Electricity/Trains/Oil/Computers; crafted at **Electronics/Chemistry** Labs.
- **Cores (exotic/quantum)** — Nuclear/Space/Fusion/Off-planet; crafted at **Energy/Rocket/Astronomy** Labs.

**[OPEN]** Final names/looks; whether an artifact can encode **multiple nodes** vs **single node** only.

## Unlock Flow
Each node defines:
- **Inputs** (items/fluids), **Time** (base), **Lab types** allowed, **Output** = an artifact bound to **NodeID**.
- Player **uses** the artifact → unlocks the node on **their** tree (duplicate use → salvage/refund).
- Artifacts are **server-minted** items (tradeable, storable, giftable).

## Labs & Networks
- **Lab blocks** (Metallurgy, Energy, Transportation, Electronics, Chemistry, Rocket, Astronomy). Labs are CP (headless-eligible).
- **Lab Link Cable** chains labs into a **Lab Network** that pools throughput.  
  - Speed = Σ(member speeds) × diminishing returns (e.g., +100% first, +75% second, +50% third…). **[OPEN]** Final curve.
- **Expansion modules** add speed/efficiency (slot-capped per tier). **[OPEN]** Slot counts & effects.
- **Output**: artifacts to designated **output port/chest**, or **auto-assign** to a player tag.

## Early vs Mid/Late
- **Early (inventory)**: simple nodes via **inventory UI** or **Research Table**, producing a **Book** you can self-use or gift.
- **Mid/Late (labs)**: most nodes require **Labs**; networks speed production.

## Power & Costs
- Draw **tiered power** (see Power & Voltage).  
- Inputs may include **data samples** (satellites/sensors) for Astronomy.  
- Scales with **difficulty**.

## UI
- **Player Research**: owned/available nodes; artifact inventory; **Use** button.  
- **Lab UI**: pick node → see cost/time/queue/throughput; link status; output target (chest/player).  
- **Team view**: networks you can use, their queues, permissions.

## Networking & Permissions
- Artifacts: **single-use**, **signed** (mint_id + node_id).  
- Lab UIs honor owner/team permissions.  
- Anti-dup: deny duplicate consumption; convert to **scrap** (refund varies by difficulty).

## Integration
- **Factories/Logistics**: Labs are **CP**; inputs via belts/pipes; outputs to ports/chests; headless-eligible.  
- **Server**: artifact minting is **atomic**; permissions enforced.  
- **Background Ticking**: lab queues snapshot with zone; deterministic resume.

## Difficulty Hooks
- **Easy:** cheaper/faster; high network efficiency; duplicates refund 100%.  
- **Normal:** baseline.  
- **Hard/Ultra:** pricier/slower; lower efficiency; duplicate refunds reduced.

## Acceptance (MVP)
- Produce a **Book**/**Chip** for a node; use to unlock; trade to another player to unlock theirs.
- Link multiple labs with **Lab Link Cable** to see faster throughput in UI.

**TODO:** JSON schema for Research Artifact (node_id, tier, mint_id, signature, optional owner_tag).  
**[OPEN]** Reuse **power cable** vs distinct **Lab Link Cable**?  
**[OPEN]** Max queue length; whether a network can queue **multiple node types** concurrently.