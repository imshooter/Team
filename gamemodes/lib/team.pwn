#if defined _INC_TEAM_
    #endinput
#endif
#define _INC_TEAM_

#if !defined MAX_TEAMS
    #define MAX_TEAMS (Team:128)
#endif

#if !defined MAX_TEAM_MEMBERS
    #define MAX_TEAM_MEMBERS (32)
#endif

#if !defined MAX_TEAM_NAME
    #define MAX_TEAM_NAME (32)
#endif

#if !defined MAX_TEAM_ABBREVIATION
    #define MAX_TEAM_ABBREVIATION (4)
#endif

#define INVALID_TEAM_ID (Team:-1)
#define INVALID_TEAM_MEMBER_ID (-1)

static enum E_TEAM_DATA {
    E_TEAM_NAME[MAX_TEAM_NAME + 1],
    E_TEAM_ABBR[MAX_TEAM_ABBREVIATION + 1],
    E_TEAM_COLOR,
    E_TEAM_MAX_MEMBERS
};

static enum E_TEAM_MEMBER_DATA {
    E_TEAM_MEMBER_NAME[MAX_PLAYER_NAME + 1],
    E_TEAM_MEMBER_PLAYER_ID
};

static
    gTeamData[MAX_TEAMS][E_TEAM_DATA],
    gTeamMemberData[MAX_TEAMS][MAX_TEAM_MEMBERS][E_TEAM_MEMBER_DATA]
;

const static
    TEAM_ITER_SIZE = _:MAX_TEAMS
;

new
    Iterator:Team<Team:TEAM_ITER_SIZE>,
    Iterator:TeamMember[MAX_TEAMS]<MAX_TEAM_MEMBERS>
;

/**
 * # Events
 */

forward OnTeamCreate(Team:teamid);

/**
 * # API
 */

stock Team:CreateTeam(const name[], const abbreviation[], color, maxMembers = MAX_TEAM_MEMBERS) {
    if (!(1 <= maxMembers <= MAX_TEAM_MEMBERS)) {
        return INVALID_TEAM_ID;
    }

    new const
        Team:teamid = Team:Iter_Alloc(Team)
    ;

    if (_:teamid == cellmin) {
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
    return (0 <= _:teamid < TEAM_ITER_SIZE) && Iter_Contains(Team, teamid);
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

stock AddTeamMember(Team:teamid, playerid = INVALID_PLAYER_ID, const name[] = "") {
    if (!IsValidTeam(teamid)) {
        return INVALID_TEAM_MEMBER_ID;
    }

    if (Iter_Count(TeamMember[teamid]) >= gTeamData[teamid][E_TEAM_MAX_MEMBERS]) {
        return INVALID_TEAM_MEMBER_ID;
    }

    new const
        index = Iter_Alloc(TeamMember[teamid])
    ;

    if (!GetPlayerName(playerid, gTeamMemberData[teamid][index][E_TEAM_MEMBER_NAME])) {
        strcopy(gTeamMemberData[teamid][index][E_TEAM_MEMBER_NAME], name);
    } else {
        gTeamMemberData[teamid][index][E_TEAM_MEMBER_PLAYER_ID] = playerid;
    }

    return index;
}

stock bool:IsValidTeamMember(Team:teamid, memberid) {
    if (!IsValidTeam(teamid)) {
        return false;
    }

    if (!(0 <= memberid < MAX_TEAM_MEMBERS)) {
        return false;
    }    

    return Iter_Contains(TeamMember[teamid], memberid);
}

stock GetTeamMemberName(Team:teamid, memberid, name[], size = sizeof (name)) {
    if (!IsValidTeamMember(teamid, memberid)) {
        return false;
    }

    strcopy(name, gTeamMemberData[teamid][memberid][E_TEAM_MEMBER_NAME], size);

    return true;
}