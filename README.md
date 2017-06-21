# Youtube

[MV]_Youtube, with this include players are able to stream youtube video's in-game.

## Functions and callbacks:

**The functions PlayYoutubeVideoFor.. depends on which API you use.**

```PAWN
native PlayYoutubeVideoForPlayer(playerid, url[], (start = 0,) Float:distance = 50.0, bool:usepos = false)
native PlayYoutubeVideoForAll(url[], (start = 0,) Float:distance = 50.0, bool:usepos = false)
native StopYoutubeVideoForPlayer(playerid)

native IsValidYoutubeURL(string[])
native IsYouTubeVideoPlaying(ofplayerid)

native GetVideoDuration(ofplayerid)
native GetVideoTitle(ofplayerid)
native GetVideoLink(ofplayerid)

public OnYoutubeVideoFinished(playerid)
```

# Installing

First of all you need to choose which API you want. 
If you have your own dedicated server / vps / any kind then I recommend you make your own API (or use mine, but I doubt thats smart as it uses my links to my server).

## YoutubeInMP3 

* Requires nothing extra
* File: `[MV]_Youtube.inc` 
* Uses: JSON or no JSON

## youtubemp3script 

* Requires an **API key**
* File: `[MV]_Youtube2.inc`
* Uses: JSON

## Your own API

* Requires [Youtube-to-mp3-API](https://github.com/MichaelBelgium/Youtube-to-mp3-API)
* File: `[MV]_YoutubeVPS.inc` 
* Uses: JSON
* Notes: You might need to edit the url to match the right one on your vps

Pick one, download it, rename it and just include it in your gamemode like this:

```pawn
#include <[MV]_Youtube>
```

When using the VPS version you'll need to install all the required stuff. 

*You can get JSON for SA:MP here: [SAMPSON](https://github.com/Hual/SAMPSON)*

# Images
<img src="http://puu.sh/oRnMo.jpg" />
<img src="http://puu.sh/oRnNh.png" />
