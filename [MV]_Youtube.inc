#include <a_http>
#include <a_json>

#define USE_VERSION			1

/*
	0 = YoutubeInMP3
	1 = youtubemp3script
	2 = VPS api
*/

#define MAX_YOUTUBE_SAVES	50
#define INVALID_YT_ID		-1
#define INVALID_PLAYLIST_ID	-1
#define MAX_PLAYLISTS		5

#if USE_VERSION == 1
#define API_KEY				"12345"
#elseif USE_VERSION == 2
#define CONVERTER_PATH		"lmdm.exp-gaming.net/ucp/ytconverter"
#define REMOVE_ON_FINISHED	true 		//Remove the downloaded song when the song finished playing or not
#endif

enum e_ytv
{
	bool:Playing,
	ytID[16],
	Duration,
	Title[256],
	Link[256],
	StreamLink[256],
	PlayFor,
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

forward OnYoutubeVideoFinished(youtubeid);
forward SongFinished(youtubeid);
forward OnYoutubeResponse(index, response_code, data[]);
forward OnYoutubeDownloadError(youtubeid, message[]);

#if USE_VERSION == 0

stock PlayYoutubeVideoFor(url[], playfor = INVALID_PLAYER_ID, startat = 0, bool:usepos = false, Float:distance = 50.0)
{
	new id = GetFreeIndex(), requestlink[128];

	Youtube[id][PlayFor] = playfor;
	Youtube[id][Distance] = distance;
	Youtube[id][UsePos] = usepos;

	if(strfind(url,"&") != -1) strmid(url,url,0,strfind(url,"&"), 128);
	format(Youtube[id][Link],256,"%s",url);

	format(requestlink,sizeof(requestlink),"youtubeinmp3.com/fetch/?format=json&video=%s&start=%i",Youtube[id][Link],startat);
	HTTP(id, HTTP_GET, requestlink, "", "OnYoutubeResponse");
	return id;
}

#else

stock PlayYoutubeVideoFor(url[], playfor = INVALID_PLAYER_ID, bool:usepos = false, Float:distance = 50.0)
{
	new id = GetFreeIndex(), requestlink[128];

	Youtube[id][PlayFor] = playfor;
	Youtube[id][Distance] = distance;
	Youtube[id][UsePos] = usepos;

	if(strfind(url,"&") != -1) strmid(url,url,0,strfind(url,"&"), 128);
	format(Youtube[id][Link],256,"%s",url);

	#if USE_VERSION == 1
	format(requestlink,sizeof(requestlink),"www.yt-mp3.com/fetch?v=%s&apikey=%s",Youtube[id][Link], API_KEY);
	#else
	strmid(Youtube[id][ytID], url, strfind(url, "watch?v=", true) + strlen("watch?v="), strlen(url), 16);
	format(requestlink,sizeof(requestlink),"%s/convert.php?youtubelink=%s",CONVERTER_PATH,Youtube[id][Link]);
	#endif
	HTTP(id, HTTP_GET, requestlink, "", "OnYoutubeResponse");
	return id;
}

#endif

stock GetFreeIndex()
{
	for(new i = 0; i < MAX_YOUTUBE_SAVES; i++)
	{
		if(!Youtube[i][Playing]) return i;
	}

	return INVALID_YT_ID;
}

stock StopYoutubeVideo(youtubeid)	
{
	Youtube[youtubeid][Playing] = false;
	KillTimer(Youtube[youtubeid][Timer]);

	if(Youtube[youtubeid][PlayFor] == INVALID_PLAYER_ID)
	{
		for(new i = 0, j = GetPlayerPoolSize(); i <= j; i++)
		{
			if(!IsPlayerConnected(i)) continue;
			StopAudioStreamForPlayer(i);
		}
	}
	else
		StopAudioStreamForPlayer(Youtube[youtubeid][PlayFor]);
}

stock IsValidYoutubeURL(string[])			return (strfind(string,"youtube.com") != -1 && strfind(string,"watch?v=") != -1);
stock IsYouTubeVideoPlaying(youtubeid)		return youtubeid == INVALID_YT_ID ? false : Youtube[youtubeid][Playing];
stock GetVideoDuration(youtubeid)			return Youtube[youtubeid][Duration];
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

stock GetVideoTarget(youtubeid) return youtubeid == INVALID_YT_ID ? -1 : Youtube[youtubeid][PlayFor];

stock CreatePlaylist(name[])
{
	for(new i = 0; i < MAX_PLAYLISTS; i++)
	{
		if(Playlist[i][Active]) continue;
		Playlist[i][Active] = true;
		format(Playlist[i][Name], 32, "%s", name);
		break;
	}
	return INVALID_PLAYLIST_ID;
}

stock GetPlaylistName(playlistid)
{
	new str[32];
	strins(str, Playlist[playlistid][Name], 0);
	return str;
}

public OnYoutubeResponse(index, response_code, data[])
{
	new string[256];

	if(response_code != 200)
	{
		format(string, sizeof(string), "%s - response code: %i", GetError(response_code), response_code);
		CallLocalFunction("OnYoutubeDownloadError", "is", index, string);
		return 0;
	}

	#if USE_VERSION == 0
	if (!strcmp(data, "<meta", true, 5))
	{
		response_code = 7;
		format(string, sizeof(string), "%s - response code: %i", GetError(response_code), response_code);
		CallLocalFunction("OnYoutubeDownloadError", "is", index, string);
		return 0;
	}
	#endif

	new JSONNode:response, Float:pos[3], time[3];

	response = json_parse_string(data);

	#if USE_VERSION == 2

	if(index == 9999)
	{
		if(json_get_bool(response, "error"))
		{
			json_get_string(response, string, sizeof(string), "type");
			CallLocalFunction("OnYoutubeDownloadError", "is", index, string);
		}
		else
		{
			json_get_string(response, string, sizeof(string), "message");
			printf("Downloaded song: %s",string);
		}
		return 0;
	}

	if(json_get_bool(response, "error"))
	{
		json_get_string(response, string, sizeof(string), "type");
		CallLocalFunction("OnYoutubeDownloadError", "is", index, string);
		return 0;
	}

	json_get_string(response, Youtube[index][Title], 256, "title");
	Youtube[index][Duration] = json_get_int(response, "duration");
	json_get_string(response, Youtube[index][StreamLink], 512, "file");

	#elseif USE_VERSION == 1
	new status[8];

	json_get_string(response, status, sizeof(status), "status");

	if(!strcmp(status, "error", true))
	{
		json_get_string(response, string, sizeof(string), "message");
		CallLocalFunction("OnYoutubeDownloadError", "is", index, string);
		return 0;
	}

	json_get_string(response, Youtube[index][Title], 256, "title");
	Youtube[index][Duration] = json_get_int(response, "length");
	json_get_string(response, Youtube[index][StreamLink], 512, "url");
	strins(Youtube[index][StreamLink], "http:", 0);
	
	#else 
	new tmp_seconds[8];

	json_get_string(response, Youtube[index][Title], 256, "title");
	json_get_string(response, tmp_seconds, 8, "length");
	json_get_string(response, Youtube[index][StreamLink], 256, "link");

	Youtube[index][Duration] = strval(tmp_seconds);
	
	#endif
	json_close(response);

	Youtube[index][Playing] = true;

	formatSeconds(GetVideoDuration(index),time[0],time[1],time[2]);
	format(string,sizeof(string),"{0049FF}[Now playing] {00c9ff}%s (Duration: %02dh %02dm %02ds)",GetVideoTitle(index),time[0],time[1],time[2]);

	if(Youtube[index][PlayFor] == INVALID_PLAYER_ID)
	{
		SendClientMessageToAll(-1, string);

		for(new i = 0, j = GetPlayerPoolSize(); i <= j; i++)
		{
			if(IsPlayerConnected(i))
			{
				if(Youtube[index][UsePos]) GetPlayerPos(i, pos[0], pos[1], pos[2]);
				PlayAudioStreamForPlayer(i, Youtube[index][StreamLink], pos[0], pos[1], pos[2], Youtube[index][Distance], Youtube[index][UsePos]);
			}
		}
	}
	else
	{
		SendClientMessage(Youtube[index][PlayFor], -1, string);

		if(Youtube[index][UsePos]) GetPlayerPos(Youtube[index][PlayFor], pos[0], pos[1], pos[2]);
		PlayAudioStreamForPlayer(Youtube[index][PlayFor], Youtube[index][StreamLink], pos[0], pos[1], pos[2], Youtube[index][Distance], Youtube[index][UsePos]);
	}
	
	Youtube[index][Timer] = SetTimerEx("SongFinished",GetVideoDuration(index)*1000,false,"i",index);
	
	return 1;
}

public SongFinished(youtubeid)
{
	StopYoutubeVideo(youtubeid);

	#if USE_VERSION == 2
		#if REMOVE_ON_FINISHED
		new string[128];
		format(string,sizeof(string),"%s/convert.php?delete=%s",CONVERTER_PATH,Youtube[youtubeid][ytID]);
		HTTP(9999, HTTP_GET, string, "", "OnYoutubeResponse");
		#endif
	#endif

	CallLocalFunction("OnYoutubeVideoFinished","i",youtubeid);
}

stock GetError(val)
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
		case 7: error = "Youtube -> MP3 api failed";
		case 300..308: error = "Redirection";
		case 400..499: error = "Client error";
		case 500..599: error = "Server error";
	}
	return error;
}

stock formatSeconds(seconds, &hours_left, &minutes_left, &seconds_left)
{
	hours_left = seconds/60/60;
	minutes_left = (seconds - hours_left*60*60)/60;
	seconds_left = (seconds - hours_left*60*60 - minutes_left*60);
}