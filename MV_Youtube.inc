#if defined MV_Youtube_Included
	#endinput
#endif
#define MV_Youtube_Included

#include <a_samp>
#include <requests>
#tryinclude <map>

#if !defined MAX_YOUTUBE_SAVES
	#define MAX_YOUTUBE_SAVES	50
#endif 

#if !defined CONVERTER_PATH
	#define CONVERTER_PATH		"https://youtube.michaelbelgium.me/api/converter/"
#endif 

#if !defined MAX_SEARCH_RESULTS
	#define MAX_SEARCH_RESULTS 	5
#endif

#if !defined API_KEY
	#define API_KEY 	""
#endif

#if !defined MAX_PLAYLISTS
	#define MAX_PLAYLISTS		5
#endif

#define INVALID_YT_ID		-1
#define INVALID_PLAYLIST_ID	-1

static RequestsClient:Converter;

#if defined __MAP_INCLUDED__
	static Map:LoadRequestToPlayerID;
	static Map:LoadRequestToYoutubeID;
#else
	static Request:PlayerIDToRequest[MAX_PLAYERS] = {Request:-1, ...};
#endif

enum e_ytv
{
	#if !defined __MAP_INCLUDED__
		Request:RequestID,
	#endif

	bool:Playing,
	ytID[16],
	Duration,
	Title[256],
	Link[256],
	StreamLink[256],
	PlayFor,
	PlaylistID,
	Float:Distance,
	bool:UsePos,
	Timer
};

new Youtube[MAX_YOUTUBE_SAVES][e_ytv];

enum e_plist
{
    bool:Active,
    Name[32]
};

new Playlist[MAX_PLAYLISTS][e_plist];

enum pSearchResult
{
	Title[64],
	Link[128]
};

new SearchResults[MAX_PLAYERS][MAX_SEARCH_RESULTS][pSearchResult];

static stock EncodeYoutubeURL(url[], maxlength = sizeof(url))
{
	new const SEARCH[] = ":", REPLACEMENT[] = "%3A", SUB_LEN = strlen(SEARCH), REP_LEN = strlen(REPLACEMENT);
    new pos, len = strlen(url), count;
    
    while(-1 != (pos = strfind(url, SEARCH, false, pos))) {
        strdel(url, pos, pos + SUB_LEN);
        
        len -= SUB_LEN;
        
        if(REP_LEN && len + REP_LEN < maxlength) {
            strins(url, REPLACEMENT, pos, maxlength);
            
            pos += REP_LEN;
            len += REP_LEN;
        }
    }
    
    return count;
}

static stock InitYoutube()
{
	for(new i = 0; i < MAX_YOUTUBE_SAVES; i++)
	{
		#if !defined __MAP_INCLUDED__
			Youtube[i][RequestID] = Request:-1;
		#endif

		Youtube[i][PlaylistID] = INVALID_PLAYLIST_ID;
	}

	Converter = RequestsClient(CONVERTER_PATH, RequestHeaders("Authorization", "Bearer "API_KEY""));
}

forward OnYoutubeVideoFinished(youtubeid);
forward SongFinished(youtubeid);
forward OnVpsResponse(Request:id, E_HTTP_STATUS:status, Node:node);
forward OnSearchResponse(Request:id, E_HTTP_STATUS:status, Node:node);
forward OnMVYoutubeError(youtubeid, const message[]);
forward OnYoutubeVideoStart(youtubeid);
forward OnPlaylistAddEntry(playerlistid, youtubeid);
forward OnPlaylistFinished(playlistid);
forward OnYoutubeSearch(playerid);

stock IsValidYoutubeURL(const string[])		return (strfind(string,"youtube.com") != -1 && strfind(string,"watch?v=") != -1);
stock IsYouTubeVideoPlaying(youtubeid)		return youtubeid == INVALID_YT_ID ? false : Youtube[youtubeid][Playing];
stock GetPlaylistFromVideo(youtubeid)		return youtubeid == INVALID_YT_ID ? INVALID_PLAYLIST_ID : Youtube[youtubeid][PlaylistID];
stock GetVideoDuration(youtubeid)			return youtubeid == INVALID_YT_ID ? 0 : Youtube[youtubeid][Duration];
stock GetVideoTarget(youtubeid)				return youtubeid == INVALID_YT_ID ? -1 : Youtube[youtubeid][PlayFor];

stock PlayYoutubeVideoFor(const url[], playfor = INVALID_PLAYER_ID, playlist = INVALID_PLAYLIST_ID, bool:usepos = false, Float:distance = 50.0)
{
	new id = GetFreeIndex(), requestlink[512];

	if(id != INVALID_YT_ID)
	{
		Youtube[id][PlayFor] = playfor;
		Youtube[id][Distance] = distance;
		Youtube[id][UsePos] = usepos;
		Youtube[id][PlaylistID] = playlist;

		format(Youtube[id][Link],256,"%s",url);

		format(requestlink,sizeof(requestlink),"convert?url=%s",Youtube[id][Link]);
		EncodeYoutubeURL(requestlink);
		
		new const Request:ID = RequestJSON(Converter, requestlink, HTTP_METHOD_GET, "OnVpsResponse", .headers = RequestHeaders());

		#if defined __MAP_INCLUDED__
			MAP_insert_val_val(LoadRequestToYoutubeID, _:ID, id);
		#else
			Youtube[id][RequestID] = ID;
		#endif
	}
	return id;
}

stock SearchYoutubeVideos(playerid, const searchquery[])
{
	new requestlink[512];
	format(requestlink, sizeof(requestlink), "search?q=%s&max_results=%i", searchquery, MAX_SEARCH_RESULTS);
	EncodeYoutubeURL(requestlink);

	new const Request:ID = RequestJSON(Converter, requestlink, HTTP_METHOD_GET, "OnSearchResponse", .headers = RequestHeaders());
	#if defined __MAP_INCLUDED__
		MAP_insert_val_val(LoadRequestToPlayerID, _:ID, playerid);
	#else
		PlayerIDToRequest[playerid] = ID;
	#endif
}

stock GetFreeIndex()
{
	for(new i = 0; i < MAX_YOUTUBE_SAVES; i++)
	{
		if(!Youtube[i][Playing] && Youtube[i][PlaylistID] == INVALID_PLAYLIST_ID) return i;
	}

	print("[Warning] MV_Youtube: MAX_YOUTUBE_SAVES reached!");
	return INVALID_YT_ID;
}

stock StopYoutubeVideo(youtubeid)
{
	Youtube[youtubeid][Playing] = false;
	KillTimer(Youtube[youtubeid][Timer]);

	if(Youtube[youtubeid][PlayFor] == INVALID_PLAYER_ID)
	{
		#if defined _INC_open_mp
			for (new i = 0; i < MAX_PLAYERS; i++)
		#else
			for(new i = 0, j = GetPlayerPoolSize(); i <= j; i++)
		#endif
		{
			if(!IsPlayerConnected(i)) continue;
			StopAudioStreamForPlayer(i);
		}
	}
	else
		StopAudioStreamForPlayer(Youtube[youtubeid][PlayFor]);
}

stock GetVideoTitle(youtubeid)
{
    new str[256];
    strins(str, Youtube[youtubeid][Title], 0);
    return str;
}

stock GetVideoLink(youtubeid)
{
    new str[256];
    strins(str, Youtube[youtubeid][Link], 0);
    return str;
}

stock GetVideoStreamLink(youtubeid)
{
	new str[256];
	strins(str, Youtube[youtubeid][StreamLink], 0);
	return str;
}

stock CreatePlaylist(const name[])
{
	for(new i = 0; i < MAX_PLAYLISTS; i++)
	{
		if(Playlist[i][Active]) continue;
		Playlist[i][Active] = true;
		format(Playlist[i][Name], 32, "%s", name);
		return i;
	}

	print("[Warning] MV_Youtube: MAX_PLAYLISTS reached!");
	return INVALID_PLAYLIST_ID;
}

stock RemovePlaylist(playlistid)
{
	if(playlistid != INVALID_PLAYLIST_ID)
	{
		Playlist[playlistid][Active] = false;
		Playlist[playlistid][Name] = EOS;

		for(new i = 0; i < MAX_YOUTUBE_SAVES; i++)
			if(Youtube[i][PlaylistID] == playlistid)
				Youtube[i][PlaylistID] = INVALID_PLAYLIST_ID;
	}
}

stock GetPlaylistName(playlistid)
{
	new str[32];
	strins(str, Playlist[playlistid][Name], 0);
	return str;
}

stock GetPlaylistSongs(playlistid)
{
	new songs[MAX_YOUTUBE_SAVES], pointer = 0;

	for(new i = 0; i < MAX_YOUTUBE_SAVES; i++) songs[i] = INVALID_YT_ID;
	for(new i = 0; i < MAX_YOUTUBE_SAVES; i++)
	{
		if(Youtube[i][PlaylistID] != playlistid) continue;
		songs[pointer] = i;
		pointer++;
	}
	return songs;
}

stock GetPlaylistSongsCount(playlistid)
{
	new count = 0;

	for(new i = 0; i < MAX_YOUTUBE_SAVES; i++)
	{
		if(Youtube[i][PlaylistID] != playlistid) continue;
		count++;
	}

	return count;
}

PlayYoutubeVideo(youtubeid) //shouldn't use this function outside of the include.
{
	new Float:pos[3];

	Youtube[youtubeid][Playing] = true;

	CallLocalFunction("OnYoutubeVideoStart", "i", youtubeid);

	if(Youtube[youtubeid][PlayFor] == INVALID_PLAYER_ID)
	{
		#if defined _INC_open_mp
			for (new i = 0; i < MAX_PLAYERS; i++)
		#else
			for(new i = 0, j = GetPlayerPoolSize(); i <= j; i++)
		#endif
		{
			if(IsPlayerConnected(i))
			{
				if(Youtube[youtubeid][UsePos]) GetPlayerPos(i, pos[0], pos[1], pos[2]);
				PlayAudioStreamForPlayer(i, Youtube[youtubeid][StreamLink], pos[0], pos[1], pos[2], Youtube[youtubeid][Distance], Youtube[youtubeid][UsePos]);
			}
		}
	}
	else
	{
		if(Youtube[youtubeid][UsePos]) GetPlayerPos(Youtube[youtubeid][PlayFor], pos[0], pos[1], pos[2]);
		PlayAudioStreamForPlayer(Youtube[youtubeid][PlayFor], Youtube[youtubeid][StreamLink], pos[0], pos[1], pos[2], Youtube[youtubeid][Distance], Youtube[youtubeid][UsePos]);
	}
	
	Youtube[youtubeid][Timer] = SetTimerEx("SongFinished",GetVideoDuration(youtubeid)*1000,false,"i",youtubeid);
}

public OnVpsResponse(Request:id, E_HTTP_STATUS:status, Node:node)
{
	#if defined __MAP_INCLUDED__
		new index = MAP_get_val_val(LoadRequestToYoutubeID, _:id);
		MAP_remove_val(LoadRequestToYoutubeID, _:id);
	#else
		new index = -1;

		for(new i = 0; i < sizeof(Youtube); i++)
		{
			if(Youtube[i][RequestID] == id)
			{
				Youtube[i][RequestID] = Request:-1;
				index = i;
				break;
			}
		}
	#endif

	new string[256];

	if(status != HTTP_STATUS_OK)
	{
		format(string, sizeof(string), "%s - response code: %i", GetError(status), _:status);
		CallLocalFunction("OnMVYoutubeError", "is", index, string);
		return 0;
	}

	new bool:error;
	JsonGetBool(node, "error", error);

	if(error)
	{
		JsonGetString(node, "message", string);
		CallLocalFunction("OnMVYoutubeError", "is", index, string);
		return 0;
	}

	JsonGetString(node, "title", Youtube[index][Title]);
	JsonGetInt(node, "duration", Youtube[index][Duration]);
	JsonGetInt(node, "youtube_id", Youtube[index][ytID]);
	JsonGetString(node, "file", Youtube[index][StreamLink]);

	if(Youtube[index][PlaylistID] != INVALID_PLAYLIST_ID)
	{
		CallLocalFunction("OnPlaylistAddEntry", "ii", Youtube[index][PlaylistID], index);

		if(GetPlaylistSongsCount(Youtube[index][PlaylistID]) != 1)
			return 0;
	}

	PlayYoutubeVideo(index);
	return 1;
}

public OnSearchResponse(Request:id, E_HTTP_STATUS:status, Node:node)
{
	#if defined __MAP_INCLUDED__
		new index = MAP_get_val_val(LoadRequestToPlayerID, _:id);
		MAP_remove_val(LoadRequestToPlayerID, _:id);
	#else
		new index = -1;

		for(new i = 0; i < sizeof(PlayerIDToRequest); i++)
		{
			if(PlayerIDToRequest[i] == id)
			{
				PlayerIDToRequest[i] = Request:-1;
				index = i;
				break;
			}
		}
	#endif
	
	new string[256];

	if(status != HTTP_STATUS_OK)
	{
		format(string, sizeof(string), "%s - response code: %i", GetError(status), _:status);
		CallLocalFunction("OnMVYoutubeError", "is", index, string);
		return 0;
	}

	new bool:error;
	JsonGetBool(node, "error", error);

	if(error)
	{
		JsonGetString(node, "message", string);
		CallLocalFunction("OnMVYoutubeError", "is", index, string);
		return 0;
	}

	JsonToggleGC(node, false);

	new Node:results, length;

	JsonGetArray(node, "results", results);
	JsonArrayLength(results, length);

	for(new i = 0; i < length; i++)
	{
		new Node:result;
		JsonArrayObject(results, i, result);

		JsonGetString(result, "title", SearchResults[index][i][Title]);
		JsonGetString(result, "full_link", SearchResults[index][i][Link]);
	}

	JsonToggleGC(node, true);

	CallLocalFunction("OnYoutubeSearch", "i", index);
	return 1;
}

public SongFinished(youtubeid)
{
	StopYoutubeVideo(youtubeid);

	CallLocalFunction("OnYoutubeVideoFinished","i",youtubeid);

	if(Youtube[youtubeid][PlaylistID] != INVALID_PLAYLIST_ID)
	{
		new plist = Youtube[youtubeid][PlaylistID], bool:finished = true;
		Youtube[youtubeid][PlaylistID] = INVALID_PLAYLIST_ID;

		for(new i = 0; i < MAX_YOUTUBE_SAVES; i++)
		{
			if(Youtube[i][PlaylistID] != plist) continue;
			PlayYoutubeVideo(i);
			finished = false;
			break;
		}

		if(finished)
			CallLocalFunction("OnPlaylistFinished", "i", plist);
	}
}

stock GetError({_, E_HTTP_STATUS}:val)
{
	new error[32];
	switch(val)
	{
		case 1: error = "Bad host";
		case 2: error = "No socket";
		case 3: error = "Can't connect";
		case 4: error = "Can't write";
		case 5: error = "Content too big";
		case 6: error = "Malformed response";
		case (_:HTTP_STATUS_MULTIPLE_CHOICES)..(_:HTTP_STATUS_PERMANENT_REDIRECT): error = "Redirection";
		case (_:HTTP_STATUS_BAD_REQUEST)..(_:HTTP_STATUS_LEGAL_FAILURE): error = "Client error";
		case (_:HTTP_STATUS_SERVER_ERROR)..(_:HTTP_STATUS_AUTH_REQUIRED): error = "Server error";
	}
	return error;
}

#if defined FILTERSCRIPT
	public OnGameModeInit()
	{
		InitYoutube();

		#if defined MV_OnGameModeInit
			return MV_OnGameModeInit();
		#else
			return 1;
		#endif
	}

	#if defined _ALS_OnGameModeInit
		#undef OnGameModeInit
	#else
		#define _ALS_OnGameModeInit
	#endif

	#define OnGameModeInit MV_OnGameModeInit
	#if defined MV_OnGameModeInit
		forward MV_OnGameModeInit();
	#endif
#else
	public OnGameModeInit()
	{
		InitYoutube();

		#if defined MV_OnGameModeInit
			return MV_OnGameModeInit();
		#else
			return 1;
		#endif
	}

	#if defined _ALS_OnGameModeInit
		#undef OnGameModeInit
	#else
		#define _ALS_OnGameModeInit
	#endif

	#define OnGameModeInit MV_OnGameModeInit
	#if defined MV_OnGameModeInit
		forward MV_OnGameModeInit();
	#endif
#endif

public OnRequestFailure(Request:id, errorCode, errorMessage[], len)
{
	#if defined MV_OnRequestFailure
		MV_OnRequestFailure(id, errorCode, errorMessage, len);
	#endif
}

#if defined _ALS_OnRequestFailure
	#undef OnRequestFailure
#else
	#define _ALS_OnRequestFailure
#endif

#define OnRequestFailure MV_OnRequestFailure
#if defined MV_OnRequestFailure
	forward MV_OnRequestFailure(Request:id, errorCode, errorMessage[], len);
#endif
