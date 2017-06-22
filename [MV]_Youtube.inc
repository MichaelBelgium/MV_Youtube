/*
Credits:

YoutubeInMP3 - API - http://youtubeinmp3.com/api/
Westie - strlib: explode() - http://forum.sa-mp.com/showthread.php?t=85697
Michael@Belgium - [MV]_Youtube
*/

#define USE_JSON	true 		//do you have SAMPSON ?
#define MAX_YOUTUBE_SAVES	50
#define INVALID_YT_INDEX	-1

#include <a_http>

#if USE_JSON
	#include <a_json>
#endif

enum e_ytv
{
	bool:Playing,
	Duration,
	Title[256],
	Link[256],
	StreamLink[256],
	PlayFor,
	Float:Distance,
	bool:UsePos
};

new Youtube[MAX_YOUTUBE_SAVES][e_ytv];

forward OnYoutubeVideoFinished(youtubeid);
forward SongFinished(youtubeid);
forward OnYoutubeResponse(index, response_code, data[]);

stock PlayYoutubeVideoFor(url[], playfor = INVALID_PLAYER_ID, startat = 0, bool:usepos = false, Float:distance = 50.0)
{
	new id = GetFreeIndex();

	Youtube[id][PlayFor] = playfor;
	Youtube[id][Distance] = distance;
	Youtube[id][UsePos] = usepos;

	DownloadSong(id, url, startat);

	return id;
}

stock GetFreeIndex()
{
	for(new i = 0; i < MAX_YOUTUBE_SAVES; i++)
	{
		if(!Youtube[i][Playing]) return i;
	}

	return INVALID_YT_INDEX;
}

stock DownloadSong(id, youtubelink[], startat)
{
	new requestlink[128];

	if(strfind(youtubelink,"&") != -1) strmid(youtubelink,youtubelink,0,strfind(youtubelink,"&"), 128);
	format(Youtube[id][Link],256,"%s",youtubelink);

	#if USE_JSON
		format(requestlink,sizeof(requestlink),"youtubeinmp3.com/fetch/?format=json&video=%s&start=%i",Youtube[id][Link],startat);
	#else
		format(requestlink,sizeof(requestlink),"youtubeinmp3.com/fetch/?format=text&video=%s&start=%i",Youtube[id][Link],startat);
	#endif
	HTTP(id, HTTP_GET, requestlink, "", "OnYoutubeResponse");
}

stock StopYoutubeVideo(youtubeid)	
{
	Youtube[youtubeid][Playing] = false;
}
stock IsValidYoutubeURL(string[])			return (strfind(string,"youtube.com") != -1 && strfind(string,"watch?v=") != -1);
stock IsYouTubeVideoPlaying(youtubeid)		return youtubeid == INVALID_YT_INDEX ? false : Youtube[youtubeid][Playing];
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

public OnYoutubeResponse(index, response_code, data[])
{
	if(response_code == 200)
	{
		if (!strcmp(data, "<meta", true, 5))
			printf("{0049FF}[ERROR] {00c9ff}An error has occured: %s (%d)",GetError(response_code),response_code);
		else
		{
			new string[256],Float:pos[3], hours,minutes,seconds, tmp_seconds[8];

			#if USE_JSON
				new JSONNode:response;

				response = json_parse_string(data);
				json_get_string(response, Youtube[index][Title], 256, "title");
				json_get_string(response, tmp_seconds, 8, "length");
				json_get_string(response, Youtube[index][StreamLink], 256, "link");
				json_close(response);

				Youtube[index][Duration] = strval(tmp_seconds);
			#else
				new content[3][256];

				explode(content,data,"<br/>");

				strmid(Youtube[index][Title], content[0], 7, strlen(content[0]), 256);
				strmid(tmp_seconds,content[1],8,strlen(content[1]));
				strmid(Youtube[index][StreamLink],content[2],6,strlen(content[2]));

				Youtube[index][Duration] = strval(tmp_seconds);
			#endif

			Youtube[index][Playing] = true;

			formatSeconds(GetVideoDuration(index),hours,minutes,seconds);

			format(string,sizeof(string),"{0049FF}[Now playing] {00c9ff}%s (Duration: %02dh %02dm %02ds)",GetVideoTitle(index),hours,minutes,seconds);

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
			
			SetTimerEx("SongFinished",(Youtube[index][Duration]+5)*1000,false,"i",index);
		}
	}
	else
		printf("[ERROR] An error has occured: %s (%d - %s)",data, response_code, GetError(response_code));
}

public SongFinished(youtubeid)
{
	StopYoutubeVideo(youtubeid);
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

/*stock explode(aExplode[][], const sSource[], const sDelimiter[] = " ", iVertices = sizeof aExplode, iLength = sizeof aExplode[])
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
}*/