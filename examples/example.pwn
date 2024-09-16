#define MAX_PLAYERS (5)

#include <open.mp>
#include <sscanf2>
#include <mysql>

// Y
#include <YSI_Visual\y_commands>

// T
#include <T\T>

#define HEX_COLOR_LENGTH (7)

static enum E_TEAM_DATA {
    E_TEAM_NAME[MAX_TEAM_NAME],
    E_TEAM_ABBR[MAX_TEAM_ABBREVIATION],
    E_TEAM_COLOR[HEX_COLOR_LENGTH],
    E_TEAM_MAX_MEMBERS
};

static enum E_HOOD_DATA {
    Float:E_HOOD_MIN_X,
    Float:E_HOOD_MIN_Y,
    Float:E_HOOD_MAX_X,
    Float:E_HOOD_MAX_Y
};

static
    DBID:gUserDBID[MAX_PLAYERS],
    DBID:gTeamDBID[MAX_TEAMS],
    DBID:gHoodDBID[MAX_HOODS],
    DBID:gTeamRankDBID[MAX_TEAMS][MAX_TEAM_RANKS],
    DBID:gTeamMemberDBID[MAX_TEAMS][MAX_TEAM_MEMBERS],
    DBID:gTeamMemberUserDBID[MAX_TEAMS][MAX_TEAM_MEMBERS],
    DBID:gTeamMemberRankDBID[MAX_TEAMS][MAX_TEAM_MEMBERS]
;

forward OnTeamRetrieve();
forward OnHoodRetrieve(Team:teamid);
forward OnRankRetrieve(Team:teamid);
forward OnMemberRetrieve(Team:teamid);

main(){}

/**
 * # Functions
 */

FetchTeam(playerid) {
    foreach (new Team:teamid : Team) {
        foreach (new TeamMember:memberid : TeamMember[teamid]) {
            if (gTeamMemberUserDBID[teamid][memberid] == gUserDBID[playerid]) {
                SetTeamMemberPlayer(teamid, memberid, playerid);
                break;
            }
        }
    }
}

/**
 * # Calls
 */

public OnGameModeInit() {
    // Connection

    mysql_connect("localhost", "root", "", "t");

    // Retrieve

    mysql_tquery(MYSQL_DEFAULT_HANDLE, "SELECT * FROM `teams`;", "OnTeamRetrieve");
    mysql_tquery(MYSQL_DEFAULT_HANDLE, "SELECT * FROM `hoods` WHERE `team_id` IS NULL;", "OnHoodRetrieve", "i", _:INVALID_TEAM_ID);

    return 1;
}

public OnPlayerSpawn(playerid) {
    // Show Hoods

    ShowHoodsForPlayer(playerid);

    // Load Team

    FetchTeam(playerid);
    
    return 1;
}

public OnTeamRetrieve() {
    new const
        count = cache_num_rows()
    ;

    if (!count) {
        return print("Number of teams loaded: 0");
    }

    new
        data[E_TEAM_DATA],
        query[256],
        Team:id,
        color
    ;

    for (new i; i < count; ++i) {
        cache_get_value(i, "name", data[E_TEAM_NAME]);
        cache_get_value(i, "abbreviation", data[E_TEAM_ABBR]);
        cache_get_value(i, "color", data[E_TEAM_COLOR]);
        cache_get_value_int(i, "max_members", data[E_TEAM_MAX_MEMBERS]);

        sscanf(data[E_TEAM_COLOR], "m", color);

        id = CreateTeam(
            data[E_TEAM_NAME],
            data[E_TEAM_ABBR],
            color,
            data[E_TEAM_MAX_MEMBERS]
        );

        if (id == INVALID_TEAM_ID) {
            break;
        }

        cache_get_value_int(i, "id", _:gTeamDBID[id]);

        mysql_format(MYSQL_DEFAULT_HANDLE, query, sizeof (query), "SELECT * FROM `hoods` WHERE `team_id` = %i;", _:gTeamDBID[id]);
        mysql_tquery(MYSQL_DEFAULT_HANDLE, query, "OnHoodRetrieve", "i", _:id);

        mysql_format(MYSQL_DEFAULT_HANDLE, query, sizeof (query), "SELECT * FROM `ranks` WHERE `team_id` = %i;", _:gTeamDBID[id]);
        mysql_tquery(MYSQL_DEFAULT_HANDLE, query, "OnRankRetrieve", "i", _:id);

        mysql_format(MYSQL_DEFAULT_HANDLE, query, sizeof (query), "\
            SELECT \
                `m`.*, \
                `u`.`name` \
            FROM \
                `members` AS `m` \
            JOIN \
                `users` AS `u` ON `m`.`user_id` = `u`.`id` \
            JOIN \
                `ranks` AS `r` ON `m`.`rank_id` = `r`.`id` \
            JOIN \
                `teams` AS `t` ON `m`.`team_id` = `t`.`id` \
            WHERE \
                `t`.`id` = %i;", _:gTeamDBID[id]
        );

        mysql_tquery(MYSQL_DEFAULT_HANDLE, query, "OnMemberRetrieve", "i", _:id);
    }

    return 1;
}

public OnHoodRetrieve(Team:teamid) {
    new const
        count = cache_num_rows()
    ;

    if (!count) {
        return print("Number of hoods loaded: 0");
    }

    new
        data[E_HOOD_DATA],
        Hood:id
    ;

    for (new i; i < count; ++i) {
        cache_get_value_float(i, "min_x", data[E_HOOD_MIN_X]);
        cache_get_value_float(i, "min_y", data[E_HOOD_MIN_Y]);
        cache_get_value_float(i, "max_x", data[E_HOOD_MAX_X]);
        cache_get_value_float(i, "max_y", data[E_HOOD_MAX_Y]);

        id = CreateHood(
            data[E_HOOD_MIN_X],
            data[E_HOOD_MIN_Y],
            data[E_HOOD_MAX_X],
            data[E_HOOD_MAX_Y]
        );

        if (id == INVALID_HOOD_ID) {
            break;
        }

        cache_get_value_int(i, "id", _:gHoodDBID[id]);

        if (teamid == INVALID_TEAM_ID) {
            continue;
        }

        SetHoodTeam(id, teamid);
    }

    return 1;
}

public OnRankRetrieve(Team:teamid) {
    new const
        count = cache_num_rows()
    ;

    if (!count) {
        return print("Number of ranks loaded: 0");
    }

    new
        name[MAX_TEAM_RANK_NAME + 1],
        TeamRank:id
    ;

    for (new i; i < count; ++i) {
        cache_get_value(i, "name", name);

        if ((id = AddTeamRank(teamid, name)) == INVALID_TEAM_RANK_ID) {
            break;
        }

        cache_get_value_int(i, "id", _:gTeamRankDBID[teamid][id]);
    }

    return 1;
}

public OnMemberRetrieve(Team:teamid) {
    new const
        count = cache_num_rows()
    ;

    if (!count) {
        return print("Number of members loaded: 0");
    }

    new
        name[MAX_PLAYER_NAME + 1],
        TeamMember:id
    ;

    for (new i; i < count; ++i) {
        cache_get_value(i, "name", name);

        if ((id = AddTeamMember(teamid, .name = name)) == INVALID_TEAM_MEMBER_ID) {
            break;
        }

        cache_get_value_int(i, "id", _:gTeamMemberDBID[teamid][id]);
        cache_get_value_int(i, "user_id", _:gTeamMemberUserDBID[teamid][id]);
        cache_get_value_int(i, "rank_id", _:gTeamMemberRankDBID[teamid][id]);

        foreach (new TeamRank:rankid : TeamRank[teamid]) {
            if (gTeamRankDBID[teamid][rankid] == gTeamMemberRankDBID[teamid][id]) {
                SetTeamMemberRank(teamid, id, rankid);
                break;
            }
        }
    }

    return 1;
}