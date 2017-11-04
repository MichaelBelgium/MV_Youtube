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

public OnYoutubeVideoStart(youtubeid)
public OnYoutubeVideoFinished(youtubeid)
public OnYoutubeDownloadError(youtubeid, message[])
public OnPlaylistAddEntry(playlistid, youtubeid)
public OnPlaylistFinished(playlistid)
```

# Requirements

* [SAMPSON](https://github.com/Hual/SAMPSON)

# Installing

By just doing this you're ready to roll.

```pawn
#include <[MV]_Youtube>
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

# Images
![Song info](http://puu.sh/oRnMo.jpg)
![Song info](http://puu.sh/oRnNh.png)