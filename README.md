## DoukutsuPSX

![Screenshot](doc/screen.png)

This is a port of Cave Story for the Sony PlayStation.
It is based on CSE2, which is a decompilation of the original freeware Cave Story executables.
The port is still very much a work-in-progress, but it should be possible to at least complete the game.

### How to run

First, either build the game yourself (see instructions below) or download the latest release from the Releases page.
This will yield an ISO image.

To play the game on a modded PlayStation, burn the ISO image to a CD-R with something like ImgBurn (tested on my modchipped SCPH-9002).
It should also work fine on most emulators (tested on PCSX-Redux, no$psx and DuckStation).
Please **do not** use outdated emulators like ePSXe or PCSX Reloaded, I won't fix issues that only appear in those.

You will need a memory card with at least 1 free block to properly save the game. You can continue the game from
Save Points when you die even if you don't have a memory card, though. Multitap is currently not supported.

#### Default controls
* `DPAD`: movement;
* `CROSS`: jump/accept;
* `SQUARE`: fire;
* `TRIANGLE`: previous weapon;
* `CIRCLE`: next weapon/cancel;
* `START`: pause menu;
* `L2`: item menu;
* `R2`: map screen.

Controls can be rebound in the Options menu.

### How to build

1. Obtain GNU Make and GCC for targets `mipsel-none-elf` or `mipsel-linux-gnu`:
    * on Windows: see [this section](https://github.com/ABelliqueux/nolibgs_hello_worlds#mips-toolchain-setup) or use WSL;
    * on Linux: see [this section](https://github.com/ABelliqueux/nolibgs_hello_worlds#install-your-distributions-mips-toolchain).
2. Obtain [mkpsxiso](https://github.com/Lameguy64/mkpsxiso) and ensure it is in `PATH`.
3. Clone this repository: `git clone --recursive https://github.com/fgsfdsfgs/doukutsupsx && cd doukutsupsx`
4. Obtain [converted PsyQ 4.7 libraries](http://psx.arthus.net/sdk/Psy-Q/psyq-4.7-converted-full.7z) and extract them into `nugget/psyq/`.
5. (Optional) If you have the PsyQ license files (`LICENSEA.DAT`, `LICENSEE.DAT`, `LICENSEJ.DAT`), put them into this folder and uncomment
one of the `<license>` lines in `iso.xml` to inject it into the resulting ISO. This will give it a proper boot logo and might be required
by some BIOS versions (?).
6. Run `make iso`. This should produce an ISO file called `doukutsu.iso`.

### Credits

* Daisuke "Pixel" Amaya for the original Cave Story;
* CuckyDev, Clownacy, Gabriel Ravier and probably others for CSE2;
* [PCSX-Redux authors](https://github.com/grumpycoders/pcsx-redux/blob/main/AUTHORS) for Nugget and PCSX-Redux itself;
* Schnappy for [nolibgs_hello_worlds](https://github.com/ABelliqueux/nolibgs_hello_worlds) and the toolchain setup instructions;
* Lameguy64 for PSn00bSDK, which was used for the earlier versions of this port, and for mkpsxiso;
* Adrian "asie" Siekierka and Ben "GreaseMonkey" Russell for libpsxav;
* Sean Barrett for stb_image;
* David Reid for dr_wav;
* axetion, CuckyDev, impiaaa, Infu, Nicolas Noble, peach, Schnappy, sickle, Stenzek and other nice people from the PSXDEV Discord server
for help and testing;
* probably more people I'm forgetting.
