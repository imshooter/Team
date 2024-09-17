#if defined _INC_T_TEAM
    #endinput
#endif
#define _INC_T_TEAM

#include <YSI_Data\y_iterate>
#include <YSI_Coding\y_hooks>

/**
 * # Header
 */

#if !defined MAX_TEAMS
    #define MAX_TEAMS (Team:32)
#endif

#if !defined MAX_TEAM_RANKS
    #define MAX_TEAM_RANKS (TeamRank:32)
#endif

#if !defined MAX_TEAM_MEMBERS
    #define MAX_TEAM_MEMBERS (TeamMember:32)
#endif

#if !defined MAX_TEAM_NAME
    #define MAX_TEAM_NAME (32)
#endif

#if !defined MAX_TEAM_RANK_NAME
    #define MAX_TEAM_RANK_NAME (32)
#endif

#if !defined MAX_TEAM_ABBREVIATION
    #define MAX_TEAM_ABBREVIATION (4)
#endif

#define INVALID_TEAM_ID (Team:-1)
#define INVALID_TEAM_RANK_ID (TeamRank:-1)
#define INVALID_TEAM_MEMBER_ID (TeamMember:-1)

static enum E_TEAM_DATA {
    E_TEAM_NAME[MAX_TEAM_NAME + 1],
    E_TEAM_ABBR[MAX_TEAM_ABBREVIATION + 1],
    E_TEAM_COLOR,
    E_TEAM_MAX_MEMBERS
};

static enum E_TEAM_RANK_DATA {
    E_TEAM_RANK_NAME[MAX_TEAM_RANK_NAME + 1]
};

static enum E_TEAM_MEMBER_DATA {
    TeamRank:E_TEAM_MEMBER_RANK_ID,
    E_TEAM_MEMBER_PLAYER_ID,
    E_TEAM_MEMBER_NAME[MAX_PLAYER_NAME + 1]
};

static
    gTeamData[MAX_TEAMS][E_TEAM_DATA],
    gTeamRankData[MAX_TEAMS][MAX_TEAM_RANKS][E_TEAM_RANK_DATA],
    gTeamMemberData[MAX_TEAMS][MAX_TEAM_MEMBERS][E_TEAM_MEMBER_DATA]
;

const static
    TEAM_ITER_SIZE        = _:MAX_TEAMS,
    TEAM_RANK_ITER_SIZE   = _:MAX_TEAM_RANKS,
    TEAM_MEMBER_ITER_SIZE = _:MAX_TEAM_MEMBERS
;

new stock
    Iterator:Team<Team:TEAM_ITER_SIZE>,
    Iterator:TeamRank[MAX_TEAMS]<TeamRank:TEAM_RANK_ITER_SIZE>,
    Iterator:TeamMember[MAX_TEAMS]<TeamMember:TEAM_MEMBER_ITER_SIZE>
;

static
    Team:gInTeamID[MAX_PLAYERS] = { INVALID_TEAM_ID, ... }
;

/**
 * # Functions
 */

stock Team:CreateTeam(const name[], const abbreviation[], color, maxMembers = _:MAX_TEAM_MEMBERS);
stock bool:IsValidTeam(Team:teamid);
stock bool:SetTeamName(Team:teamid, const name[]);
stock bool:GetTeamName(Team:teamid, name[], size = sizeof (name));
stock bool:SetTeamAbbreviation(Team:teamid, const abbreviation[]);
stock bool:GetTeamAbbreviation(Team:teamid, abbreviation[], size = sizeof (abbreviation));
stock bool:SetTeamColor(Team:teamid, color);
stock GetTeamColor(Team:teamid);
stock bool:SetTeamMaxMembers(Team:teamid, maxMembers);
stock GetTeamMaxMembers(Team:teamid);

stock TeamRank:AddTeamRank(Team:teamid, const name[]);
stock bool:IsValidTeamRank(Team:teamid, TeamRank:rankid);
stock bool:GetTeamRankName(Team:teamid, TeamRank:rankid, name[], size = sizeof (name));

stock TeamMember:AddTeamMember(Team:teamid, playerid = INVALID_PLAYER_ID, const name[] = "");
stock bool:IsValidTeamMember(Team:teamid, TeamMember:memberid);
stock bool:SetTeamMemberRank(Team:teamid, TeamMember:memberid, TeamRank:rankid);
stock bool:GetTeamMemberRank(Team:teamid, TeamMember:memberid, &TeamRank:rankid);
stock bool:SetTeamMemberPlayer(Team:teamid, TeamMember:memberid, playerid);
stock GetTeamMemberPlayer(Team:teamid, TeamMember:memberid);
stock bool:GetTeamMemberName(Team:teamid, TeamMember:memberid, name[], size = sizeof (name));

stock bool:IsPlayerInAnyTeam(playerid);
stock Team:GetPlayerTeamID(playerid, &Team:teamid = INVALID_TEAM_ID);

/**
 * # Events
 */

forward OnTeamCreate(Team:teamid);
forward OnTeamRankAdd(Team:teamid, TeamRank:rankid);
forward OnTeamMemberAdd(Team:teamid, TeamMember:memberid);

/**
 * # API
 */

stock Team:CreateTeam(const name[], const abbreviation[], color, maxMembers = _:MAX_TEAM_MEMBERS) {
    if (!(1 <= maxMembers <= TEAM_MEMBER_ITER_SIZE)) {
        return INVALID_TEAM_ID;
    }

    new const
        Team:teamid = Team:Iter_Alloc(Team)
    ;

    if (_:teamid == INVALID_ITERATOR_SLOT) {
        return INVALID_TEAM_ID;
    }

    strcopy(gTeamData[teamid][E_TEAM_NAME], name);
    strcopy(gTeamData[teamid][E_TEAM_ABBR], abbreviation);
    gTeamData[teamid][E_TEAM_COLOR] = color;
    gTeamData[teamid][E_TEAM_MAX_MEMBERS] = maxMembers;

    CallLocalFunction("OnTeamCreate", "i", _:teamid);

    return teamid;
}

stock bool:IsValidTeam(Team:teamid) {
    if (!(0 <= _:teamid < TEAM_ITER_SIZE)) {
        return false;
    }

    return Iter_Contains(Team, teamid);
}

stock bool:SetTeamName(Team:teamid, const name[]) {
    if (!IsValidTeam(teamid)) {
        return false;
    }

    strcopy(gTeamData[teamid][E_TEAM_NAME], name);

    return true;
}

stock bool:GetTeamName(Team:teamid, name[], size = sizeof (name)) {
    if (!IsValidTeam(teamid)) {
        return false;
    }
    
    strcopy(name, gTeamData[teamid][E_TEAM_NAME], size);

    return true;
}

stock bool:SetTeamAbbreviation(Team:teamid, const abbreviation[]) {
    if (!IsValidTeam(teamid)) {
        return false;
    }

    strcopy(gTeamData[teamid][E_TEAM_ABBR], abbreviation);

    return true;
}

stock bool:GetTeamAbbreviation(Team:teamid, abbreviation[], size = sizeof (abbreviation)) {
    if (!IsValidTeam(teamid)) {
        return false;
    }

    strcopy(abbreviation, gTeamData[teamid][E_TEAM_ABBR], size);

    return true;
}

stock bool:SetTeamColor(Team:teamid, color) {
    if (!IsValidTeam(teamid)) {
        return false;
    }
    
    gTeamData[teamid][E_TEAM_COLOR] = color;

    return true;
}

stock GetTeamColor(Team:teamid) {
    if (!IsValidTeam(teamid)) {
        return 0;
    }
    
    return gTeamData[teamid][E_TEAM_COLOR];
}

stock bool:SetTeamMaxMembers(Team:teamid, maxMembers) {
    if (!IsValidTeam(teamid)) {
        return false;
    }
    
    gTeamData[teamid][E_TEAM_MAX_MEMBERS] = maxMembers;

    return true;
}

stock GetTeamMaxMembers(Team:teamid) {
    if (!IsValidTeam(teamid)) {
        return 0;
    }

    return gTeamData[teamid][E_TEAM_MAX_MEMBERS];
}

/**
 * # Rank
 */

stock TeamRank:AddTeamRank(Team:teamid, const name[]) {
    if (!IsValidTeam(teamid)) {
        return INVALID_TEAM_RANK_ID;
    }

    new const
        TeamRank:rankid = TeamRank:Iter_Alloc(TeamRank[teamid])
    ;

    strcopy(gTeamRankData[teamid][rankid][E_TEAM_RANK_NAME], name);

    CallLocalFunction("OnTeamRankAdd", "ii", _:teamid, _:rankid);

    return rankid;
}

stock bool:IsValidTeamRank(Team:teamid, TeamRank:rankid) {
    if (!IsValidTeam(teamid)) {
        return false;
    }

    if (!(0 <= _:rankid < TEAM_RANK_ITER_SIZE)) {
        return false;
    }

    return Iter_Contains(TeamRank[teamid], rankid);
}

stock bool:GetTeamRankName(Team:teamid, TeamRank:rankid, name[], size = sizeof (name)) {
    if (!IsValidTeamRank(teamid, rankid)) {
        return false;
    }

    strcopy(name, gTeamRankData[teamid][rankid][E_TEAM_RANK_NAME], size);

    return true;
}

/**
 * # Member
 */

stock TeamMember:AddTeamMember(Team:teamid, playerid = INVALID_PLAYER_ID, const name[] = "") {
    if (!IsValidTeam(teamid)) {
        return INVALID_TEAM_MEMBER_ID;
    }

    if (Iter_Count(TeamMember[teamid]) >= gTeamData[teamid][E_TEAM_MAX_MEMBERS]) {
        return INVALID_TEAM_MEMBER_ID;
    }

    new const
        TeamMember:memberid = TeamMember:Iter_Alloc(TeamMember[teamid])
    ;

    if (GetPlayerName(playerid, gTeamMemberData[teamid][memberid][E_TEAM_MEMBER_NAME])) {
        gInTeamID[playerid] = teamid;
        gTeamMemberData[teamid][memberid][E_TEAM_MEMBER_PLAYER_ID] = playerid;
    } else {
        strcopy(gTeamMemberData[teamid][memberid][E_TEAM_MEMBER_NAME], name);
    }

    CallLocalFunction("OnTeamMemberAdd", "ii", _:teamid, _:memberid);

    return memberid;
}

stock bool:IsValidTeamMember(Team:teamid, TeamMember:memberid) {
    if (!IsValidTeam(teamid)) {
        return false;
    }

    if (!(0 <= _:memberid < TEAM_MEMBER_ITER_SIZE)) {
        return false;
    }

    return Iter_Contains(TeamMember[teamid], memberid);
}

stock bool:SetTeamMemberRank(Team:teamid, TeamMember:memberid, TeamRank:rankid) {
    if (!IsValidTeamMember(teamid, memberid)) {
        return false;
    }

    if (!IsValidTeamRank(teamid, rankid)) {
        return false;
    }

    gTeamMemberData[teamid][memberid][E_TEAM_MEMBER_RANK_ID] = rankid;

    return true;
}

stock bool:GetTeamMemberRank(Team:teamid, TeamMember:memberid, &TeamRank:rankid) {
    if (!IsValidTeamMember(teamid, memberid)) {
        return false;
    }

    rankid = gTeamMemberData[teamid][memberid][E_TEAM_MEMBER_RANK_ID];

    return true;
}

stock bool:SetTeamMemberPlayer(Team:teamid, TeamMember:memberid, playerid) {
    if (!IsValidTeamMember(teamid, memberid)) {
        return false;
    }

    if (!GetPlayerName(playerid, gTeamMemberData[teamid][memberid][E_TEAM_MEMBER_NAME])) {
        return false;
    }

    gTeamMemberData[teamid][memberid][E_TEAM_MEMBER_PLAYER_ID] = playerid;

    return true;
}

stock GetTeamMemberPlayer(Team:teamid, TeamMember:memberid) {
    if (!IsValidTeamMember(teamid, memberid)) {
        return INVALID_PLAYER_ID;
    }

    return gTeamMemberData[teamid][memberid][E_TEAM_MEMBER_PLAYER_ID];
}

stock bool:GetTeamMemberName(Team:teamid, TeamMember:memberid, name[], size = sizeof (name)) {
    if (!IsValidTeamMember(teamid, memberid)) {
        return false;
    }

    strcopy(name, gTeamMemberData[teamid][memberid][E_TEAM_MEMBER_NAME], size);

    return true;
}

stock bool:IsPlayerInAnyTeam(playerid) {
    if (!IsPlayerConnected(playerid)) {
        return false;
    }

    return (gInTeamID[playerid] != INVALID_TEAM_ID);
}

stock Team:GetPlayerTeamID(playerid, &Team:teamid = INVALID_TEAM_ID) {
    if (!IsPlayerConnected(playerid)) {
        teamid = INVALID_TEAM_ID;
        
        return teamid;
    }

    teamid = gInTeamID[playerid];

    return teamid;
}