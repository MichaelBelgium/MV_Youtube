# Youtube

[MV]_Youtube, with this include players are able to stream youtube video's in-game.

## Functions and callbacks:


```PAWN
native PlayYoutubeVideoFor(const url[], playfor = INVALID_PLAYER_ID, playlist = INVALID_PLAYLIST_ID, bool:usepos = false, Float:distance = 50.0) 
	-> returns a youtubeid
native StopYoutubeVideo(youtubeid)

native IsValidYoutubeURL(const string[])
native IsYouTubeVideoPlaying(youtubeid)

native GetVideoDuration(youtubeid)
native GetVideoTitle(youtubeid)
native GetVideoLink(youtubeid)
native GetVideoStreamLink(youtubeid)
native GetVideoTarget(youtubeid)

native CreatePlaylist(const name[])
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
public OnMVYoutubeError(youtubeid, const message[])
public OnPlaylistAddEntry(playlistid, youtubeid)
public OnPlaylistFinished(playlistid)
```

# Requirements

* [pawn-requests](https://github.com/Southclaws/pawn-requests)
* [Hash-map implementation in PAWN](https://github.com/BigETI/pawn-map) (optional)

# Installing

Using sampctl:

`sampctl package install MichaelBelgium/MV_Youtube`

And/or manually:

```pawn
#include <MV_Youtube>
```

## Youtube API

This include requires a Youtube API.. which one? This one: [Youtube-to-mp3-API](https://github.com/MichaelBelgium/Youtube-to-mp3-API)

You are free to use the existing settings of this include that uses the installed youtube API on my domain. If preferred, you can install the API yourself on your own dedicated server/vps and configure the include to use that.

## Settings

```PAWN
#define MAX_YOUTUBE_SAVES	50
#define MAX_PLAYLISTS		5

#define CONVERTER_PATH		"https://youtube.michaelbelgium.me/api/converter/"
#define MAX_SEARCH_RESULTS 	5
```

# Images
![Song info](http://puu.sh/oRnMo.jpg)
![Song info](http://puu.sh/oRnNh.png)
