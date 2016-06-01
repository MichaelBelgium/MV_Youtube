/*
Credits:

YoutubeInMP3 - API - http://youtubeinmp3.com/api/
Westie - strlib: explode() - http://forum.sa-mp.com/showthread.php?t=85697
Michael@Belgium - [MV]_Youtube

native PlayYoutubeVideoForPlayer(playerid,url[])
native StopYoutubeVideoForPlayer(playerid)
native ContainsYoutubeURL(string[])
native IsYouTubeVideoPlaying(ofplayerid)
native GetVideoDuration(ofplayerid)
native GetVideoTitle(ofplayerid)
native GetVideoLink(ofplayerid)
*/

#include <a_http>

enum e_yt
{
	bool:Playing,
	Duration,
	Title[256],
	Link
};

new Youtube[MAX_PLAYERS][e_yt];

forward OnYoutubeVideoFinished(playerid);
forward SongFinished(playerid);

stock PlayYoutubeVideoForPlayer(playerid, url[], start = 0)
{
	if(strfind(url,"&") != -1) strmid(url,url,0,strfind(url,"&"),128);
	format(Youtube[playerid],256,"%s",url);
	format(url,128,"youtubeinmp3.com/fetch/?format=text&video=%s&start=%i",Youtube[playerid],start);
	HTTP(playerid, HTTP_GET, url, "", "OnYoutubeResponse");
}

stock StopYoutubeVideoForPlayer(playerid)	return StopAudioStreamForPlayer(playerid);
stock ContainsYoutubeURL(string[])			return (strfind(string,"youtube") != -1 && strfind(string,"watch?v=") != -1);

stock IsYouTubeVideoPlaying(ofplayerid)		return Youtube[ofplayerid][Playing];
stock GetVideoDuration(ofplayerid)			return Youtube[ofplayerid][Duration];
stock GetVideoTitle(ofplayerid)				return Youtube[ofplayerid][Title];
stock GetVideoLink(ofplayerid)				return Youtube[ofplayerid][Link];

forward OnYoutubeResponse(playerid, response_code, data[]);
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
			new content[3][256],stream[256],string[256];
			new hours,minutes,seconds,tmp_seconds[8];

			explode(content,data,"<br/>");

			strmid(tmp_seconds,content[1],8,strlen(content[1]));
			strmid(stream,content[2],6,strlen(content[2]));

			Youtube[playerid][Duration] = strval(tmp_seconds);
			Youtube[playerid][Title] = content[0];

			formatSeconds(Youtube[playerid][Duration],hours,minutes,seconds);
			
	  		format(string,sizeof(string),"{0049FF}[Now playing] {00c9ff}%s (Duration: %02dh %02dm %02ds)",GetVideoTitle(playerid),hours,minutes,seconds);
			SendClientMessage(playerid,-1,string);

			PlayAudioStreamForPlayer(playerid,stream);
			Youtube[playerid][Playing] = true;
			
			SetTimerEx("SongFinished",(Youtube[playerid][Duration]+5)*1000,false,"i",playerid);
		}
	}
	else
	{
		format(error,sizeof(error),"{0049FF}[ERROR] {00c9ff}An error has occured: %s (%d)",GetError(response_code),response_code);
		SendClientMessage(playerid,0xFF0000FF,error);
	}
}

public SongFinished(playerid)
{
	Youtube[playerid][Playing] = false;
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
		{
			strmid(aExplode[iNode], sSource, iPrevious, iPointer, iLength);
		}

		iPrevious = (iPointer += iDelimiter);
		++iNode;
	}
	return iPrevious;
}
