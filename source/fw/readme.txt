HappyGem Firmwares
==================

Directory structure
-------------------

main.c:
Just forwards the call to main function to the firmware main function (fw_main).
We don't want to define the main function in the firmware main file, since
we want a different main function in the simulator.

happygem01:
Standard firmware for the first happygem prototype