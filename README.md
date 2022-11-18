# Introduction
This repository documents changes to [Unifont](https://unifoundry.com/unifont/), used by Minecraft as of version 1.19.4.

**This project exists only for documentation purposes.** Issues related to this font in the game should still be reported on https://bugs.mojang.com.

Also please note that this is a very old version of Unifont, so there is not much point in using this in any new project.

## Context
Unifont is bundled with the game as a series of images named `/assets/minecraft/textures/font/unicode_page_*.png`, together with a generated file `/assets/minecraft/font/glyph_sizes.bin`.

In the game, it is used as a fallback for glyphs missing from default font and as a seperate font with identifier `minecraft:uniform`.
Users can force UI to use Unifont glyphs exclusively by enabling option "Force Unicode Font" in language options.

No part of original package (including `hex` files) was used by the game directly.
This repository tries to re-create changes to source based on changes to images.

Files were first included in snapshot [11w49a](https://web.archive.org/web/20120309102948/http://www.mojang.com/2011/12/minecraft-development-snapshot-week-49/) and were based on work of Ryan 'Scaevolus' Hitchman.

## Contents
- `/unifont-5.1.20080914` - original package (or one that is sufficiently close to match reconstructed `hex` files)
   - downloaded from [http://czyborra.com/unifont/updates/unifont-5.1.20080914/](https://web.archive.org/web/20211206234649/http://czyborra.com/unifont/updates/unifont-5.1.20080914/)
- `/hex` - copy of `/unifont-5.1.20080914/font/precompiled/unifontfull.hex` with reconstructed changes applied
- `/png` - rendering of current contents of `/hex/unifont.hex` (for purposes of visualisation of differences)

## Changes
- Filtered out Private Use Area (PUA) characters (pattern `FFB9C5EDD5D5D5D5D5D5D5D5EDB991FF`)
- Filtered out placeholder glyphs (pattern: `00542A542A542A542A542A542A542A00`), except for U+ABXX range
