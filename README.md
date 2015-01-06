Goobbue
=======

Crafting solver for FFXIV

Goobbue is an experiment to see if I could programtically solve crafting in FFXIV.
A blog post about Goobbue is [here](http://secondreality.co.uk/blog/solving-crafting-in-ffxiv/).

##How to configure crafters
Crafter information is stored in json files which you will have to edit manually. An option is displayed in the application to configure these crafters but it's currently not functional.

## Build requirements
Goobue is configured to build with CLion and g++. It builds on Windows and Linux.

### Threading
To enable threading support you'll need the boost threading library, if you do not have this then comment out the USE_THREADS define in expectimax.h.
