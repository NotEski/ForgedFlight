# 04 – Systems / Vehicles & Physics

## Regimes
- **Ground:** wheels/tracks; traction, gearing, suspension; mass & torque matter.  
- **Water:** hull buoyancy by voxel displacement; propellers/paddles; wave drag.  
- **Atmosphere:** lift/drag from flat facets; control surfaces; engines/props/balloons.  
- **Space (Hybrid Rails):** **live** rigid-body under thrust; **on-rails** analytic orbit while coasting.

## Spaceflight – Hybrid “Rails” Model
- **Under Thrust (LIVE):** standard rigid-body integration (double precision state) in a **planet-centric inertial frame** (PCI).  
- **Engine Cutoff → On-Rails:** at last interaction (cutoff/stage/docking), compute **osculating Keplerian elements** relative to the current primary and store:
  - `body_id, epoch_t0, a, e, i, Ω, ω, M0` (or ν0), plus craft mass & cross-section for UI.  
  - While on-rails, the server/client **propagate analytically** (two-body) — no continuous physics tick needed.
- **Patched Conics / SoI:** when a trajectory crosses a Sphere-of-Influence boundary, switch reference body and recompute elements.  
- **Event Queue (server-authoritative):**
  - Periapsis/apoapsis passage, **SoI boundary**, **atmospheric interface**, **predicted ground/sea impact**, **scheduled burns**.  
  - Crossing an event **wakes LIVE physics** (e.g., at atmo entry for drag/re-entry VFX).
- **Atmospheric Drag & Decay (Simplified):**
  - No drag above `h_atmo` of the current body.  
  - If periapsis < `R_body + h_atmo`, schedule **entry event**; drag/heat handled LIVE during the brief entry window.  
  - **[OPEN]** Long-term decay modeling beyond single-pass entries (ignore vs. analytic decay).
- **Collisions on Rails:** if a propagated arc intersects terrain/ocean before next wake, raise **impact event** → resume LIVE just before impact for deterministic contact.
- **Docking & Rendezvous:**
  - While coasting, craft follow rails; switching to **proximity (< X km)** wakes LIVE for both to allow docking physics. **[OPEN]** Proximity radius.
- **Time Warp:** not required; orbits are short (Outer-Wilds style). **[OPEN]** Dev-only warp safety limits.

## Precision & Frames
- **Double precision** for world transforms and state in physics.  
- **Floating origin** rebased around the active craft; render uses high/low-freq split.  
- Frames: **Star-centric** for ephemerides; **Planet-centric inertial (PCI)** for local flight; seamless handoff at SoI.

## Multiplayer & Authority
- Server replicates minimal **rails state** (`body_id + elements + epoch`) and **LIVE state** during burns/entries/docking.  
- Burns are server-authoritative **impulses** (or finite thrust). Clients preview, server confirms → new orbit.  
- Deterministic outcomes; tiny packet sizes while on-rails.

## Tools & UI (Space)
- **Orbit HUD:** body, peri/apo altitudes, period, SoI countdown, entry countdown.  
- **Maneuver Node (later):** place Δv; preview resultant elements; queue as an **event**. **[OPEN]** MVP scope.
- **Safety:** warnings for predicted impact; auto-wake windows before critical events.

**TODO:** Δv planner MVP, proximity wake radius, analytic decay decision, buffer time before event wake.

## Gas Giant Flight
- **Buoyancy:** aerostats compute lift from local density and lifting gas; lift margin shrinks with altitude/temperature.  
- **Drag & Shear:** airspeed adds drag quadratic in speed; wind shear adds forces/torque; turbulence noise band-limited.  
- **Lightning & Icing:** periodic hazard pulses; equipment reduces risk (grounded tethers, de-icers).  
- **Entry/Exit:** high dynamic pressure on entry; **safe corridors** published via satellite survey. **[OPEN]** Heat & damage model numbers.

## Contraptions
- **Definition:** Collections of **modular parts** assembled by players into a functional whole, governed by physics.  
- **Contrast with Multiblocks:**  
  - **Multiblocks**: predefined structures; recipe-locked; GUI; often abstract simulation.  
  - **Contraptions**: open-ended builds; direct physics simulation; no GUI (function arises from mechanics).  
- **Examples:** turret batteries, crane arms, ship control rigs, mobile siege engines.  
- **Dependencies:** parts from **Metallurgy**, **Mechanics**, and **Electronics** trees.  
- **Integration:**  
  - Attach to physics objects (ships, vehicles) as control modules.  
  - Combine with sensors (radar, optics) for targeting and automation.  
  - Subject to durability, torque, and power limits.  
- **[OPEN]** Define common part library (rotators, sliders, clutches, gearboxes, sensors, emitters).

### Energy Backbone (Contraptions)
- **Primary:** **Rotational (MR)** via shafts/gears; vehicles & rigs couple through clutches/gearboxes.  
- **Secondary:** **Electrical (DC bus)** feeds motors/servos/sensors; must couple to MR via **motors** to drive mechanics.  
- **Conversion:** **Generators** (MR→DC) for charging; **Motors** (DC→MR) for actuation.

## Construction
- **Jigs/benches** assemble modules; constraints enforced (torque, heat, structural).  
- **Property warnings**: torque limits, heat dissipation, structural integrity.  
- **Trains**: first vehicle tier; bounded to rails; later **physics cars/boats/airships/planes**.  
- **Rockets**: ground‑up construction; engines/tanks/guidance; first‑person flight.

## Simulation
- Ground vehicles: traction, mass distribution, drivetrain torque curves (simplified).  
- Boats: buoyancy & drag; wind/wave hooks (optional).  
- Aircraft: lift/drag approximations; turbulence hooks from weather (optional).  
- Rockets: thrust/weight, staging, atmospheric drag; **Outer Wilds‑like** orbital pacing.

## Testing & UI
- Test pads/tracks; stat readouts; stability hints; failure modes preview.  
- Dock profiling for trains (lane⇄manifold mapping; dwell/throughput).

**[OPEN]** Damage model (crash, wear, heat) scope per difficulty?  
**[OPEN]** Material tiers for frames/skins and unlock order?
