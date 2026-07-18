# UE Project Context

*Last updated: 2026-07-16*

## Engine & Project Overview
**Engine version:** UE 5.8 — Launcher build
**Project name:** MoCap_1
**Description:** Multiplayer party-racing game (Mario-Kart-on-foot style) built on top of Epic's Game Animation Sample (motion-matching Sandbox character), with a GAS-driven mystery-box item system, race tracks with checkpoints/laps, and obstacle-course hazards.
**Project type:** Game (prototype, derived from the Game Animation Sample — `Category: Samples` in .uproject)
**Genre / domain:** Third-person multiplayer racing / party game
**Target platforms:**
- Windows (DX12, SM6; desktop-max graphics: Lumen, Nanite, VSM, MSAA 4x alternative AA method 4)
- Multiplayer model: **listen server only** — no dedicated server or matchmaking planned

## Module Structure
**Primary game module:** `MoCap_1` (only module)

| Module | Type | Notes |
|--------|------|-------|
| MoCap_1 | Runtime | All game code; Public/Private folder split |

**Key dependencies:**
- **MoCap_1**: PublicDeps: Core, CoreUObject, Engine, InputCore, EnhancedInput, GameplayAbilities, GameplayTags, GameplayTasks, UMG; PrivateDeps: Niagara, Slate, SlateCore

**Targets:** `MoCap_1.Target.cs` (Game), `MoCap_1Editor.Target.cs` (Editor). No Server/Client targets.

## Plugin Dependencies
**Gameplay:**
- GameplayAbilities (GAS) — item/ability system
- SmartObjects + GameplayInteractions — NPC/world interactions (from sample)
- NetworkPrediction, Mover — experimental movement (sample ships both CMC and Mover character variants; CVar `DDCvar.LocomotionSetupMover=1`)

**Animation (Game Animation Sample stack):**
- PoseSearch (Motion Matching), Chooser, MotionWarping, AnimationWarping, AnimationLocomotionLibrary, Locomotor, CurveExpression, RigLogic

**Character/Capture:**
- MetaHuman, MetaHumanCharacter, MetaHumanBodyTracker, MetaHumanLiveLink, LiveLink, LiveLinkControlRig, HairStrands — mocap/LiveLink pipeline

**Editor/AI tooling:**
- ModelContextProtocol + AIAssistant + Terminal + the full toolset suite (EditorToolset, GASToolsets, UMGToolSet, NiagaraToolsets, PCGToolset, etc.) — MCP server on `http://127.0.0.1:8000/mcp` for AI-driven editor automation
- ModelingToolsEditorMode (editor only)

**Custom plugins:** None (no `Plugins/` directory in the project).

## Coding Conventions
**Naming prefixes:** Standard UE (F/U/A/E/I) plus project prefix `MO_` on all game classes (`AMO_PlayerState`, `UMO_AbilitySystemComponent`, `FMO_GameplayTags`). Follows the user's Aura reference architecture (see `~/.claude/projects/.../memory/aura-gas-architecture.md`).
**Header style:** `#pragma once`
**Log categories in use:** None defined yet (uses `LogTemp`)
**Assertion style:** `check()` preferred
**Header organization:** Public/Private folders, domain subfolders (AbilitySystem, Character, Components, Game, Input, Obstales*, Pickups, Player, Race, UI)
**Additional rules:**
- `TObjectPtr<>` for UPROPERTY object references
- Replicated properties use `ReplicatedUsing=OnRep_*` with old-value params + multicast delegates for UI (Aura pattern)
- Native gameplay tags via `FMO_GameplayTags` C++ singleton, registered from `UMO_AssetManager::StartInitialLoading`
- Doc comments on classes explaining architectural intent
- *`Obstales` folder name is a typo but is load-bearing — preserve it, don't "fix" include paths

## Subsystems in Use
**Gameplay framework:**
- GameMode: `GM_Sandbox` (Blueprint, from Game Animation Sample) — `/Game/Blueprints/GM_Sandbox`
- GameState: `AMO_RaceGameState` — race orchestration: checkpoint validation, lap counting, race positions, lap times
- PlayerController: `AMO_PlayerController` (C++) / `PC_Sandbox` (BP, sample)
- PlayerState: `AMO_PlayerState` — **owns the ASC** (Aura-style placement, survives respawn); replicated race stats (score, laps, position, lap times) with OnRep + native multicast delegates
- Character: `AMO_BaseCharacter` (C++); BP characters `BP_MO_Base`, `SandboxCharacter_CMC` / `SandboxCharacter_Mover` (sample)
- HUD: `AMO_HUD` — owns/creates widget controllers (Aura WidgetController MVC pattern)
- AssetManager: `UMO_AssetManager` (set in DefaultEngine.ini) — initializes native gameplay tags

**Custom systems:**
- **Mystery-box item system (GAS):** `MysteryBox` / `MysteryBoxSpawner` pickups grant item abilities; main + queued item slots; `UMO_ItemInfo` data asset maps ability tags → UI/ability class
- **Race system:** `MO_RaceTrack` + `MO_RaceCheckpoint` + `MO_KillVolume`; position/lap-time HUD
- **Obstacles:** BouncePad, FallingPlatform, SlidingDoor, SpeedPad, SpinningDisc, SwingingHammer (in `Obstales/`)
- **Movement components:** `LaunchComponent`, `SpeedBoostComponent`
- **UI:** Aura WidgetController pattern — `UMO_WidgetController` base → `UMO_OverlayWidgetController`; `UMO_UserWidget` base; per-mystery-box widget controllers; `SpeedLinesWidget`

**GAS usage:**
- ASC: `UMO_AbilitySystemComponent` (on PlayerState for players)
- Ability base: `UMO_GameplayAbility` → `UMO_ItemAbility` → item abilities: Bazooka, Bonk, Lube, Mosquito, Yeet, Zeus
- Attribute Sets: **none yet** (planned — TODO)
- Input: Enhanced Input via `UMO_InputConfig` / `UMO_InputComponent` (Aura pattern); single item slot bound to `InputTag.UseItem` — the item ability holding that tag in its spec's DynamicAbilityTags is what LMB activates
- Key native tags (`FMO_GameplayTags`): `InputTag.UseItem`, `Abilities.Item.{Bazooka,Bonk,Lube,Mosquito,Yeet,Zeus}`
- Ini tags (from sample): `Foley.Event.*`, `MotionMatching.*`, `SmartObject.ObjectType.*`, `StateTree.*`, `Abilities.ContextualAnim`, `Abilities.MoveTo`
- **Architecture reference:** new GAS work follows the user's Aura-5.7 framework conventions

**Known TODOs (as of 2026-07-15):**
- Real `GA_*` ability logic (several are stubs)
- Attribute set
- Race countdown
- Position-weighted item probability

## Build Configuration
**Build targets:** Game, Editor
**Custom macros / build flags:** None
**Third-party libraries:** None
**Platform-specific notes:** Windows-first (DX12/SM6); Android sections in config are engine defaults, not a real target
**Engine modifications:** None (launcher build)

## Team Context
**Team size:** Solo developer
**Source control:** Git — GitHub remote `IGR15/Project_Broke` (https://github.com/IGR15/Project_Broke), branch `main`. Note: working tree also lives inside a OneDrive-synced folder.
**Notes:** Editor is driven both manually and via the MCP AI toolchain; Blueprint graphs are sometimes authored programmatically through MCP.
