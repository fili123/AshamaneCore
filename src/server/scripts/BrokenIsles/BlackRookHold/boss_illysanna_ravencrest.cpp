/*
 * Copyright (C) 2017-2018 AshamaneProject <https://github.com/AshamaneProject>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ScriptMgr.h"
#include "black_rook_hold.h"

enum Phases
{
    PHASE_VENGEANCE     = 1,
    PHASE_FURY          = 2,
};

enum Spells
{
    // Phase 1 - Vengeance
    SPELL_VENGEFUL_SHEAR        = 197418,

    SPELL_DARK_RUSH             = 197478,
    SPELL_BLAZING_TRAIL         = 197521,

    SPELL_BRUTAL_GLAIVE         = 197546,

    // Phase 2 - Fury
    SPELL_EYE_BEAMS             = 197696,
    SPELL_FELBLAZED_GROUND      = 197821,

    // Soul-torn Vanguard (100485)
    SPELL_BONECRUSHING_STRIKE   = 197974,

    // Risen Arcanist (100486)
    SPELL_ARCANE_BLITZ          = 197797,
};

// 98696
struct boss_illysanna_ravencrest : public BossAI
{
    boss_illysanna_ravencrest(Creature* creature) : BossAI(creature, DATA_ILLYSANNA_RAVENCREST) { }

    void ScheduleTasks() override
    {
        events.ScheduleEvent(SPELL_VENGEFUL_SHEAR,  16s,        0, PHASE_VENGEANCE);
        events.ScheduleEvent(SPELL_DARK_RUSH,       10s, 20s,   0, PHASE_VENGEANCE);
        events.ScheduleEvent(SPELL_BRUTAL_GLAIVE,   12s,        0, PHASE_VENGEANCE);

        events.ScheduleEvent(SPELL_EYE_BEAMS,       12s,        0, PHASE_FURY);
    }

    void ExecuteEvent(uint32 eventId) override
    {
        switch (eventId)
        {
            case SPELL_VENGEFUL_SHEAR:
            {
                DoCast(SPELL_VENGEFUL_SHEAR);
                events.Repeat(16s);
                break;
            }
            case SPELL_DARK_RUSH:
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1))
                    DoCast(target, SPELL_DARK_RUSH);

                events.Repeat(10s, 20s);
                break;
            }
            case SPELL_BRUTAL_GLAIVE:
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 1))
                    DoCast(target, SPELL_BRUTAL_GLAIVE);

                events.Repeat(12s);
                break;
            }
            case SPELL_EYE_BEAMS:
            {
                if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
                    DoCast(target, SPELL_EYE_BEAMS);

                events.Repeat(10s, 20s);
                break;
            }
            default:
                break;
        }
    }
};

// 100485
struct npc_soultorn_vanguard : public ScriptedAI
{
    npc_soultorn_vanguard(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        me->GetScheduler().Schedule(5s, 10s, [](TaskContext context)
        {
            GetContextUnit()->CastSpell(nullptr, SPELL_BONECRUSHING_STRIKE, false);
            context.Repeat();
        });
    }
};

// 100486
struct npc_risen_arcanist : public ScriptedAI
{
    npc_risen_arcanist(Creature* creature) : ScriptedAI(creature) { }

    void UpdateAI(uint32 /*diff*/) override
    {
        if (!UpdateVictim())
            return;

        DoSpellAttackIfReady(SPELL_ARCANE_BLITZ);
    }
};

void AddSC_boss_illysanna_ravencrest()
{
    RegisterCreatureAI(boss_illysanna_ravencrest);
    RegisterCreatureAI(npc_soultorn_vanguard);
    RegisterCreatureAI(npc_risen_arcanist);
}
