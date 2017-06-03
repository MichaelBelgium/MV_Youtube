/*
Credits:
youtubemp3script - API -http://www.youtubemp3script.com/
Michael@Belgium - [MV]_Youtube

native PlayYoutubeVideoForPlayer(playerid, url[], Float:distance = 50.0, bool:usepos = false)
native StopYoutubeVideoForPlayer(playerid)
native ContainsYoutubeURL(string[])
native IsYouTubeVideoPlaying(ofplayerid)
native GetVideoDuration(ofplayerid)
native GetVideoTitle(ofplayerid)
native GetVideoLink(ofplayerid)
*/

#include <a_http>
#include <a_json>

enum e_yt
{
	bool:Playing,
	Duration,
	Title[256],
	Link[256],
	Float: Distance,
	bool: Usepos
};

new Youtube[MAX_PLAYERS][e_yt];

forward OnYoutubeVideoFinished(playerid);
forward SongFinished(playerid);
forward OnYoutubeResponse(playerid, response_code, data[]);

stock PlayYoutubeVideoForPlayer(playerid, url[], Float:distance = 50.0, bool:usepos = false)
{
	if(strfind(url,"&") != -1) strmid(url,url,0,strfind(url,"&"),128);
	Youtube[playerid][Distance] = distance;
	Youtube[playerid][Usepos] = usepos;
	format(Youtube[playerid][Link],256,"%s",url);
	format(url,128,"www.yt-mp3.com/fetch?v=%s&apikey=1234567",Youtube[playerid][Link]);
	HTTP(playerid, HTTP_GET, url, "", "OnYoutubeResponse");
}

stock PlayYoutubeVideoForAll(url[], start = 0, Float:distance = 50.0, bool:usepos = false)
{
	for(new i = 0, j = GetPlayerPoolSize(); i <= j; i++)
		if(IsPlayerConnected(i))
			PlayYoutubeVideoForPlayer(i, url, start, distance, usepos);
}

stock StopYoutubeVideoForPlayer(playerid)	
{
	Youtube[playerid][Playing] = false;
	StopAudioStreamForPlayer(playerid);
}
stock IsValidYoutubeURL(string[])			return (strfind(string,"youtube.com") != -1 && strfind(string,"watch?v=") != -1);
stock IsYouTubeVideoPlaying(ofplayerid)		return Youtube[ofplayerid][Playing];
stock GetVideoDuration(ofplayerid)			return Youtube[ofplayerid][Duration];
stock GetVideoTitle(ofplayerid) 
{
    new str[256];
    strins(str, Youtube[ofplayerid][Title], 0);
    return str;
}

stock GetVideoLink(ofplayerid) 
{
    new str[256];
    strins(str, Youtube[ofplayerid][Link], 0);
    return str;
}

public OnYoutubeResponse(playerid, response_code, data[])
{
	new error[128],stream[512];

	if(response_code == 200)
	{
		new JSONNode:response, Float:pos[3], hours,minutes,seconds, string[128];

		response = json_parse_string(data);

		json_get_string(response, Youtube[playerid][Title], 256, "title");
		Youtube[playerid][Duration] = json_get_int(response, "length");
		json_get_string(response, stream, 512, "url");
		strins(stream, "http:", 0);

		json_close(response);

		if(Youtube[playerid][Usepos]) GetPlayerPos(playerid, pos[0], pos[1], pos[2]);
		formatSeconds(Youtube[playerid][Duration],hours,minutes,seconds);
		
  		format(string,sizeof(string),"{0049FF}[Now playing] {00c9ff}%s (Duration: %02dh %02dm %02ds)",GetVideoTitle(playerid),hours,minutes,seconds);
		SendClientMessage(playerid,-1,string);

		PlayAudioStreamForPlayer(playerid, stream, pos[0], pos[1], pos[2], Youtube[playerid][Distance], Youtube[playerid][Usepos]);
		Youtube[playerid][Playing] = true;
		
		SetTimerEx("SongFinished",(Youtube[playerid][Duration]+5)*1000,false,"i",playerid);

	}
	else
	{
		format(error,sizeof(error),"{0049FF}[ERROR] {00c9ff}An error has occured: %s (%d - %s)",data, response_code, GetError(response_code));
		SendClientMessage(playerid,0xFF0000FF,error);
	}
}

public SongFinished(playerid)
{
	Youtube[playerid][Playing] = false;
	Youtube[playerid][Usepos] = false;
	CallLocalFunction("OnYoutubeVideoFinished","i",playerid);
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