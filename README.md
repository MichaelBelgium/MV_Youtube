# Youtube

[MV]_Youtube, with this include players are able to stream youtube video's in-game.

## Functions and callbacks:

**The functions PlayYoutubeVideoFor.. depends on which API you use.**

```PAWN
native PlayYoutubeVideoFor(url[], playfor = INVALID_PLAYER_ID, playlist = INVALID_PLAYLIST_ID, (startat = 0,) bool:usepos = false, Float:distance = 50.0) 
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
public OnPlaylistAddEntry(playerlistid, youtubeid)
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
If you have your own dedicated server / vps / any kind then I recommend you make your own API (or use mine, but I doubt thats smart as it uses my links to my server).

```PAWN
#define USE_VERSION			1
```

### 0: YoutubeInMP3 

* Requires nothing extra

### 1: youtubemp3script 

* Requires an **API key** (the key "12345" works so it's default)

### 2: Your own API

* Requires [Youtube-to-mp3-API](https://github.com/MichaelBelgium/Youtube-to-mp3-API)
* Notes: You might need to edit the requesturl in the include to match the one from your vps.

# Images
<img src="http://puu.sh/oRnMo.jpg" />
<img src="http://puu.sh/oRnNh.png" />
