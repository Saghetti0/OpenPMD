# OpenPMD
An open-source re-creation of Pokemon Mystery Dungeon: Explorers of Sky. Also includes multiplayer support.

**Caution: OpenPMD is still in very early alpha. Don't expect anything to work**

## Building
For now, this project uses a custom build script written in Python to compile the code. Probably going to switch over to premake sometime in the future.

### Requirements
* For Windows
  * Python 3.5 or newer
  * mingw32-gcc
  * SDL2 installed and visible mingw32-gcc
* For Linux
  * Python 3.5 or newer
  * GCC
  * SDL2-devel

### Compiling
Compiling is (or at least should) be as easy as running this command

Compiling the client using mingw32
```bash
python build.py client_mingw32
```

Compiling the client using GCC on Linux (note: this should work, but hasn't been tested yet. Feel free to open an issue if it doesn't work for you.)
```bash
python build.py client
```

Compiling the server. As of now, the server is nothing more than a hello world application, as it hasn't been implemented yet.
```bash
python build.py server
```

View help for the build script
```bash
python build.py -h
```

### Running
The OpenPMD client expects the following files and folders to be available in the working directory.
* dungeon_gfx/ - contains the graphics and data used to render dungeons. I will upload these later.
* SDL2.dll (on windows)
* icon.bmp - will also be uploaded later (in the form of a release)

The server (located in bin/server/) requires these files to be present
* pthreadGC2.dll from pthreads-win32 (on windows only)
