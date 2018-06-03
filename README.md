# Downpour

Downpour is a multiplayer [Puyo Puyo](https://en.wikipedia.org/wiki/Puyo_Puyo) puzzle game inspired by [Tetrinet](https://en.wikipedia.org/wiki/TetriNET) written by me sometime around 2003. The game is very playable but in terms of UI it's a bit rough around the edges. I'm archiving this project on GitHub for posterity and grant everybody the right to do anything they want with the source code or artwork royalty free. 

<img src="https://user-images.githubusercontent.com/5240185/40891432-daa41d4a-674a-11e8-98f1-b15cc85c2725.jpg" width="45%"></img>
<img src="https://user-images.githubusercontent.com/5240185/40891433-dab0adda-674a-11e8-83ee-270046f6d26a.jpg" width="45%"></img>

Downpour is written in C++ using [SDL 1.2](https://www.libsdl.org/download-1.2.php), [SDL_net](https://www.libsdl.org/projects/SDL_net/release-1.2.html), & [SDL_ttf](https://www.libsdl.org/projects/SDL_ttf/release-1.2.html)

This build was originally compiled using [DEV-CPP](https://en.wikipedia.org/wiki/Dev-C%2B%2B) (don't recall the version) and SDL 1.2.6, SDL_net 1.2.5, and SDL_ttf 2.0.6

The font "times.ttf" included in the release was originally Windows Times New Roman font which I assume I don't have the legal right to include so I found a font I could include and renamed it to times.ttf. It originally was called "cmunbi.ttf" and the README for that font package is the following:

>This package was compiled by Christian Perfect (http://checkmyworking.com) from the Computer Modern Unicode fonts created by Andrey V. Panov (http://cm-unicode.sourceforge.net/)

> They're released under the SIL Open Font License. See OFL.txt and OFL-FAQ.txt for the terms.

>A demo page for these fonts was at http://www.checkmyworking.com/cm-web-fonts/ when I released them. I can only apologise, citizen of the future, if that address doesn't exist any more. 

## How to Play

First clicking the main **SERVER** button (top or bottom of screen) will crash the app. I never got around to adding the dedicated server funtionalition so you don't click this button... 

Click **CONNECT** to bring up the network menu and enter your username by clicking on **NoName**. 

To host a room click the **SERVER** button in the middle of the window not the one at the bottom. Chances are Windows firewall dialog box will pop up requesting you give this app permission. Once you give the app permission you will now be hosting a server. NOTE: All network activity is restricted to using port 9999 so you may to adjust your router firewall/port forwarding settings accordingly.

To connect to a server click in the **Conect To:** text area and enter the hostname or ip address hosting a game. Then click the **CONNECT** button in the middle of the window (not at bottom) to connect to the server. 

Once connected click the **CHAT ROOM** button and click the **START** or **STOP** button to begin or end a game. Once the game has started you need to clikc the **SHOW FIELDS** button to display the game boards.

You control the piece by using the arrorow keys. LEFT/RIGHT moves the piece and UP/DOWN rotates the peice. The space bar drops the piece 1 row and holding it down makes the piece fall quicker.

If you just want to goof around you can host your own server without anybody else connected and start a game from the chat room menu.
