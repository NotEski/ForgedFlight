/docs/GDD/04-systems/power-voltage.md
# 04 – Systems / Power & Energy (Mechanical & Electrical)

> **[DECISION]** Replace abstract voltage tiers with **real units**:  
> • **Mechanical:** torque (**N·m**), speed (**RPM/rad·s⁻¹**), power **Pₘ = τ·ω**.  
> • **Electrical:** voltage (**V**), current (**A**), resistance (**Ω**), power **Pₑ = V·I**.  
> Machines specify **min/nominal voltage**, **running current**, and **surge current** instead of “tiers”.

## 1) Mechanical Side (Rotational)
> Mechanical system values (RPM, torque, power per tier) will be specified during playtesting. Use real-life references for inspiration, but simplify for gameplay clarity and balance.
- **Backbone for contraptions**: shafts, belts/chains, clutches, gearboxes, governors, flywheels.
- **Ratings per part**: **Torque limit (N·m)**, **RPM limit**, **efficiency** per joint.
- **Power**: `Pₘ (W) = τ (N·m) × ω (rad/s)` with `ω = 2π·RPM/60`.
- **Safety**: slip → wear → tooth shear → shaft snap (difficulty scales thresholds).
- **[OPEN]** Default joint loss % and failure thresholds.

## 2) Electrical Side (Real Units)
- **Buses** are **DC** by default (simpler). AC/inverters may arrive later.  
- **Wires** have **resistance per meter** and **amp limits** (heat). **Voltage drop** matters over distance.  
- **Protection**: fuses/breakers; brownouts when voltage under spec; thermal trips on overload.
- **Common rails (suggested presets, not tiers)**: **12 V**, **24 V**, **48 V** DC via regulators. Players can pick any, but presets help early game.

### Wire Gauges (suggested starting values) — **[OPEN] finalize**
- **Light**: 0.20 Ω/m, 10 A max  
- **Standard**: 0.05 Ω/m, 50 A max  
- **Heavy**: 0.01 Ω/m, 250 A max  
(Voltage drop `ΔV = I × R_total`; heat budget scales with I²R.)

## 3) Conversion Between Mechanical ↔ Electrical
- **Generator/Dynamo (MR→DC bus)**  
  - Equations: `E = kₑ · ω` (open-circuit bus voltage before regulation), `τ = kₜ · I` (shaft torque from load current).  
  - **SI relation**: `kₑ ≈ kₜ` when using SI units (V·s/rad and N·m/A).  
  - **Efficiency** `η_gen` → `Pₑ ≈ η_gen · Pₘ` under load.
  - Include **rectifier** (if alternator) and **voltage regulator** to hold target bus voltage (12/24/48 V or user set).
- **Motor (DC bus→MR)**  
  - `τ = kₜ · I`, back-EMF `E = kₑ · ω`; net supplied `V ≈ E + I·R_internal`.  
  - **Efficiency** `η_motor` and **stall current** constraints.

**[OPEN]** Default constants per device class (kₑ/kₜ, internal R, η, I_max).

## 3a) Generator/Alternator **Classes** (Constant Conversion Model)
**[DECISION]** Use a **constant conversion** model with **class-limited max output**, instead of electrical tiers.

**Rule of thumb**
- Electrical output power is **proportional to mechanical input power** with a class efficiency, **capped** by the device’s **P_max**:
  
  `Pₑ_out = clamp( η_class × Pₘ , 0 , P_max_class )`, where `Pₘ = τ × ω`.
  
- Below a **cut-in RPM**, output is **0 W** (regulator disabled). Above cut-in, a **regulator** holds the chosen **DC bus** (e.g., 12/24/48 V).  
  Current on the bus: `I ≈ Pₑ_out / V_bus` (minus small regulator losses).

**Player-visible behavior**
- Spin it faster or apply more torque → more watts, **until** you hit the class **P_max**.  
- If loads ask for more than **P_max**, the bus **browns out** or the regulator **droops** (configurable).  
- Mechanical drag rises with electrical load (we compute `τ_load ≈ Pₑ_out / (ω × η_class)` under the hood).

**Class examples (numbers TBD)**
- **Alternator Mk.I** — **[OPEN]** `P_max` (kW), `η` (%), **cut-in RPM**. Early tech.  
- **Alternator Mk.II** — **[OPEN]** higher `P_max`, better `η`.  
- **Alternator Mk.III** — **[OPEN]** industrial `P_max`, best `η`.

**Notes**
- Keep **real units** (V/A/Ω/W and τ/RPM) everywhere; classes only bound **how much** conversion is allowed.  
- Motors remain real-unit devices; no “tiers” needed.

## 4) Machines & Specs
Each powered machine declares:
- **Min/nominal voltage (V)**, **running current (A)**, **surge current (A for t ms)**, **idle draw**, **power factor** (if AC later).  
- **Mechanical coupling** (if it accepts MR): nominal **τ/RPM** window and clutch requirements.
- Mis-spec behavior: **undervoltage** → slow/fault; **overcurrent** → fuse trip; **overspeed** → shutdown/damage.

## 5) Components
- **Voltage Regulator**: holds bus setpoint; dissipates excess; efficiency curve.  
- **Batteries/Capacitors**: buffer surges; source/sink on bus with internal R.  
- **Meters**: voltmeter, ammeter, wattmeter, clamp meter.  
- **Transformers/Inverters** (later): AC rails, frequency drives. **[OPEN]** Milestone placement.

## 6) UI & Debug
- **Electrical Overlay**: live **V** and **A** heatmap on cables; show **ΔV** per span.  
- **Bus Panel**: setpoint, generation, load, net charge, fault log.  
- **Tap Tool**: click a machine to see min/nominal V/A, current draw, and brownout status.

### Bus Panel (Alternators)
- Show **P_gen** and **P_max (class cap)**, **η_class**, and **cut-in RPM** status.  
- Indicate **droop/brownout** when demand exceeds **P_max** or when **V_bus** falls below machine minimums.

## 7) Difficulty Hooks
- Easy: generous wire amp limits; lower I²R heat; lenient brownout windows.  
- Normal: baseline numbers.  
- Hard/Ultra: aggressive heat, tight brownouts, realistic fuse curve.

## 8) Migration Notes (from VT tiers)
- Replace VT tiers with **bus presets** (12/24/48 V suggested) while keeping the **real V/A math** under the hood.  
- Old “VT ratings” become **min voltage** + **current draw** lines on the spec sheet.
