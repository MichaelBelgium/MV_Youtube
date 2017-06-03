/*
Credits:

YoutubeInMP3 - API - http://youtubeinmp3.com/api/
Westie - strlib: explode() - http://forum.sa-mp.com/showthread.php?t=85697
Michael@Belgium - [MV]_Youtube

native PlayYoutubeVideoForPlayer(playerid, url[], start = 0, Float:distance = 50.0, bool:usepos = false)
native StopYoutubeVideoForPlayer(playerid)
native IsValidYoutubeURL(string[])
native IsYouTubeVideoPlaying(ofplayerid)
native GetVideoDuration(ofplayerid)
native GetVideoTitle(ofplayerid)
native GetVideoLink(ofplayerid)
*/

#define USE_JSON	true 		//do you have SAMPSON ?

#include <a_http>

#if USE_JSON
	#include <a_json>
#endif

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

stock PlayYoutubeVideoForPlayer(playerid, url[], start = 0, Float:distance = 50.0, bool:usepos = false)
{
	if(strfind(url,"&") != -1) strmid(url,url,0,strfind(url,"&"),128);
	Youtube[playerid][Distance] = distance;
	Youtube[playerid][Usepos] = usepos;
	format(Youtube[playerid][Link],256,"%s",url);
	format(url,128,"youtubeinmp3.com/fetch/?format=%s&video=%s&start=%i",USE_JSON == true ? ("json") : ("text"), Youtube[playerid][Link],start);
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
	new error[128];

	if(response_code == 200)
	{
		if (!strcmp(data, "<meta", true, 5))
		{
			response_code = 7;
			format(error,sizeof(error),"{0049FF}[ERROR] {00c9ff}An error has occured: %s (%d)",GetError(response_code),response_code);
			SendClientMessage(playerid,0xFF0000FF,error);
		}
		else
		{
			new stream[256],string[256],Float:pos[3], hours,minutes,seconds, tmp_seconds[8];

			#if USE_JSON
				new JSONNode:response;

				response = json_parse_string(data);
				json_get_string(response, Youtube[playerid][Title], 256, "title");
				json_get_string(response, tmp_seconds, 8, "length");
				json_get_string(response, stream, 256, "link");
				json_close(response);

				Youtube[playerid][Duration] = strval(tmp_seconds);
			#else
				new content[3][256];

				explode(content,data,"<br/>");

				strmid(Youtube[playerid][Title], content[0], 7, strlen(content[0]), 256);
				strmid(tmp_seconds,content[1],8,strlen(content[1]));
				strmid(stream,content[2],6,strlen(content[2]));

				Youtube[playerid][Duration] = strval(tmp_seconds);
			#endif

			if(Youtube[playerid][Usepos]) GetPlayerPos(playerid, pos[0], pos[1], pos[2]);

			formatSeconds(Youtube[playerid][Duration],hours,minutes,seconds);
			
	  		format(string,sizeof(string),"{0049FF}[Now playing] {00c9ff}%s (Duration: %02dh %02dm %02ds)",GetVideoTitle(playerid),hours,minutes,seconds);
			SendClientMessage(playerid,-1,string);

			PlayAudioStreamForPlayer(playerid, stream, pos[0], pos[1], pos[2], Youtube[playerid][Distance], Youtube[playerid][Usepos]);
			Youtube[playerid][Playing] = true;
			
			SetTimerEx("SongFinished",(Youtube[playerid][Duration]+5)*1000,false,"i",playerid);
		}
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

stock explode(aExplode[][], const sSource[], const sDelimiter[] = " ", iVertices = sizeof aExplode, iLength = sizeof aExplode[])
{
	new
		iNode,
		iPointer,
		iPrevious = -1,
		iDelimiter = strlen(sDelimiter);

	while(iNode < iVertices)
	{
		iPointer = strfind(sSource, sDelimiter, false, iPointer);

		if(iPointer == -1)
		{
			strmid(aExplode[iNode], sSource, iPrevious, strlen(sSource), iLength);
			break;
		}
		else
			strmid(aExplode[iNode], sSource, iPrevious, iPointer, iLength);

		iPrevious = (iPointer += iDelimiter);
		++iNode;
	}
	return iPrevious;
}