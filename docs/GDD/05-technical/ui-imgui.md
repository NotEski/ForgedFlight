/docs/GDD/05-technical/ui-imgui.md
# 05 – Technical / UI (ImGui)

## Integration
- ImGui with docking/layout persistence; **pixel/bitmap fonts** with DPI scaling.  
- **Panel registry** for Debug: Perf HUD, Console, World/Chunk, Items/Recipes, Tech Tree, Sun/Season, Net diag.

## Game UI Style
- Satisfactory-leaning themed windows with **pixel-art** skins (9-slice).  
- Survival HUD bottom-left: Health, Water, Hunger, Temp, O₂ with **trend arrows**.  
- Overlays: orange (heat), blue (cold), desaturation (hunger/thirst).  
- **Permissive rebinding** (multi-bind; conflicts allowed; panic reset & timed revert).

**[OPEN]** Icon pack & palette; DPI scale defaults; metrics panel.

## Blueprint UI
- **Blueprint Browser** with tags/search/thumbnails; personal vs faction filter.
- **Placement HUD** showing collisions, needed items, power draw, ETA.
- **Agent Panels** for Robotic Arm / Builder Bay (queues, priorities, throughput).

## Research UI
- **Player Tree**: owned/available nodes; artifacts; “Use” on artifacts in inventory.
- **Lab Network Panel**: queue, throughput, link status, modules, target output (player/chest).
- **Artifact Library**: list & share **Books/Chips/Cores**.

## Model Viewer (Dev)
- Load **.glb**; inspect nodes/bones, materials, animations. Wireframe/collision/origin. Live-reload.

### Model Viewer Checks
- **Facet rule** (no curved geometry) and **flat shading** validation.
- Scale check: **1 unit = 1 m**; voxel alignment helpers.

## Contraption Tools
- **Shaft Network Overlay:** rpm/torque/load with color mapping and direction arrows.
- **Probe Tool:** click any shaft to read **rpm/Nm/kW**, part ratings, recent faults.

## Warehouse Terminal
- Global search (post-scanner): item counts with **locations** and **lead time/ETA** if you place an **Order**.  
- **Pick Tickets** for Kit Racks; highlight shelf paths. Permissioned bays/shelves; audit log.

## Blueprint Material Checklist HUD
- Required/remaining items for active ghost/blueprint.  
- Toggle **Snap-to-Ghost**; quick filters to find parts in nearby containers.

## Electrical Tools
- **Cable Overlay:** visualize **voltage (V)** and **current (A)**; highlight over-amp segments and voltage drop hotspots.  
- **Bus Panel:** voltage setpoint, generation vs load, battery charge, faults.  
- **Tap Tool (Electrical):** min/nominal **V/A**, surge, live draw, brownout margin.

### Bus Panel (Alternators)
- Show **P_gen** vs **P_max (class cap)**, **η_class**, and **cut-in RPM** status; indicate **droop/brownout**.

## World Settings (Admin)
- **GameMode:** dropdown **None / PvE / PvP / PvPvE** (default highlighted **PvPvE**) with helper text and preset preview.  
- Toggles: PvP (derived by mode), StructureDamage, Explosives, HostileFauna intensity, LandClaims, RequireWarForClaimDamage, RequireBothOnlineForClaimDamage, RaidWindows schedule.  
- Panels: **Claims Overlay toggle**, **War Declarations** list, **Raid Schedule** editor.

## Flag Tools
### Pattern Designer
- Layerable patterns & abstract shapes; move/scale/rotate per layer; color palette; order/blend modes **[OPEN]**.

### Pixel Art Designer
- Grid canvas (e.g., 96×64 px), layers, palette lock, symmetry, stamps, dithering.  
- Save/load faction presets; export to **banners/decals/UI emblems**; server bakes atlases.

### Asset Notice
- **Flags** and **Banners** are edited **separately**; conversions are not automatic to preserve readability.
