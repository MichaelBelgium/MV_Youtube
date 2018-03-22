# Youtube

[MV]_Youtube, with this include players are able to stream youtube video's in-game.

## Functions and callbacks:


```PAWN
native PlayYoutubeVideoFor(url[], playfor = INVALID_PLAYER_ID, playlist = INVALID_PLAYLIST_ID, bool:usepos = false, Float:distance = 50.0) 
	-> returns a youtubeid
native StopYoutubeVideo(youtubeid)

native IsValidYoutubeURL(string[])
native IsYouTubeVideoPlaying(youtubeid)

native GetVideoDuration(youtubeid)
native GetVideoTitle(youtubeid)
native GetVideoLink(youtubeid)
native GetVideoStreamLink(youtubeid)
native GetVideoTarget(youtubeid)

native CreatePlaylist(name[])
	-> returns a playlistid
native RemovePlaylist(playlistid)
native GetPlaylistName(playlistid)
native GetPlaylistSongs(playlistid)
native GetPlaylistSongsCount(playlistid)
native GetPlaylistFromVideo(youtubeid)

native SearchYoutubeVideos(playerid, searchquery[])
public OnYoutubeSearch(playerid)

public OnYoutubeVideoStart(youtubeid)
public OnYoutubeVideoFinished(youtubeid)
public OnMVYoutubeError(youtubeid, message[])
public OnPlaylistAddEntry(playlistid, youtubeid)
public OnPlaylistFinished(playlistid)
```

# Requirements

* [SAMPSON](https://github.com/Hual/SAMPSON)

# Installing

Using sampctl:

`sampctl package install MichaelBelgium/MV_Youtube:^4.x`

And/or manually:

```pawn
#include <MV_Youtube>
```

## Settings

First of all you need to choose which API you want. Default is youtubemp3script.
If you have your own dedicated server / vps / any kind then I recommend you make your own API (number 2)

```PAWN
#define USE_VERSION			1
```

### 0: youtubemp3script 

* Requires an **API key**

### 1: Your own API

* Requires [Youtube-to-mp3-API](https://github.com/MichaelBelgium/Youtube-to-mp3-API)
* Notes: You might need to edit the requesturl in the include to match the one from your vps.
* If you don't wanna install it you can use mine (from michaelbelgium.me domain).

Other settings:
```PAWN
#define MAX_YOUTUBE_SAVES	50
#define INVALID_YT_ID		-1
#define INVALID_PLAYLIST_ID	-1
#define MAX_PLAYLISTS		5

#if USE_VERSION == 0
#define API_KEY				"1234567"
#elseif USE_VERSION == 1
#define CONVERTER_PATH		"michaelbelgium.me/ytconverter"
#define REMOVE_ON_FINISHED	true 		//Remove the downloaded song when the song finished playing or not
#define MAX_SEARCH_RESULTS 	5
#endif
```

# Images
![Song info](http://puu.sh/oRnMo.jpg)
![Song info](http://puu.sh/oRnNh.png)
