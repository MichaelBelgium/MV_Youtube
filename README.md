# Youtube

[MV]_Youtube, with this include players are able to stream youtube video's in-game.

## Functions and callbacks:

**The functions PlayYoutubeVideoFor* depends on which API you use.**

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

To install or use [MV]_Youtube include you have to have to include line:
```pawn
#include <[MV]_Youtube>
```

*If you want to use JSON you need to have [SAMPSON](https://github.com/Hual/SAMPSON)*
Note: JSON is required with the API that gets used in `[MV]_Youtube2.inc`

# Images
<img src="http://puu.sh/oRnMo.jpg" />
<img src="http://puu.sh/oRnNh.png" />
