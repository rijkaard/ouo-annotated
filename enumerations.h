/*
 * enumerations.h — Centralised enum definitions for the ouo UO server.
 *
 * DO NOT duplicate enums already defined as named types in other headers:
 *   EntityTypeTag, EventType   — entity.h
 *   AccountFlag                — account.h
 *   ItemFlag                   — item.h
 *   MobileFlag                 — mobile.h
 *   PlayerFlag                 — player.h
 *   TileFlag, LandTileFlag, TerrainTileFlag — terrain.h
 *
 * Promoted #define blocks (timer.h TIMER_EVENT_*, wombat.h WTYPE_* / STMT_*
 * RNODE_* / WOMBAT_ARRAY_TYPE_*, resbank.h EQ_QUAL_*) are defined here as
 * proper enums.  Once this header is adopted, those #define sequences should
 * be removed from their respective source headers and replaced with an
 * include of this file.
 */

#ifndef ENUMERATIONS_H_
#define ENUMERATIONS_H_

/* =========================================================================
 * 1. CHARACTER & MOBILE
 * ========================================================================= */

/* Three primary stat indices — STR/DEX/INT.
 * Used by CMobile_GetStat, CMobile_SetBaseStat, skill weight tables, etc. */
enum StatIndex {
    STAT_STR = 0, /* Strength  — controls max HP pool  */
    STAT_DEX = 1, /* Dexterity — controls max Stamina  */
    STAT_INT = 2, /* Intelligence — controls max Mana  */
};

/* Selects which stat-bar packet(s) CMobile_BroadcastStatUpdate sends. */
enum StatUpdateType {
    STATUPDATE_HP      = 0, /* HP_HEALTH packet only      */
    STATUPDATE_MANA    = 1, /* MANA_HEALTH packet only    */
    STATUPDATE_STAMINA = 2, /* FAT_HEALTH packet only     */
    STATUPDATE_ALL     = 3, /* Combined HEALTH (all three bars) */
};

/* Locomotion class stored in CMobile.movementType (save tag "movetype").
 * Controls surface-flag queries and Z-range checks.
 * Values 1/3/5 — gaps intentional (template flag bits 0/1/2). */
enum MovementType {
    MOVETYPE_WALK = 1, /* Land-walking creature       */
    MOVETYPE_SWIM = 3, /* Water-capable / amphibious  */
    MOVETYPE_FLY  = 5, /* Flying creature             */
};

/* Passability evaluation context passed to GetLandTileFlags / surface queries.
 * Values 0-8 — DIFFERENT from MovementType above (entity.c GetLandTileFlags). */
enum MoveType {
    MOVETYPE_PASS_WALK    = 0, /* Standard walk: blocked if tile lacks walkable flag */
    MOVETYPE_PASS_RUN     = 1, /* Same passability as walk                            */
    MOVETYPE_PASS_MOVE2   = 2, /* Same passability as walk                            */
    MOVETYPE_PASS_GHOST   = 3, /* Blocked if tile has wet/blocking flag (inverted)    */
    MOVETYPE_PASS_FLY     = 4, /* Same passability as ghost                           */
    MOVETYPE_PASS_SWIM    = 5, /* Strictest: blocked if not walkable OR tile is wet   */
    MOVETYPE_PASS_MOVE6   = 6, /* Same passability as walk/run                        */
    MOVETYPE_PASS_BLOCKED = 7, /* Always returns 0x600 (impassable); early exit       */
    MOVETYPE_PASS_BOAT    = 8, /* Exempts water tile ranges, then falls through walk  */
};

/* Weapon-swing phase — combat.c version (4 values, authoritative).
 * SWING_FIRED=3 is a return value from CMobile_AdvanceSwingState only;
 * stored swingState only ever holds 0-2. */
enum SwingState {
    SWING_IDLE   = 0, /* Below wind-up threshold; idle              */
    SWING_WINDUP = 1, /* Wind-up in progress                        */
    SWING_READY  = 2, /* Past wind-up delay; ready to fire          */
    SWING_FIRED  = 3, /* Return-only: swing fired, state reset to 0 */
};

/* Discrete reputation tier returned by NotoValueToLevel; used for packet
 * coloring and paperdoll title selection. */
enum NotorietyLevel {
    NOTO_INNOCENT = 1, /* Blue;   never attacked anyone criminal    */
    NOTO_FRIEND   = 2, /* Green;  guild ally                        */
    NOTO_ANIMAL   = 3, /* Grey;   neutral animal or creature        */
    NOTO_CRIMINAL = 4, /* Grey;   recent criminal act               */
    NOTO_ENEMY    = 5, /* Orange; guild enemy                       */
    NOTO_MURDERER = 6, /* Red;    CMobile_IsMurderer returns true   */
};

/* 8-way compass heading stored in CMobile.direction and used in all
 * movement functions.  Also used in wombat_exec.c CalcDirection strings. */
enum Direction {
    DIR_NORTH     = 0, /* Decreasing Y   */
    DIR_NORTHEAST = 1,
    DIR_EAST      = 2, /* Increasing X   */
    DIR_SOUTHEAST = 3,
    DIR_SOUTH     = 4, /* Increasing Y   */
    DIR_SOUTHWEST = 5,
    DIR_WEST      = 6, /* Decreasing X   */
    DIR_NORTHWEST = 7,
};

/* Biological sex stored in CMobile.sex; selects gendered title strings
 * and pronouns in wombat_exec.c. */
enum MobileSex {
    SEX_MALE   = 0, /* "Swordsman", "Armsman", he/him/his    */
    SEX_FEMALE = 1, /* "Swordswoman", "Armswoman", she/her   */
};

/* Staff sub-rank stored in tag "counType"; selects displayed counselor title. */
enum CounselorType {
    COUN_SEER   = 1, /* Title "Seer"             */
    COUN_COUNSELOR = 2, /* Title "Counselor"        */
    COUN_SENIOR = 3, /* Title "Senior Counselor" */
};

/* Reason code for CPlayer_BusyMessage — which "you must wait" text to send. */
enum BusyType {
    BUSY_SKILL  = 0, /* "You must wait a few moments to use another skill." */
    BUSY_ACTION = 1, /* "You must wait to perform another action."           */
};

/* Paperdoll title rank computed from best skill value / 100. */
enum SkillProficiencyRank {
    RANK_NONE        = 0,  /* 0-299 skill (values 1 and 2 also fall here) */
    RANK_NEOPHYTE    = 3,  /* 300-399 */
    RANK_NOVICE      = 4,  /* 400-499 */
    RANK_APPRENTICE  = 5,  /* 500-599 */
    RANK_JOURNEYMAN  = 6,  /* 600-699 */
    RANK_EXPERT      = 7,  /* 700-799 */
    RANK_ADEPT       = 8,  /* 800-899 */
    RANK_MASTER      = 9,  /* 900-999 */
    RANK_GRANDMASTER = 10, /* 1000    */
};

/* Index into CMobile.skills[50].  Source of truth: rundir/skills.txt
 * (positional order).  Passed as int8_t skillId throughout skill.h API.
 * Note: player.c title-string switch uses a different ordering for some
 * skills (e.g. Hiding=20 in player.c vs Herding=20 here) — skills.txt wins. */
enum SkillId {
    SKILL_ALCHEMY       =  0, /* alchemy          */
    SKILL_ANATOMY       =  1, /* anatomy          */
    SKILL_ANIMAL_LORE   =  2, /* animlore         */
    SKILL_ITEM_ID       =  3, /* appraise         */
    SKILL_ARMS_LORE     =  4, /* armslore         */
    SKILL_PARRYING      =  5, /* battle_defense   */
    SKILL_BEGGING       =  6, /* begging          */
    SKILL_BLACKSMITHY   =  7, /* blacksmithing    */
    SKILL_BOWCRAFT      =  8, /* bowyer_and_fletcher */
    SKILL_PEACEMAKING   =  9, /* calm             */
    SKILL_CAMPING       = 10, /* campingsk        */
    SKILL_CARPENTRY     = 11, /* carpentry        */
    SKILL_CARTOGRAPHY   = 12, /* mapmaking        */
    SKILL_COOKING       = 13, /* cooking          */
    SKILL_DETECT_HIDDEN = 14, /* detcthid         */
    SKILL_ENTICEMENT    = 15, /* entice           */
    SKILL_EVAL_INT      = 16, /* evaluate         */
    SKILL_HEALING       = 17, /* first_aid        */
    SKILL_FISHING       = 18, /* fishing          */
    SKILL_FORENSIC      = 19, /* forensic         */
    SKILL_HERDING       = 20, /* herding          */
    SKILL_HIDING        = 21, /* hidesk           */
    SKILL_PROVOCATION   = 22, /* incite           */
    SKILL_INSCRIPTION   = 23, /* inscribe         */
    SKILL_LOCKPICKING   = 24, /* lockpicking      */
    SKILL_MAGERY        = 25, /* magic            */
    SKILL_MAGIC_RESIST  = 26, /* magic_defense    */
    SKILL_TACTICS       = 27, /* melee_fighting   */
    SKILL_SNOOPING      = 28, /* peek             */
    SKILL_MUSICIANSHIP  = 29, /* play_instrument  */
    SKILL_POISONING     = 30, /* poisonsk         */
    SKILL_ARCHERY       = 31, /* ranged_weapon    */
    SKILL_SPIRIT_SPEAK  = 32, /* seance           */
    SKILL_STEALING      = 33, /* stealing         */
    SKILL_TAILORING     = 34, /* tailoring        */
    SKILL_ANIMAL_TAMING = 35, /* tame             */
    SKILL_TASTE_ID      = 36, /* taste            */
    SKILL_TINKERING     = 37, /* tinkering        */
    SKILL_TRACKING      = 38, /* tracking         */
    SKILL_VETERINARY    = 39, /* veterinarian     */
    SKILL_SWORDSMANSHIP = 40, /* melee_fighting   */
    SKILL_MACE_FIGHTING = 41, /* melee_fighting   */
    SKILL_FENCING       = 42, /* melee_fighting   */
    SKILL_WRESTLING     = 43, /* melee_fighting   */
    SKILL_LUMBERJACKING = 44, /* bladed           */
    SKILL_MINING        = 45, /* bladed           */
    SKILL_MEDITATION    = 46, /* meditation       */
    SKILL_STEALTH       = 47, /* stealth          */
    SKILL_REMOVE_TRAP   = 48, /* removetrap       */
    SKILL_COUNT         = 49, /* MAX_SKILLS=50; slot 49 unused in skills.txt */
};

/* Selects which mobile cap-stat to get/set via Script_GetMaxStat /
 * Script_SetMaxStat in wombat_exec.c. */
enum MaxStatId {
    STAT_MAX_HP      = 0, /* CMobile_GetMaxHp / VT_SET_MAX_HP       */
    STAT_MAX_STAMINA = 1, /* CMobile_GetMaxStamina / VT_SET_MAX_STAMINA */
    STAT_MAX_MANA    = 2, /* CMobile_GetMaxMana / VT_SET_MAX_MANA   */
};

/* =========================================================================
 * 2. COMBAT & ANIMATION
 * ========================================================================= */

/* Queued animation/sound command type in AnimSequence_Process. */
enum AnimSeqCmdType {
    ANIMCMD_LOC_EFFECT        = 0, /* Fixed-location EFFECT (type 2), no serial      */
    ANIMCMD_MOB_EFFECT        = 1, /* Mobile-attached EFFECT (type 3), source serial  */
    ANIMCMD_LIGHTNING         = 2, /* Lightning EFFECT (type 1), source serial only   */
    ANIMCMD_MISSILE_LOC2LOC   = 3, /* Moving EFFECT (type 0), loc to loc              */
    ANIMCMD_MISSILE_LOC2MOB   = 4, /* Moving EFFECT (type 0), loc to mob serial       */
    ANIMCMD_MISSILE_MOB2LOC   = 5, /* Moving EFFECT (type 0), mob serial to loc       */
    ANIMCMD_MISSILE_MOB2MOB   = 6, /* Moving EFFECT (type 0), serial to serial        */
    ANIMCMD_ANIMATE_MOBILE    = 7, /* ANIM packet for a mobile                        */
    ANIMCMD_SFX               = 8, /* PlaySoundAtLocation                             */
    ANIMCMD_SFX_TO            = 9, /* SendSoundToEntity                               */
};

/* Packet 0x70 (EFFECT) sub-type — which graphical effect the client renders. */
enum EffectType {
    EFFECT_MISSILE   = 0, /* Moving projectile from src to dst         */
    EFFECT_LIGHTNING = 1, /* Lightning strike on entity                */
    EFFECT_LOC_FIXED = 2, /* Fixed effect at a location (no serial)    */
    EFFECT_MOB_FIXED = 3, /* Fixed effect attached to a mobile (serial) */
};

/* Weapon-swing animation type — selects animId for mounted vs on-foot attackers. */
enum SwingAnimType {
    SWINGANIM_SLASH    = 0, /* One-handed slash;     0x0D mounted / 0x09 foot */
    SWINGANIM_BACKHAND = 1, /* One-handed backhand;  0x0C mounted / 0x0B foot */
    SWINGANIM_OVERHEAD = 2, /* Two-handed overhead;  0x0E mounted / 0x0A foot */
};

/* =========================================================================
 * 3. NPC
 * ========================================================================= */

/* Action a predator/scanner takes against its chosen scan target. */
enum NPCScanActionType {
    SCAN_ACTION_ATTACK     = 0, /* CombatInitiate; aiState = ATTACK_TARGET        */
    SCAN_ACTION_PACK_MERGE = 1, /* NPC_PackMerge — join pack with target NPC      */
    SCAN_ACTION_FLEE       = 2, /* Project away from threat; aiState = PURSE_SHELTER */
};

/* Internal state numbering used by CNPC_IdleScan while it overwrites
 * npc->aiState; translated to/from NPC_STATE_* on entry/exit. */
enum NPCIdleScanState {
    ISCAN_WANDER    =  0, /* Idle wander     → NPC_STATE_IDLE           */
    ISCAN_PURSUE    =  1, /* Shelter pursuit → NPC_STATE_SEEK_SHELTER   */
    ISCAN_RUNAWAY   =  2, /* Aversion flee   → NPC_STATE_RUNAWAY        */
    ISCAN_COMBAT    =  3, /* Predator attack → NPC_STATE_ATTACK_TARGET  */
    ISCAN_FOLLOWING =  4, /* Following owner → NPC_STATE_FOLLOWING      */
    ISCAN_TALKING   =  5, /* Talking (string table only; no exit-xlate) */
    ISCAN_LOITER    =  6, /* Loiter (string table only; no exit-xlate)  */
    ISCAN_SLEEP     =  7, /* Sleep countdown → NPC_STATE_EAT_FOOD      */
    ISCAN_IDLE      = 10, /* Default idle; entry-xlated from NPC_STATE_IDLE (0xA) */
};

/* State parameter to CMobile_NPC_SetAIState — selects animation and sound
 * for the current AI activity. */
enum NPCAnimAction {
    NPC_ANIM_WANDER         = 1, /* Walk animation + sfxNotice              */
    NPC_ANIM_IDLE_SOUND     = 2, /* sfxIdle only; no animation              */
    NPC_ANIM_WANDER_VARIETY = 3, /* Randomised idle/graze/rest + sfxIdle    */
    NPC_ANIM_COMBAT_SWING   = 4, /* Attack swing + melee miss SFX           */
    NPC_ANIM_COMBAT_WANDER  = 5, /* Combat-walk animation + sfxNotice       */
};

/* npc->aiByte3 movement speed modifier.  Non-zero forces 80% speed.
 * Confidence: low — semantics beyond speed effect are unknown. */
enum NPCAIByte3Mode {
    AIBYTE3_DEFAULT = 0, /* No speed override; falls through to aiState table */
    AIBYTE3_MODE1   = 1, /* speedPct forced to 80                             */
    AIBYTE3_MODE2   = 2, /* speedPct forced to 80 (likely second pursuit mode) */
};

/* Broad creature category output from classifyCreature() in wombat_exec.c. */
enum MobTypeClass {
    MOB_CLASS_UNKNOWN = 0, /* Default / unrecognised body type; *outDiff=4 */
    MOB_CLASS_ANIMAL  = 1, /* Animal — OBJPICKER graphic 0x2122            */
    MOB_CLASS_MONSTER = 2, /* Monster — OBJPICKER graphic 0x20D8           */
    MOB_CLASS_PERSON  = 3, /* Humanoid — OBJPICKER graphic 0x2106          */
};

/* =========================================================================
 * 4. ITEMS & CONTAINERS
 * ========================================================================= */

/* GUMP ID sent to clients to select container art/bounds for an open container.
 * Source: CItem_GetContainerGump / CContainer_GetContainerBounds in container.c. */
enum ContainerGumpId {
    CGUMP_UNKNOWN_07        = 0x07, /* Bounds table only; no body type maps here   */
    CGUMP_DEFAULT           = 0x09, /* Fallback when no body type matches          */
    CGUMP_BACKPACK          = 0x3C, /* Backpack (body 0x09B0/0x09B2/0x0E75/0x0E79) */
    CGUMP_LEATHERBAG        = 0x3D, /* Leather bag (body 0x0E76)                   */
    CGUMP_GENERIC           = 0x3E, /* Barrel / small bag / keg / crate generic    */
    CGUMP_KEG               = 0x3F, /* Keg (body 0x0E7A)                           */
    CGUMP_UNKNOWN_40        = 0x40, /* Bounds table only; no body type maps here   */
    CGUMP_BANKBOX           = 0x41, /* Bank box / metal chest                      */
    CGUMP_MEDCRATE_EAST     = 0x42, /* Medium crate facing east                    */
    CGUMP_WOODBOX_SOUTH     = 0x43, /* Wooden box facing south                     */
    CGUMP_LARGECHEST        = 0x44, /* Wooden box / large chest                    */
    CGUMP_SHIPHOLD          = 0x47, /* Ship hold (body 0x2AF8)                     */
    CGUMP_ARMOIRE           = 0x48, /* Chest of drawers / armoire                  */
    CGUMP_MEDCRATE_SOUTH    = 0x49, /* Medium crate facing south                   */
    CGUMP_METALCHEST_SMALL  = 0x4A, /* Small metal chest                           */
    CGUMP_METALBOX          = 0x4B, /* Metal box                                   */
    CGUMP_SECURETRADE       = 0x4C, /* Secure trade window                         */
    CGUMP_BOOKCASE          = 0x4D, /* Bookcase                                    */
    CGUMP_FANCYDRESSER_A    = 0x4E, /* Fancy dresser variant A                     */
    CGUMP_FANCYDRESSER_B    = 0x4F, /* Fancy dresser variant B                     */
    CGUMP_WOODCHEST         = 0x51, /* Wooden chest / shelf / small crate          */
    CGUMP_COFFIN            = 0x52, /* Coffin (body 0x1E5E)                        */
    CGUMP_SPELLBOOK         = 0x091A, /* Game board / spellbook (body 0x0FA6)      */
    CGUMP_SCROLLCASE        = 0x092E, /* Scroll case (body 0x0E1C/0x0FAD)          */
};

/* Caller-supplied passability context for CItem_GetSurfaceFlags_VT.
 * Controls which tile-flag combinations count as blocking, wet, or passable. */
enum SurfaceMoveType {
    SMT_WALK       = 0, /* Standard player walk                                  */
    SMT_RUN        = 1, /* Grouped with SMT_DOOR_AWARE/SMT_MOUNT                 */
    SMT_FLY        = 2, /* Height < 0x14 overrides impassable                    */
    SMT_CREATURE_A = 3, /* flags & 0x80 -> wet; else solid; grouped with 4       */
    SMT_CREATURE_B = 4, /* Same as SMT_CREATURE_A                                */
    SMT_EXTENDED   = 5, /* Solid + wet + impassable all checked; propagates 0x80 */
    SMT_DOOR_AWARE = 6, /* Like SMT_RUN but clears blocking for doors             */
    SMT_WATER      = 7, /* Always returns 0x200 (wet/impassable); boat hulls     */
    SMT_MOUNT      = 8, /* Grouped with SMT_RUN and SMT_DOOR_AWARE               */
};

/* Two-bit sub-field (bits 14-15) of the VT_GET_FLAGS word; grammatical article
 * prepended to an item's display name in CItem_GetNameString_VT. */
enum ItemArticle {
    ARTICLE_NONE = 0x0000, /* No article; name printed bare */
    ARTICLE_A    = 0x4000, /* Prepend "a "                  */
    ARTICLE_AN   = 0x8000, /* Prepend "an "                 */
    ARTICLE_THE  = 0xC000, /* Prepend "the "                */
};

/* =========================================================================
 * 5. WORLD & RESOURCES
 * ========================================================================= */

/* Single-character discriminator in dynamic0.mul save records.
 * dynamic.c normalizes to lowercase (+= 0x20) before dispatch.
 *
 * CONFLICT NOTE: entity.h @=X comment says ETYPE_MULTI; dynamic.c 'x' case
 * constructs CCorpse.  Similarly entity.h @=B says ETYPE_BOAT; dynamic.c 'b'
 * constructs CBulletinBoard.  The dynamic.c switch is the runtime truth. */
enum SaveTypeChar {
    SAVE_ITEM       = 'd', /* CItem (implied by entity.h @=D; may use default path) */
    SAVE_CONTAINER  = 'c', /* CContainer_Constructor                                 */
    SAVE_WEAPON     = 'w', /* CWeapon_ConstructorFromItem                            */
    SAVE_MOBILE     = 'm', /* CMobile_Constructor                                    */
    SAVE_NPC        = 'n', /* CResourceMobile_Init on a CNPC                         */
    SAVE_PLAYER     = 'p', /* CPlayer_Constructor                                    */
    SAVE_SHOPKEEPER = 's', /* CShopkeeper_ConstructorNoArgs                          */
    SAVE_GUARD      = 'g', /* CGuard_Constructor on a CNPC                           */
    SAVE_CORPSE     = 'x', /* CCorpse_Constructor (conflicts with entity.h ETYPE_MULTI) */
    SAVE_EGG        = 'e', /* CEgg_Constructor                                       */
    SAVE_SIGNPOST   = 'z', /* CSignpost_Constructor                                  */
    SAVE_BBOARD     = 'b', /* CBulletinBoard_Constructor (conflicts with entity.h ETYPE_BOAT) */
};

/* Decay-rate profile used by the world item-decay scanner (CWorld_InitDecay). */
enum DecayMode {
    DECAY_NORMAL   = 0, /* 4 buckets/tick, max age 0xFA — slow background decay    */
    DECAY_FAST     = 1, /* 0x6D buckets/tick, max age 0x48 — aggressive cleanup    */
    /* value 2: no case; gap suggests removed or unimplemented mode                */
    DECAY_INSTANT  = 3, /* 0x4000 buckets/tick, max age 3 — likely debug/test mode */
    /* default: DECAY_DISABLED — interval=0, all rates zeroed; no decay            */
};

/* NPC template classification; drives CreateEntity dispatch at spawn time. */
enum NPCTemplateType {
    TMPL_TYPE_UNKNOWN    = 0, /* Unclassified / fallback                          */
    TMPL_TYPE_ITEM       = 1, /* Spawns as a static item                          */
    TMPL_TYPE_NORMAL     = 2, /* Standard NPC mobile                              */
    TMPL_TYPE_GUARD      = 3, /* Guard NPC (maps to ETYPE_GUARD at runtime)       */
    TMPL_TYPE_SHOPKEEPER = 4, /* Vendor NPC (maps to ETYPE_SHOPKEEPER at runtime) */
};

/* Moral alignment of a spawned NPC; parsed from template "alignment" field. */
enum NPCAlignment {
    ALIGNMENT_NEUTRAL = 0,
    ALIGNMENT_GOOD    = 1,
    ALIGNMENT_EVIL    = 2,
    ALIGNMENT_CHAOTIC = 3,
};

/* Top-level type of a CResBankSet; determines which subtype names are valid. */
enum ResBankSetType {
    SET_TYPE_WALL       = 0, /* Building wall pieces              */
    SET_TYPE_HOUSE      = 1, /* House structure                   */
    SET_TYPE_TREE       = 2, /* Tree / foliage                    */
    SET_TYPE_TERRAIN    = 3, /* Ground terrain                    */
    SET_TYPE_ROOF       = 4, /* Roof pieces                       */
    SET_TYPE_FLATROOF   = 5, /* Flat roof                         */
    SET_TYPE_COASTLINE  = 6, /* Water/land transition pieces      */
    SET_TYPE_TRANSITION = 7, /* Terrain-to-terrain transition     */
};

/* Geometric role of a tile within a WALL set (CResBankSetMember.subtype). */
enum WallMemberSubtype {
    WALL_SUB_NS         =  0, /* North-South segment "NS(/)"       */
    WALL_SUB_EW         =  1, /* East-West segment "EW(\\)"        */
    WALL_SUB_N_CORNER   =  2, /* North corner                      */
    WALL_SUB_S_CORNER   =  7, /* South corner; gap at 3-6          */
    WALL_SUB_ROOF_BIT_1 = 0xA, /* First roof attachment bit        */
    WALL_SUB_ROOF_BIT_2 = 0xB, /* Second roof attachment bit       */
    WALL_SUB_UNKNOWN    = 0xC, /* Default / unrecognised           */
};

/* Geometric role of a tile within a ROOF set (CResBankSetMember.subtype). */
enum RoofMemberSubtype {
    ROOF_SUB_PIECE_1 =  0,
    ROOF_SUB_PIECE_2 =  1,
    ROOF_SUB_PIECE_3 =  2,
    ROOF_SUB_PIECE_4 =  3,
    ROOF_SUB_PIECE_5 =  4,
    ROOF_SUB_PIECE_6 =  5,
    ROOF_SUB_PIECE_7 =  6,
    ROOF_SUB_PIECE_8 =  7,
    ROOF_SUB_SW_JOIN =  8,
    ROOF_SUB_NE_JOIN =  9,
    ROOF_SUB_NW_JOIN = 0xA,
    ROOF_SUB_SE_JOIN = 0xB,
    ROOF_SUB_X_JOIN  = 0xC,
    ROOF_SUB_N_T     = 0xD, /* T-junction, north */
    ROOF_SUB_S_T     = 0xE, /* T-junction, south */
    ROOF_SUB_W_T     = 0xF, /* T-junction, west  */
    ROOF_SUB_E_T     = 0x10, /* T-junction, east */
    ROOF_SUB_UNKNOWN = 0x11, /* Default / unrecognised */
};

/* Geometric role of a tile within a COASTLINE set (CResBankSetMember.subtype). */
enum CoastlineMemberSubtype {
    COAST_SUB_BANK_TL_BR       =  0, /* Diagonal bank, TL to BR           */
    COAST_SUB_BANK_TR_BL       =  1, /* Diagonal bank, TR to BL           */
    COAST_SUB_BANK_T_B         =  2, /* Straight bank, top to bottom      */
    COAST_SUB_EDGE_TL_BANK_BR_1 = 3,
    COAST_SUB_EDGE_TL_BANK_BR_2 = 4,
    COAST_SUB_EDGE_TR_BANK_BL_1 = 5,
    COAST_SUB_EDGE_TR_BANK_BL_2 = 6,
    COAST_SUB_EDGE_BL_BANK_TR_1 = 7,
    COAST_SUB_EDGE_BL_BANK_TR_2 = 8,
    COAST_SUB_EDGE_BR_BANK_TL_1 = 9,
    COAST_SUB_EDGE_BR_BANK_TL_2 = 0xA,
    COAST_SUB_EDGE_T_BANK_B    = 0xB,
    COAST_SUB_EDGE_L_BANK_R    = 0xC,
    COAST_SUB_EDGE_R_BANK_L    = 0xD,
    COAST_SUB_EDGE_B_BANK_T    = 0xE,
    COAST_SUB_RAVINE_WIDTH     = 0xF,  /* Ravine width parameter tile      */
    COAST_SUB_RAVINE_FLOOR     = 0x10, /* Ravine floor tile                */
    COAST_SUB_UNKNOWN          = 0x11, /* Default / unrecognised           */
};

/* Geometric role of a tile within a TRANSITION set (CResBankSetMember.subtype). */
enum TransitionMemberSubtype {
    TRANS_SUB_TILE_1        =  0, /* Primary fill tile                    */
    TRANS_SUB_TILE_2        =  1, /* Secondary fill tile                  */
    TRANS_SUB_2_CORNER_TOP  =  2,
    TRANS_SUB_2_CORNER_RIGHT  = 3,
    TRANS_SUB_2_CORNER_BOTTOM = 4,
    TRANS_SUB_2_CORNER_LEFT   = 5,
    TRANS_SUB_1_CORNER_TOP    = 6,
    TRANS_SUB_1_CORNER_RIGHT  = 7,
    TRANS_SUB_1_CORNER_BOTTOM = 8,
    TRANS_SUB_1_CORNER_LEFT   = 9,
    TRANS_SUB_2TR_1BL  = 0xA, /* Diagonal: 2-tile TR, 1-tile BL */
    TRANS_SUB_1TL_2BR  = 0xB, /* Diagonal: 1-tile TL, 2-tile BR */
    TRANS_SUB_1TR_2BL  = 0xC, /* Diagonal: 1-tile TR, 2-tile BL */
    TRANS_SUB_2TL_1BR  = 0xD, /* Diagonal: 2-tile TL, 1-tile BR */
    TRANS_SUB_UNKNOWN  = 0xE, /* Default / unrecognised         */
};

/* Bucketed proximity into a verbal description (wombat_exec.c distTable). */
enum DistanceRating {
    DIST_HERE       = 0, /* "right here"             */
    DIST_SHORT      = 1, /* "just a short way"       */
    DIST_WAYS       = 2, /* "a ways"                 */
    DIST_FAIR       = 3, /* "a fair distance"        */
    DIST_LONG       = 4, /* "a long way"             */
    DIST_QUITE_LONG = 5, /* "quite a long distance"  */
};

/* =========================================================================
 * 6. NETWORKING & PACKETS
 * ========================================================================= */

/* Speech delivery mode in HandlePacket_SPEECH (packet 0x03). */
enum SpeechMode {
    SPEECH_REGULAR   = 0, /* Range 9; shares path with SYSTEM/FOCUSED */
    SPEECH_BROADCAST = 1, /* GM only; sent to all players globally     */
    SPEECH_EMOTE     = 2, /* Range 7; dead players cannot emote        */
    SPEECH_SYSTEM    = 3, /* Range 9; shares path with REGULAR         */
    SPEECH_FOCUSED   = 4, /* Range 9; shares path with REGULAR         */
    SPEECH_SPELL     = 6, /* Spell incantation via TEXT_ENTRY 0x26     */
    SPEECH_WHISPER   = 8, /* Range 1                                   */
    SPEECH_YELL      = 9, /* Range 18; triggers CWorld_SpeechNotifyNearby */
};

/* Bulletin board sub-command in HandlePacket_BBOARD (packet 0x71). */
enum BBoard_SubCmd {
    BBOARD_REQ_POST_BODY    = 3, /* Client requests full body of a specific post   */
    BBOARD_REQ_BOARD_SUMMARY = 4, /* Client requests board header/summary           */
    BBOARD_POST_MESSAGE     = 5, /* Client submits new message or reply             */
    BBOARD_REMOVE_POST      = 6, /* Client deletes a post                           */
};

/* GM world-editing sub-command in HandlePacket_GODCOMMAND (packet 0x12). */
enum GodCmdSubType {
    GCMD_OUTDATED       =  0, /* "Outdated form of editing"; no-op             */
    GCMD_1              =  1, /* No-op                                         */
    GCMD_2              =  2, /* No-op                                         */
    GCMD_3              =  3, /* No-op                                         */
    GCMD_CREATE_DYNAMIC =  4, /* Create dynamic CItem via CWorld_CreateItem    */
    GCMD_DELETE_TOP     =  5, /* Delete highest item at X,Y                    */
    GCMD_SET_HIDDEN     =  6, /* SetHiddenFlag on player                       */
    GCMD_CREATE_NPC     =  7, /* Create NPC from template                      */
    GCMD_8              =  8, /* No-op                                         */
    GCMD_9              =  9, /* No-op                                         */
    GCMD_10             = 10, /* No-op                                         */
    GCMD_CREATE_STATIC  = 11, /* Create static entity; sets g_EditorStaticCreateFlag=1 */
};

/* Macro sub-command in HandlePacket_TEXT_ENTRY (packet 0x9A). */
enum MacroSubCmd {
    MACRO_STUB_07         = 0x07, /* Stub — immediate return                         */
    MACRO_STUB_08         = 0x08, /* Stub                                            */
    MACRO_STUB_09         = 0x09, /* Stub                                            */
    MACRO_USE_SKILL       = 0x24, /* UseSkillByMacro: parse "skillId skillArg"        */
    MACRO_CAST_SPELL_NAME = 0x26, /* CastSpellByName; broadcast as SPEECH_SPELL      */
    MACRO_OPEN_SPELLBOOK  = 0x27, /* OpenSpellbookToSpell                            */
    MACRO_OPEN_DOOR_EQUIP = 0x43, /* OpenDoor_Equipment: from equipment slots        */
    MACRO_CAST_SPELL_ID   = 0x56, /* CastSpellByID: search spellbook/backpack        */
    MACRO_TOGGLE_DIR_FLAG = 0x57, /* Toggle a player direction flag by index         */
    MACRO_OPEN_DOOR_SPATIAL = 0x58, /* OpenDoor: spatial search in facing direction  */
    MACRO_LAST_SPELL      = 0x59, /* Recast last spell via LastSpell()               */
    MACRO_SET_COMBAT_BYTES = 0x5C, /* Write combatByte2/3/4 from sscanf "%d %d %d"  */
    MACRO_BOW_SALUTE      = 0xC7, /* Bow (anim 0x20) or salute (0x21) if unarmed    */
};

/* TriggerEdit / GodViewQuery operations in HandlePacket_TriggerEdit (0x7E). */
enum TriggerEditCmd {
    TEDIT_QUERY_ENTITY   =  0, /* Serialize entity scripts + tag defs to response */
    TEDIT_ATTACH_SCRIPT  =  1, /* Entity_AttachScript by name                     */
    TEDIT_OP_CALL        =  2, /* Call TriggerEdit_Op545E(entity, opData)          */
    TEDIT_SET_OBJVAR     =  4, /* Set entity ObjVar; inner dispatch on ObjVarType  */
    TEDIT_SET_STRING_PROP =  5, /* TriggerEdit_SetStringProp(entity, opData)        */
    TEDIT_OP_546F        =  7, /* TriggerEdit_Op546F(opData)                       */
    TEDIT_TAG_SEARCH     =  8, /* Scan all entities for tag defs matching name/val */
    TEDIT_SCRIPT_CALLBACK = 9, /* Script_callback(serial, 1, connIndex)            */
    TEDIT_DELETE_ENTITIES = 10, /* Delete connIndex entities by serial list        */
};

/* Object-variable storage type in CEntity_SetObjVar and TriggerEdit wire
 * serialisation.  Wire value 2 encodes OBJVAR_LOC (internal value 3). */
enum ObjVarType {
    OBJVAR_INT = 0, /* 32-bit integer                    */
    OBJVAR_STR = 1, /* CString                           */
    OBJVAR_LOC = 3, /* CLocation (x/y/z); wire type = 2 */
    OBJVAR_OBJ = 4, /* Object reference (32-bit serial)  */
};

/* Sub-request type in HandlePacket_CLIENTQUERY (packet 0x34). */
enum ClientQuerySubType {
    CQUERY_MUSIC       = 0x02, /* Send region music for resource slot   */
    CQUERY_RESTYPE     = 0x03, /* GM: CResourceTypeManager_SendAll      */
    CQUERY_STATUS      = 0x04, /* Send MOBILESTAT (0x11) for target     */
    CQUERY_SKILLS_ALL  = 0x05, /* Send full skill list                  */
    CQUERY_SKILL_SINGLE = 0x06, /* Send single skill update             */
    CQUERY_RES_NODE    = 0xFD, /* Send resource node data for target    */
};

/* Door tile-art orientation (8 values), obtained from tiledata layer field. */
enum DoorFacing {
    DOOR_FACING_0 = 0, /* Open: +y -x; Close: -y +x              */
    DOOR_FACING_1 = 1, /* Open: +y +x; Close: -y -x              */
    DOOR_FACING_2 = 2, /* Open: -x;    Close: +x                 */
    DOOR_FACING_3 = 3, /* Open: -y +x; Close: +y -x              */
    DOOR_FACING_4 = 4, /* Open: +y +x; Close: -y -x (same as 1) */
    DOOR_FACING_5 = 5, /* Open: -y +x; Close: +y -x (same as 3) */
    DOOR_FACING_6 = 6, /* Open: +y -x; Close: -y +x (same as 0) */
    DOOR_FACING_7 = 7, /* Open: -y;    Close: +y                 */
};

/* =========================================================================
 * 7. GAME SYSTEMS
 * ========================================================================= */

/* Timer event dispatch table index (TimerNode.eventType).
 * Promoted from TIMER_EVENT_* #defines in timer.h.
 * 21-entry table at 0x00613C18; fired by ScheduleEvent. */
enum TimerEventType {
    TIMER_EVENT_DELETE            =  1, /* Delete entity on expiry                  */
    TIMER_EVENT_DECAY_CONTAINER   =  2, /* Decay container contents                 */
    TIMER_EVENT_IDLE_DISCONNECT   =  3, /* Disconnect idle client                   */
    TIMER_EVENT_DOOR_CLOSE        =  4, /* Auto-close door                          */
    TIMER_EVENT_CALLBACK          =  5, /* Generic script callback                  */
    TIMER_EVENT_AI_SCHED_DELETE   =  6, /* Delete entity after AI schedule end      */
    TIMER_EVENT_SPEECH            =  7, /* Deliver delayed NPC speech (extraData)   */
    TIMER_EVENT_VALUELESS_DECAY   =  8, /* Decay zero-value item                    */
    TIMER_EVENT_ONLINE_CHECK      =  9, /* Periodic connection keep-alive check     */
    TIMER_EVENT_NPC_TICK          = 10, /* NPC AI tick                              */
    TIMER_EVENT_SET_STRING_PROP   = 11, /* Set a string object-variable property    */
    TIMER_EVENT_WAR_MODE_CLEAR    = 12, /* Clear war-mode flag                      */
    TIMER_EVENT_SET_STRING_DETACH = 13, /* Set string property then detach timer    */
    TIMER_EVENT_UNFREEZE          = 14, /* Remove frozen status                     */
    TIMER_EVENT_UNSQUELCH         = 15, /* Remove squelch                           */
    TIMER_EVENT_CLR_INVULN        = 16, /* Clear invulnerability                    */
    TIMER_EVENT_CRIMINAL          = 17, /* Criminal-flag expiry (0x11)              */
    TIMER_EVENT_SET_CHAOS         = 18, /* Set chaos allegiance                     */
    TIMER_EVENT_SET_ORDER         = 19, /* Set order allegiance                     */
    TIMER_EVENT_CTRL_TIMEOUT      = 20, /* Pet/control timeout                      */
};

/* Placement qualifier for <eq> template entries.
 * Promoted from EQ_QUAL_* #defines in resbank.h. */
enum EquipQualifier {
    EQ_QUAL_WEAR          = 0, /* Worn in body slot (default)             */
    EQ_QUAL_SELFCONTAINED = 1, /* Carried loot, not worn                  */
    EQ_QUAL_SELLABLE      = 2, /* Vendor stock container (equipment[26])  */
    EQ_QUAL_BUYABLE       = 3, /* Vendor buy-list container (equipment[28]) */
    /* value 4 absent in source */
    EQ_QUAL_CONTAINED     = 5, /* First container in equipment[0..25]    */
    EQ_QUAL_INVENT        = 6, /* Vendor offered container (equipment[27]) */
};

/* Body-layer slot indices used by getItemAtSlot / equipment[].
 * Values confirmed from Script_getItemAtSlot and Script_getFreeHandSlot
 * in wombat_exec.c. */
enum EquipLayer {
    EQUIP_RIGHT_HAND   =  1,  /* Primary weapon / right hand            */
    EQUIP_LEFT_HAND    =  2,  /* Shield / secondary / left hand         */
    EQUIP_SHOES        =  3,
    EQUIP_PANTS        =  4,
    EQUIP_SHIRT        =  5,
    EQUIP_HELMET       =  6,
    EQUIP_GLOVES       =  7,
    EQUIP_RING         =  8,
    /* slot 9 unused */
    EQUIP_NECK         = 10,
    EQUIP_HAIR         = 11,  /* 0x0B */
    EQUIP_WAIST        = 12,
    EQUIP_INNER_TORSO  = 13,
    EQUIP_OUTER_TORSO  = 14,  /* Robe / surcoat                         */
    EQUIP_BRACELET     = 15,
    EQUIP_FACIAL_HAIR  = 16,  /* 0x10 */
    EQUIP_MIDDLE_TORSO = 17,  /* Tunic / half apron                     */
    EQUIP_EARRINGS     = 18,
    EQUIP_ARMS         = 19,
    EQUIP_CLOAK        = 20,
    EQUIP_BACKPACK     = 21,  /* 0x15 — player/NPC main pack            */
    EQUIP_OUTER_LEGS   = 22,
    EQUIP_INNER_LEGS   = 23,
    EQUIP_BEARD        = 24,
    EQUIP_MOUNT        = 25,  /* 0x19 — mount / follower slot           */
    EQUIP_NPC_SELL     = 26,  /* Vendor sell container                  */
    EQUIP_NPC_BUY      = 27,  /* Vendor buy-list container              */
    EQUIP_NPC_OFFERED  = 28,  /* Vendor offered-item container          */
    EQUIP_BANK         = 29,  /* 0x1D — bank box                        */
};

/* GCM orchestrator server management events (gamecentmon.c).
 * Switched as eventType - 1. */
enum GCMServerEventType {
    GCM_SRV_STATUS_QUERY        = 1, /* Responds with player/NPC counts, etc.  */
    GCM_SRV_UNKNOWN2            = 2, /* Unhandled — falls to default           */
    GCM_SRV_SPAWN_ENABLE        = 3, /* g_SpawnEnabled = 1                     */
    GCM_SRV_SPAWN_DISABLE       = 4, /* g_SpawnEnabled = 0                     */
    GCM_SRV_INITIAL_SPAWN_CLEAR = 5, /* g_IsInitialSpawn = 0                   */
    GCM_SRV_INITIAL_SPAWN_SET   = 6, /* g_IsInitialSpawn = 1                   */
    GCM_SRV_TICK_NPCS           = 7, /* GameCentMon_TickAllNPCs()              */
};

/* GCM monitor resource/template management events (gamecentmon.c).
 * Switched as eventType - 1.  Overlapping values have different semantics
 * from GCMServerEventType above. */
enum GCMMonEventType {
    GCM_MON_BROADCAST_REGIONS  =  1, /* Stream resource-bank regions; responds 0xA */
    GCM_MON_ENTITY_LIST        =  2, /* Build entity list; responds 0xC            */
    GCM_MON_CLEAR_STATE        =  3, /* g_gcmState = 0                             */
    GCM_MON_REGION_TMPL_UPDATE =  4, /* Write template counts into CResBankRegion  */
    GCM_MON_SPAWN_PASSTHROUGH  =  5, /* Forward raw spawn-data payload             */
    GCM_MON_ACK                =  6, /* Empty ack; responds 0xC flag=0xFFFF        */
    GCM_MON_REBUILD_SPAWN      =  7, /* BuildSpawnEntries()                        */
    GCM_MON_RESET_DECAY        =  8, /* GameCentMon_ResetDecay()                   */
    GCM_MON_RELOAD_TEMPLATES   =  9, /* CTemplateManager_Shutdown() + startup()    */
    GCM_MON_TICK_TEMPLATE      = 10, /* GameCentMon_TickTemplateChains(templateId) */
    GCM_MON_SET_AI_TIMER       = 11, /* Writes g_npcAITimerReset from packet       */
};

/* GCM NPC/mobile action after byteTable normalization (confidence: low).
 * Raw switchIndex range 0-17 maps to caseIndex 0-9 via 18-entry byteTable. */
enum GCMNPCActionType {
    GCM_NPC_BROADCAST_SUBTYPE  = 0, /* Broadcasts subtype & 0xFF; shared with 2  */
    GCM_NPC_TELEPORT           = 1, /* Reads (x,y), hides entity, drops at loc   */
    GCM_NPC_BROADCAST_SUBTYPE2 = 2, /* Same broadcast-all path as 0              */
    GCM_NPC_SELECT_TARGET      = 3, /* 0xFF=DeselectTarget; else SelectTarget    */
    GCM_NPC_BROADCAST_1        = 4, /* Sends broadcast code 1                    */
    GCM_NPC_BROADCAST_DEATH    = 5, /* Sends broadcast codes 2 then 6            */
    GCM_NPC_DATA_PASSTHROUGH   = 6, /* Swaps int32 endian, overrides serial      */
    GCM_NPC_BROADCAST_7        = 7, /* Sends broadcast code 7                    */
    GCM_NPC_BROADCAST_8        = 8, /* Sends broadcast code 8                    */
    GCM_NPC_BROADCAST_9        = 9, /* Sends broadcast code 9                    */
};

/* =========================================================================
 * 8. WOMBAT SCRIPTING ENGINE
 * ========================================================================= */

/* Runtime type of a Wombat script variable or expression result.
 * Promoted from WTYPE_* #defines in wombat.h.
 * Used as typeId, retType, subType, CListNode.typeTag, TagNode.type,
 * EventParam.type.  Valid list node tags are 0-5; 6-7 abort the thread. */
enum WombatType {
    WTYPE_INT     = 0, /* sig char 'i'; integer                          */
    WTYPE_STRING  = 1, /* sig char 's'; CString                          */
    WTYPE_USTRING = 2, /* sig char 'q'; CUString                         */
    WTYPE_LOC     = 3, /* sig char 'c'; CLocation (6 bytes)              */
    WTYPE_OBJ     = 4, /* sig char 'o'; object reference (serial)        */
    WTYPE_LIST    = 5, /* sig char 'l'; CList                            */
    WTYPE_VOID    = 6, /* sig char 'v'; size 0; CListNode_Match aborts thread */
    WTYPE_UNKNOWN = 7, /* sig char 'u'; size 0; wildcard in sig matching */
    WTYPE_COUNT   = 8, /* sentinel / "not found" return from SigCharToTypeId */
};

/* Return value of ClassifyStatement; selects parse path for current token.
 * Promoted from STMT_* #defines in wombat.h. */
enum StatementType {
    STMT_VAR_DECL     =  0, /* Type keyword or TK_MEMBER; byte-table jump[0] */
    STMT_LOCAL_VAR    =  1, /* Local variable name; byte-table jump[1]        */
    STMT_TRIGGER_VAR  =  2, /* Trigger/context variable; byte-table jump[1]   */
    STMT_INT_LITERAL  =  3, /* T_BYTE / T_WORD / T_DWORD; jump[5]            */
    STMT_STR_LITERAL  =  4, /* Starts with '"'; jump[5]                       */
    STMT_USTR_LITERAL =  5, /* Starts with 'L"'; jump[5]                      */
    STMT_BUILTIN_CALL =  6, /* Control-flow token or SM_RBRACE; jump[2]       */
    STMT_MEMBER_VAR   =  7, /* Member variable reference; jump[3]             */
    STMT_OPERATOR     =  8, /* Operator from g_OperatorTable; jump[5]         */
    STMT_SEMI         =  9, /* SM_SEMI no-op; jump[5]                         */
    STMT_GOTO         = 10, /* TK_GOTO label; jump[4]                         */
    STMT_UNKNOWN      = 11, /* Unrecognized default                           */
};

/* What ResultNode.value points to during expression evaluation/compilation.
 * Promoted from RNODE_* #defines in wombat.h.
 * Types 3 and 5 are lvalues; assign handlers require one on the LHS. */
enum ResultNodeType {
    RNODE_HANDLER_RESULT = 0,  /* value = BuiltinHandlerEntry *                    */
    RNODE_FUNC_REF       = 1,  /* value = script function index                    */
    RNODE_TVAR_RVAL      = 2,  /* value = WombatVar *; trigger var rvalue          */
    RNODE_LVAR_LVAL      = 3,  /* value = WombatVar *; local var lvalue            */
    RNODE_LVAR_RVAL      = 4,  /* value = WombatVar *; local var rvalue            */
    RNODE_TVAR_LVAL      = 5,  /* value = WombatVar *; trigger var lvalue          */
    RNODE_INT_LIT        = 6,  /* value = int literal                              */
    RNODE_STR_LIT        = 7,  /* value = CString *                                */
    RNODE_USTR_LIT       = 8,  /* value = CUString * (also member ref)             */
    RNODE_GOTO_LABEL     = 9,  /* value = malloc'd char * (label name)             */
    RNODE_LOC_RESULT     = 10, /* value = packed x|y, extra = z                    */
    RNODE_OBJ_RESULT     = 11, /* value = serial                                   */
};

/* Data type tag for a WombatArray column (row 0 of the flat buffer).
 * Promoted from WOMBAT_ARRAY_TYPE_* #defines in wombat.h.
 * Corresponds to the INT/STRING/USTRING subset of WombatType. */
enum WombatArrayColType {
    WARRAY_TYPE_INT  = 0, /* Integer column   */
    WARRAY_TYPE_STR  = 1, /* CString column   */
    WARRAY_TYPE_USTR = 2, /* CUString column  */
};

/* Kind of node in a compiled Wombat ResultNode stream.
 * Drives the execution engine's per-node dispatch and size-calculation. */
enum WombatNodeType {
    WNODE_HANDLER_REF    =  0, /* BuiltinHandlerEntry reference; calls GetVarType  */
    WNODE_FUNC_CALL      =  1, /* CFunction reference; calls GetFuncRetType        */
    WNODE_TRIG_VAR_RVAL  =  2, /* Trigger-scope variable, read-only               */
    WNODE_LOCAL_VAR_LVAL =  3, /* Local-scope variable, lvalue; flag=1             */
    WNODE_LOCAL_VAR_RVAL =  4, /* Local-scope variable, read-only                  */
    WNODE_TRIG_VAR_LVAL  =  5, /* Trigger-scope variable, lvalue; flag=1           */
    WNODE_INT_LITERAL    =  6, /* Integer constant; type=WTYPE_INT                 */
    WNODE_STRING_LITERAL =  7, /* CString constant; type=WTYPE_STRING              */
    WNODE_USTRING_LITERAL =  8, /* CUString / member-string; type=WTYPE_USTRING   */
    WNODE_GOTO_LABEL     =  9, /* Goto target; no runtime data (size=0)            */
    WNODE_SEMI           = 10, /* Statement separator; size=sizeof(void*)          */
};

/* Handler-dispatch parameter-marshalling signature codes.
 * paramTypeLookup[] and resultTypeLookup[] in wombat_exec.c are keyed by
 * typeSig character and return these values. */
enum WombatSigCode {
    WSIG_LOC     = 0, /* 'c'/'C' — 6-byte location, passed as struct pointer */
    WSIG_INT     = 1, /* 'i'/'I' — integer, passed by value                  */
    WSIG_INT2    = 2, /* 'j'     — secondary/auxiliary integer                */
    WSIG_LIST    = 3, /* 'l'     — CList pointer                              */
    WSIG_OBJ     = 4, /* 'o'/'O' — object serial, by value                   */
    WSIG_USTRING = 5, /* 'q'     — CUString pointer                          */
    WSIG_STRING  = 6, /* 's'     — CString pointer                           */
    WSIG_COMPLEX = 7, /* 'u'     — unresolved/polymorphic; walks param list   */
    WSIG_SEP     = 8, /* '|'     — separator; does not consume a slot         */
};

/* Comparator selector for script list sorting (wombat_exec.c).
 * Bit 0 of flags is the reverse flag; type = flags & ~1. */
enum SortListType {
    SORT_INT    = 0, /* sortList_cmpInt */
    SORT_STRING = 2, /* sortList_cmpStr; value 2 because bit 0 is reserved */
    SORT_OBJ    = 4, /* sortList_cmpObj */
};

/* Selects which name string to retrieve from a CResourceType definition. */
enum ResourceNameId {
    RESNAME_FOOD = 0, /* CResourceType_GetFoodName */
    RESNAME_1    = 1, /* CResourceType_GetName1   */
    RESNAME_2    = 2, /* CResourceType_GetName2   */
    RESNAME_3    = 3, /* CResourceType_GetName3   */
};

/* Exact ABI for invoking a BuiltinHandlerEntry function pointer.
 * Encodes: argument count, pass-by-value vs pass-by-address, and how the
 * return value is captured.  169 entries; enables replacing the large
 * switch in wombat_exec.c:1413-2018. */
enum HandlerCallConv {
    HCALL_VOID_0              =   0, /* void, 0 args                                      */
    HCALL_RET_0               =   1, /* returns uintptr_t, 0 args                         */
    HCALL_VOID_1V             =   2, /* void, 1 arg by value                              */
    HCALL_VOID_2V             =   3, /* void, 2 args by value                             */
    HCALL_VOID_1A             =   4, /* void, 1 arg by address                            */
    HCALL_VOID_1A_1V          =   5, /* void, arg1 by addr, arg2 by value                 */
    HCALL_VOID_1A_1V_b        =   6, /* same signature as 5 (distinct handler variant)    */
    HCALL_VOID_1V_b           =   7, /* void, 1 arg by value (variant)                   */
    HCALL_VOID_1V_c           =   8, /* void, 1 arg by value (variant)                   */
    HCALL_VOID_2V_b           =   9, /* void, 2 args by value                             */
    HCALL_VOID_2V_c           =  10, /* void, 2 args by value (variant)                  */
    HCALL_VOID_2V_d           =  11, /* void, 2 args by value (shares case with 9)        */
    HCALL_VOID_2V_e           =  12, /* void, 2 args by value                             */
    HCALL_VOID_2V_f           =  13, /* void, 2 args by value                             */
    HCALL_VOID_1A_1V_c        =  14, /* void, arg1 by addr, arg2 by value                 */
    HCALL_VOID_2A             =  15, /* void, both args by address                        */
    HCALL_VOID_2V_g           =  16, /* void, 2 args by value                             */
    HCALL_VOID_7V             =  17, /* void, 7 args by value                             */
    HCALL_VOID_6V             =  18, /* void, 6 args by value                             */
    HCALL_VOID_1A_3V          =  19, /* void, arg1 by addr, args 2-4 by value             */
    HCALL_RET_1V              =  20, /* returns obj, 1 arg by value                       */
    HCALL_RET_2V              =  21, /* returns obj, 2 args by value                      */
    /* 22 unused */
    HCALL_RET_2V_b            =  23, /* returns obj, 2 args by value                      */
    HCALL_RETSTR_2V           =  24, /* returns CString via temp, 2 args by value         */
    HCALL_RETUSTR_2V          =  25, /* returns CUString via temp, 2 args by value        */
    HCALL_RETLOC_2V           =  26, /* returns CLocation via temp, 2 args by value       */
    HCALL_RET_2V_c            =  27, /* returns obj, 2 args by value                      */
    HCALL_RETSTR_2V_b         =  28, /* returns CString via temp (variant)                */
    HCALL_RETLOC_2V_b         =  29, /* returns CLocation via temp (variant)              */
    HCALL_RET_2V_d            =  30, /* returns obj, 2 args by value                      */
    HCALL_VOID_TYPEDPAIR_1V   =  31, /* void, arg1 by val, arg2 as (typeId, addr)         */
    HCALL_VOID_TYPEDPAIR_2V   =  32, /* void, arg1+3 by val, arg2 as (typeId, addr)       */
    HCALL_RET_2V_e            =  33, /* returns obj, 2 args by value                      */
    HCALL_RET_1V_b            =  34, /* returns obj, 1 arg by value                       */
    HCALL_RET_TYPEDPAIR       =  35, /* returns obj, arg1 by val, arg2 as (typeId, addr)  */
    HCALL_VOID_2V_h           =  36, /* void, 2 args by value                             */
    HCALL_VOID_2V_i           =  37, /* void, 2 args by value                             */
    HCALL_VOID_2V_j           =  38, /* void, 2 args by value                             */
    HCALL_RET_1V_1A           =  39, /* returns obj, arg1 by val, arg2 by addr            */
    HCALL_RET_1V_1A_1V        =  40, /* returns obj, arg1 val, arg2 addr, arg3 val        */
    HCALL_VOID_1V_1A_1V       =  41, /* void, arg1 val, arg2 addr, arg3 val               */
    HCALL_VOID_1V_1A_2V       =  42, /* void, arg1 val, arg2 addr, args 3-4 val           */
    HCALL_RET_1V_c            =  43, /* returns obj, 1 arg by value                       */
    HCALL_RET_1V_d            =  44, /* returns obj, 1 arg by value                       */
    HCALL_RET_2V_f            =  45, /* returns obj, 2 args by value                      */
    HCALL_RET_3V              =  46, /* returns obj, 3 args by value                      */
    HCALL_RET_3V_b            =  47, /* returns obj, 3 args by value                      */
    HCALL_VOID_3V             =  48, /* void, 3 args by value                             */
    HCALL_RET_2V_g            =  49, /* returns obj, 2 args by value                      */
    HCALL_VOID_2V_k           =  50, /* void, 2 args by value                             */
    HCALL_VOID_3V_b           =  51, /* void, 3 args by value                             */
    HCALL_VOID_4V             =  52, /* void, 4 args by value                             */
    HCALL_VOID_5V             =  53, /* void, 5 args by value                             */
    HCALL_VOID_1V_4A          =  54, /* void, arg1 val, args 2-5 by addr                  */
    HCALL_VOID_6V_b           =  55, /* void, 6 args by value                             */
    HCALL_VOID_8V             =  56, /* void, 8 args by value                             */
    HCALL_VOID_1A_2V          =  57, /* void, arg1 by addr, args 2-3 by value             */
    HCALL_RET_1A_1V           =  58, /* returns obj, arg1 by addr, arg2 by value          */
    HCALL_RET_1A_2V           =  59, /* returns obj, arg1 by addr, args 2-3 by value      */
    HCALL_RET_1A_4V           =  60, /* returns obj, arg1 by addr, args 2-5 by value      */
    HCALL_RET_1A_3V           =  61, /* returns obj, arg1 by addr, args 2-4 by value      */
    HCALL_RET_1A_4V_b         =  62, /* returns obj, arg1 by addr, args 2-5 by value      */
    HCALL_RET_1A_5V           =  63, /* returns obj, arg1 by addr, args 2-6 by value      */
    HCALL_RET_1A_2V_b         =  64, /* returns obj, arg1 by addr, args 2-3 by value      */
    HCALL_RET_1A_1V_3A_3V     =  65, /* returns obj, complex mixed addr/val, 8 params     */
    HCALL_RET_3V_c            =  66, /* returns obj, 3 args by value                      */
    HCALL_RET_3V_d            =  67, /* returns obj, 3 args by value                      */
    HCALL_RET_4V              =  68, /* returns obj, 4 args by value                      */
    HCALL_VOID_1A_1V_d        =  69, /* void, arg1 by addr, arg2 by value                 */
    HCALL_RET_2A              =  70, /* returns obj, both args by address                 */
    HCALL_RETLOC_2A           =  71, /* returns CLocation via temp, both args by addr     */
    HCALL_VOID_2V_l           =  72, /* void, 2 args by value                             */
    HCALL_VOID_2V_m           =  73, /* void, 2 args by value                             */
    HCALL_RET_2V_h            =  74, /* returns obj, 2 args by value                      */
    HCALL_VOID_3V_c           =  75, /* void, 3 args by value                             */
    HCALL_VOID_3V_d           =  76, /* void, 3 args by value                             */
    HCALL_VOID_4V_b           =  77, /* void, 4 args by value                             */
    HCALL_RET_3V_e            =  78, /* returns obj, 3 args by value                      */
    HCALL_RETLOC_1V           =  79, /* returns CLocation via temp, 1 arg by value        */
    HCALL_RETLOC_1V_b         =  80, /* returns CLocation via temp, 1 arg by value (var)  */
    HCALL_VOID_1V_e           =  81, /* void, 1 arg by value                              */
    HCALL_VOID_2V_n           =  82, /* void, 2 args by value                             */
    HCALL_RET_0_b             =  83, /* returns obj, 0 args                               */
    HCALL_VOID_2V_o           =  84, /* void, 2 args by value                             */
    HCALL_RET_1A_b            =  85, /* returns obj, arg1 by address                      */
    HCALL_VOID_1A_b           =  86, /* void, arg1 by address                             */
    HCALL_VOID_1V_1A_b        =  87, /* void, arg1 by val, arg2 by addr                   */
    HCALL_VOID_1V_1A_1V_b     =  88, /* void, arg1 val, arg2 addr, arg3 val               */
    HCALL_RET_1V_1A_2V        =  89, /* returns obj, arg1 val, arg2 addr, args 3-4 val    */
    HCALL_VOID_1V_1A_2V_b     =  90, /* void, arg1 val, arg2 addr, args 3-4 val           */
    HCALL_RET_3V_f            =  91, /* returns obj, 3 args by value                      */
    HCALL_RET_4V_b            =  92, /* returns obj, 4 args by value                      */
    HCALL_RETSTR_2A           =  93, /* returns CString via temp, both args by address    */
    HCALL_RETSTR_1V_b         =  94, /* returns CString via temp, 1 arg by value          */
    HCALL_RETSTR_2V_c         =  95, /* returns CString via temp, 2 args by value         */
    HCALL_VOID_7V_b           =  96, /* void, 7 args by value                             */
    HCALL_RET_1V_e            =  97, /* returns obj, 1 arg by value                       */
    HCALL_RET_2V_i            =  98, /* returns obj, 2 args by value                      */
    HCALL_VOID_2V_p           =  99, /* void, 2 args by value                             */
    HCALL_VOID_4V_1A          = 100, /* void, args 1-4 by val, arg5 by addr               */
    HCALL_VOID_3V_e           = 101, /* void, 3 args by value                             */
    HCALL_VOID_3V_f           = 102, /* void, 3 args by value                             */
    HCALL_VOID_1A_2V_b        = 103, /* void, arg1 by addr, args 2-3 by value             */
    HCALL_VOID_5V_b           = 104, /* void, 5 args by value                             */
    HCALL_RET_4V_c            = 105, /* returns obj, 4 args by value                      */
    HCALL_VOID_1A_3V_b        = 106, /* void, arg1 by addr, args 2-4 by value             */
    HCALL_VOID_2V_TYPEDPAIR   = 107, /* void, args 1-2 by val, arg3 as (typeId, addr)     */
    HCALL_RET_2V_TYPEDPAIR    = 108, /* returns obj, args 1-2 val, arg3 (typeId, addr)    */
    /* 109 unused */
    HCALL_RET_1V_f            = 110, /* returns obj, 1 arg by value                       */
    HCALL_RET_2V_j            = 111, /* returns obj, 2 args by value                      */
    HCALL_RET_2V_k            = 112, /* returns obj, 2 args by value                      */
    HCALL_RET_1V_1A_c         = 113, /* returns obj, arg1 by val, arg2 by addr            */
    HCALL_VOID_2A_2V          = 114, /* void, args 1-2 by addr, args 3-4 by value         */
    HCALL_VOID_4V_1A_b        = 115, /* void, args 1-4 val, arg5 by addr                  */
    HCALL_VOID_4V_b2          = 116, /* void, 4 args by value                             */
    HCALL_VOID_4V_c           = 117, /* void, 4 args by value                             */
    HCALL_VOID_4V_d           = 118, /* void, 4 args by value                             */
    HCALL_VOID_4V_e           = 119, /* void, 4 args by value                             */
    HCALL_VOID_4V_f           = 120, /* void, 4 args by value                             */
    HCALL_RET_3V_g            = 121, /* returns obj, 3 args by value                      */
    HCALL_RETSTR_3V           = 122, /* returns CString via temp, 3 args by value         */
    HCALL_RETUSTR_3V          = 123, /* returns CUString via temp (from CString), 3 args  */
    HCALL_VOID_1V_1A_c        = 124, /* void, arg1 by val, arg2 by addr                   */
    HCALL_RET_2V_l            = 125, /* returns obj, 2 args by value                      */
    HCALL_RETSTR_1V_c         = 126, /* returns CString via temp, 1 arg by value          */
    HCALL_VOID_3V_g           = 127, /* void, 3 args by value                             */
    HCALL_VOID_1A_1V_e        = 128, /* void, arg1 by addr, arg2 by value                 */
    HCALL_RET_1A_2V_c         = 129, /* returns obj, arg1 by addr, args 2-3 by value      */
    HCALL_RET_1A_1V_b         = 130, /* returns obj, arg1 by addr, arg2 by value          */
    HCALL_RET_1V_1A_d         = 131, /* returns obj, arg1 by val, arg2 by addr            */
    HCALL_VOID_3V_h           = 132, /* void, 3 args by value                             */
    HCALL_VOID_2A_4V          = 133, /* void, args 1-2 by addr, args 3-6 by value         */
    HCALL_VOID_1A_5V          = 134, /* void, arg1 by addr, args 2-6 by value             */
    HCALL_VOID_1V_1A_4V       = 135, /* void, arg1 val, arg2 addr, args 3-6 val           */
    HCALL_VOID_6V_c           = 136, /* void, 6 args by value                             */
    HCALL_VOID_4V_g           = 137, /* void, 4 args by value                             */
    HCALL_VOID_5V_c           = 138, /* void, 5 args by value                             */
    HCALL_VOID_8V_b           = 139, /* void, 8 args by value                             */
    HCALL_VOID_1A_5V_b        = 140, /* void, arg1 by addr, args 2-6 by value             */
    HCALL_RET_3V_h            = 141, /* returns obj, 3 args by value                      */
    HCALL_RET_1A_4V_c         = 142, /* returns obj, arg1 by addr, args 2-5 by value      */
    HCALL_RET_1A_6V           = 143, /* returns obj, arg1 by addr, args 2-7 by value      */
    HCALL_RET_5V              = 144, /* returns obj, 5 args by value                      */
    HCALL_RET_1A_1V_c         = 145, /* returns obj, arg1 by addr, arg2 by value          */
    HCALL_RET_1A_2V_d         = 146, /* returns obj, arg1 by addr, args 2-3 by value      */
    HCALL_RET_3V_i            = 147, /* returns obj, 3 args by value                      */
    HCALL_VOID_5V_1A_3V       = 148, /* void, 9 params, arg6 by addr                      */
    HCALL_RET_COMPLEX_11      = 149, /* returns obj, 11 params, complex mix addr/val      */
    HCALL_RETSTR_2V_d         = 150, /* returns CString via temp, 2 args by value         */
    HCALL_RETSTR_1V_1A        = 151, /* returns CString via temp, arg1 val, arg2 addr     */
    HCALL_RET_1V_3A           = 152, /* returns obj, arg1 by val, args 2-4 by addr        */
    HCALL_VOID_3V_i           = 153, /* void, 3 args by value                             */
    HCALL_VOID_6V_d           = 154, /* void, 6 args by value                             */
    HCALL_VOID_2V_2A_1V_1A    = 155, /* void, 6 params, mixed addr/val                   */
    HCALL_RET_5V_b            = 156, /* returns obj, 5 args by value                      */
    HCALL_RET_1V_2A           = 157, /* returns obj, arg1 by val, args 2-3 by addr        */
    HCALL_RET_1V_1A_1V_1A     = 158, /* returns obj, 5 params, alternating val/addr       */
    HCALL_VOID_4V_h           = 159, /* void, 4 args by value                             */
    HCALL_VOID_3V_j           = 160, /* void, 3 args by value                             */
    HCALL_VOID_3V_k           = 161, /* void, 3 args by value                             */
    HCALL_VOID_4V_i           = 162, /* void, 4 args by value                             */
    HCALL_RET_1V_1A_1V_b      = 163, /* returns obj, arg1 val, arg2 addr, arg3 val        */
    HCALL_RET_1A_c            = 164, /* returns obj, arg1 by address                      */
    HCALL_RET_1V_1A_1V_c      = 165, /* returns obj, arg1 val, arg2 addr, arg3 val        */
    HCALL_RET_1V_1A_e         = 166, /* returns obj, arg1 by val, arg2 by addr            */
    HCALL_VOID_4V_j           = 167, /* void, 4 args by value                             */
    HCALL_RET_1A_3V_b         = 168, /* returns obj, arg1 by addr, args 2-4 by value      */
    HCALL_RET_4V_d            = 169, /* returns obj, 4 args by value                      */
};

#endif /* ENUMERATIONS_H_ */
