// This is a comment
// uncomment the line below if you want to write a filterscript
//#define FILTERSCRIPT

#include <a_samp>
#include <zcmd>
#include <sscanf2>
#include <[MV]_Youtube2>
//or
//#include <[MV]_Youtube>
//#include <[MV]_YoutubeVPS>

new gYoutubeID[MAX_PLAYERS], gYoutubeIDForAll;

public OnFilterScriptInit()
{
	print("\n--------------------------------------");
	print(" Example filterscript to use with [MV]_Youtube");
	print("--------------------------------------\n");

	for(new i = 0; i < MAX_PLAYERS; i++) gYoutubeID[i] = INVALID_YT_INDEX;
	gYoutubeIDForAll = INVALID_YT_INDEX;
	return 1;
}

public OnFilterScriptExit()
{
	return 1;
}

CMD:playforme(playerid,params[])
{
	new song[256];
	if(sscanf(params, "s[256]", song)) return SendClientMessage(playerid, -1, "Usage: /playforme [youtube url]");
	if(!IsValidYoutubeURL(song)) return SendClientMessage(playerid, -1, "Invalid url.");
	if(IsYouTubeVideoPlaying(gYoutubeID[playerid])) return SendClientMessage(playerid, -1, "A song is already playing.");

	gYoutubeID[playerid] = PlayYoutubeVideoFor(song, playerid);
	return 1;
}	

CMD:playforall(playerid,params[])
{
	new song[256];
	if(sscanf(params, "s[256]", song)) return SendClientMessage(playerid, -1, "Usage: /playforall [youtube url]");
	if(!IsValidYoutubeURL(song)) return SendClientMessage(playerid, -1, "Invalid url.");
	if(IsYouTubeVideoPlaying(gYoutubeIDForAll)) return SendClientMessage(playerid, -1, "A song is already playing.");

	gYoutubeIDForAll = PlayYoutubeVideoFor(song);
	return 1;
}	

CMD:whatsongisplayingforme(playerid,params[])
{
	new playing[256];

	format(playing, sizeof(playing), "Name: %s\nDuration: %i seconds\nLink: %s", GetVideoTitle(gYoutubeID[playerid]), GetVideoDuration(gYoutubeID[playerid]), GetVideoLink(gYoutubeID[playerid]));
	ShowPlayerDialog(playerid, 0, DIALOG_STYLE_MSGBOX, "Now playing", playing, "OK", "");
	return 1;
}

CMD:whatsongisplaying(playerid,params[])
{
	new playing[256];

	format(playing, sizeof(playing), "Name: %s\nDuration: %i seconds\nLink: %s", GetVideoTitle(gYoutubeIDForAll), GetVideoDuration(gYoutubeIDForAll), GetVideoLink(gYoutubeIDForAll));
	ShowPlayerDialog(playerid, 0, DIALOG_STYLE_MSGBOX, "Now playing", playing, "OK", "");
	return 1;
}

public OnYoutubeVideoFinished(youtubeid)
{
	new string[128];
	if(youtubeid == gYoutubeIDForAll)
	{
		format(string, sizeof(string), "The song that played for everyone (%s) has finished. Execute /playforall to play another song.", GetVideoTitle(youtubeid));
		SendClientMessageToAll(-1, string);
	}
	
	for(new i = 0, j = GetPlayerPoolSize(); i <= j; i++)
	{
		if(gYoutubeID[i] == youtubeid)
		{
			format(string, sizeof(string), "The song that played for you (%s) has finished. Execute /playforme to play another song.", GetVideoTitle(youtubeid));
			SendClientMessage(i, -1, string);
			break;
		}
	}

	return 1;
}