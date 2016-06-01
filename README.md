Youtube
=======

[MV]_Youtube, Stream youtube links into your SA:MP Server.

Functions and callbacks:

```PAWN
native PlayYoutubeVideoForPlayer(playerid,url[],start = 0)
native StopYoutubeVideoForPlayer(playerid)

native ContainsYoutubeURL(string[])
native IsYouTubeVideoPlaying(ofplayerid)

native GetVideoDuration(ofplayerid)
native GetVideoTitle(ofplayerid)
native GetVideoLink(ofplayerid)

public OnYoutubeVideoFinished(playerid)
```

<h1>Installing</h1>

To install or use [MV]_Youtube include you have to have to include line:
```pawn
#include <[MV]_Youtube>
```

<h1>Images</h1>
<img src="http://puu.sh/oRnMo.jpg" />
<img src="http://puu.sh/oRnNh.png" />
