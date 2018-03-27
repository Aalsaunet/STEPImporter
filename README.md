# STEPImporter
## Introduction
This project uses the Open Cascade Application Framework to allow geometry data to be read from step files and passed to other applications (such as Unity) throught Microsoft's COM framework.
The project should be compiled into a single DLL (called STEPImporter.dll) and used by calling the three exposed functions:
- ImportSTEPFile: Reads the specified file, transfer the content into memory and find all solid objects. Returns the number of objects found.
- ProcessSubShape: Computes the geometry data for the specified shape/solid (identified by its index) and writes it to a shared memory buffer.
- ClearStepModelData: Clears up the data structures used by the previously mention functions.

## Setup
To build the project the Open Cascade assemblies needs to be linked to. These are included in the repo, but can also be downloaded at https://www.opencascade.com/content/latest-release . 
When this is done, go into the VS project properties, and perform these two steps:
1. Go to Configuration Properties -> C/C++ -> (General) -> Additional Include Directories and list opencascade-7.2.0\inc and opencascade-7.2.0\src
2. Go to Configuration Properties -> Linker -> (General) -> Additional Library Directories and list opencascade-7.2.0\win64\vc10\lib and opencascade-7.2.0\win64\vc10\bin

Note: The project should be built for x64. A release build makes the DLL, while a debug build builds an executable that can be used for testing purposes.