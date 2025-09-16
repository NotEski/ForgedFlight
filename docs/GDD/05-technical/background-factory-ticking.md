# 05 – Technical / Background Factory Ticking

## Island/Zone Model
- Auto‑created **Factory Controllers** claim zones; compile **headless IR** (proxies + capacity/latency edges).  
- LOD: Live (loaded), Headless (background), Frozen (paused). Swaps use **barrier ticks** and **snapshots**.

## Execution
- Background **job scheduler** with per‑controller CPU budgets; adaptive timestep (e.g., 0.2–5 s).  
- Power budgets provided per tier by Energy Solver; storage contributes in/out caps.

## Reconciliation
- Deterministic **catch‑up** on resume/LOD change (time‑sliced).  
- Ports are authoritative; no double‑production; GC dead controllers.

## Testing Hooks
- Sim step override; snapshot export/import; headless vs live throughput diff tooling.

**[OPEN]** Recipe batching policy; max catch‑up window; default CPU budget per controller.

## Lab Queues
- Labs are **Contained Processing** and tick in **headless** mode when unloaded.
- Each **Lab Network** maintains a queue of node jobs; throughput scales with network size & modules.
- **Outputs** are **Research Artifacts** delivered to an output slot/port with optional player tag.

## Construction Jobs (Blueprints)
- **Always WI:** construction tasks require an **Activation Zone**; no headless construction to avoid terrain/entity consistency issues.
- **Queue & Snapshot:** construction queues are saved with controller/zone snapshot; paused when zone is headless/frozen.
- **Power Budget:** job execution draws from zone power; throttled when budget is exceeded.

## Scheduling
- Priority: players > WI islands near players > remote processing islands.
- Catch-up: deterministic batch steps; ports authoritative to avoid dupes.

## Spacecraft & Rails
- **Coasting craft** require **no headless tick**; they’re represented by **rails state + pending events**.  
- The **Orbital Event Queue** wakes the craft into LIVE mode for: atmospheric entry, SoI changes, impact windows, or scheduled burns.  
- After the LIVE window, craft **return to rails** with updated elements.

**[OPEN]** Max number of concurrent LIVE craft; per-tick event budget; rollback strategy on desync.