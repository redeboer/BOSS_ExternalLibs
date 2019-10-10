# BOSS External Libraries

This repository is a collection header files and source code of the external libraries on which the [BESIII Offline Software System](https://besiii.gitbook.io/boss/tutorials/getting-started/intro) has been built. These header files can be used to activate autocomplete (intellisense), particular when working with [Visual Studio Code](https://code.visualstudio.com/). Another motivation to collect these files is (an attempt) to allow the user to build BOSS on one's own system.

Header files can be updated by running [`bash FetchExtLibs.sh`](FetchExtLibs.sh) on IHEP's [`lxslc`](https://besiii.gitbook.io/boss/tutorials/getting-started/server) server, but this should in principle only overwrite the files that already come with this repository.

The external libraries have been collected for **BOSS version 7.0.4.** and are (see [here](https://indico.ihep.ac.cn/event/8569/session/1/contribution/90/material/slides/1.pdf), p. 26):

- [ROOT v5.34](https://root.cern.ch/content/release-53434)
- [CLHEP v2.0.4.5](https://gitlab.cern.ch/CLHEP/CLHEP/tree/91a7f70af08cc371cd303be82951fae0a5364ea1)
- [Gaudi v23r9](https://gitlab.cern.ch/gaudi/Gaudi/tree/dd3b5243ca46915da72d26e46554cd1e5e20710e)
- [Geant4 v9.3](https://gitlab.cern.ch/geant4/geant4/tree/74cad5e589f2f0ce98503e2246afb07c62ffbff0)
- [CMT v1r25](http://www.cmtsite.net/download.html) (see also [its Wiki](https://trac.lal.in2p3.fr/CMT/wiki))
- Additional:
  - Reflex
  - Schema
  - Saxana
- (`gcc` version: 4.6.3)

Header files for BOSS 7.0.4 have also been added.

## Activating intellisense in Visual Studio Code

If you are editing code for BOSS package using Visual Studio Code, you can make use of [intellisense](https://code.visualstudio.com/docs/editor/intellisense) to have autocomplete available for all functions and class methods of these libraries, including BOSS. All you need to do is create a file called `.vscode/c_cpp_properties.json` in the folder in which you are working, containing the following lines:

```json
{
  "configurations": [
    {
      "name": "Linux64/Win64",
      "intelliSenseMode": "clang-x64",
      "includePath": [
        "${workspaceFolder}/ThirdParty/headers/Additional",
        "${workspaceFolder}/ThirdParty/headers/BOSS",
        "${workspaceFolder}/ThirdParty/headers/CLHEP",
        "${workspaceFolder}/ThirdParty/headers/Gaudi",
        "${workspaceFolder}/ThirdParty/headers/Geant4",
        "${workspaceFolder}/ThirdParty/headers/root"
      ],
      "cStandard": "c11",
      "cppStandard": "c++11",
      "browse": {
        "path": [
          "${workspaceFolder}/ThirdParty/headers/Additional",
          "${workspaceFolder}/ThirdParty/headers/BOSS",
          "${workspaceFolder}/ThirdParty/headers/CLHEP",
          "${workspaceFolder}/ThirdParty/headers/Gaudi",
          "${workspaceFolder}/ThirdParty/headers/Geant4",
          "${workspaceFolder}/ThirdParty/headers/root"
        ],
        "limitSymbolsToIncludedHeaders": true
      }
    }
  ],
  "version": 4
}
```

The fastest way to do this, is to press **Ctrl+Shift+P** (Show All Commands) and enter the command "C/C++: Edit configurations (JSON)". This will create or open the `.vscode/c_cpp_properties.json` file.

Note that you may have to edit the paths from the workspace. In this example, this repository has been added as a submodule of the [BOSS Starter Kit](https://github.com/redeboer/BOSS_StarterKit), where the BOSS External Libraries has been stored as submodule `ThirdParty/headers`.

Formally, you should use `c99` and `c++03`, because of the version of `gcc` on the `lxslc` server. Using [this trick](https://github.com/redeboer/BOSS_Tutorials/tree/master/Cpp11Support/Cpp11Support-00-00-00), though, you can activate many C++11 features within the BOSS package framework, including the use of `auto`.

Note that you may have to install the `boost` libraries. In Linux, this can be done through:

```bash
sudo apt-get install libboost-all-dev
```

By default, the headers are then installed under `/usr/include/boost`.

## Still to add

Some components that allow one to build BOSS locally on one's system are still missing:

- [Configuration Management Tool (`cmt`)](https://app.gitbook.com/@besiii/s/boss/tutorials/getting-started/intro#configuration-management-tool-cmt)
- [Concurrent Versions System (`CVS`)](http://polywww.in2p3.fr/activites/physique/glast/workbook/pages/softwareOverview/cvsBasics.htm), though this one becomes redundant when using Git
