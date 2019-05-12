![alt text](https://raw.githubusercontent.com/gogos-venge/gogotetris/master/ss1.jpg)

Tetris game built for educational purposes

So yeah, this is just another Tetris, built for educational purposes. It's object oriented, built in pure C++ (pure C++11 xD) with SDL2 for presenting graphics, sounds and reading keys. The sounds are somewhat stolen from Nintendo, no, they actually ARE 100% stolen from Nintendo Tetris for Game Boy, but they were recorded through an emulator, and therefore I'm not sure if you can somehow prove these are Game Boy generated sounds, since they are not. In any case, this Tetris features a half complete Super Rotation System (SRS) (wallkicks are not implemented yet), bad graphics, a fast controlling system and the official Tetris scoring and speed system. Chances are you won't be able to build this, but if you somehow manage to do it, here are the controls: 
A: Rotate Clockwise
S: Rotate Counter Clockwise
Enter: Pause
Arrows: Control (except up ofc)

As I said before there's a great chance you'll give up trying to build this, although I promise it builds, so everything in the code is correct, you can grab it and learn some stuff if you find them worthy of learning. In the slight of possibilities you want this built, you're going to need freetype262.dll (freetype262d.dll for debug), SDL2.dll, SDL2_mixer.dll, SDL2_ttf.dll, PressStart2P.ttf, and the music folder to be placed in the root of the executable. Failing to do so, will result in fatal errors or maybe nothing, because I'm lazy to do checks, deeply sorry. I think nuget will try to get all the needed packages to build, but VS won't copy some of these DLLs in the output folder (I think freetype262, SDL2_ttf, and SDL2_mixer) so you'll have to copy them yourself.

Thanks for reading and ofc... Have fuN!

Gogos Venge