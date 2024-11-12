# kpisrla

A [SRLA](https://github.com/aikiriao/SRLA) decoder plugin for [KbMedia Player](https://kobarin.sakura.ne.jp/index.htm)

## Projects

This repository contains two VC++ 2022 projects within a single solution:

* `kpisrla`: SRLA decoder plugin for KbMedia Player
* `srlaenc`: A 24bit-aware SRLA command line encoder

## Building

To build the projects, first clone the repository:

```
git clone --recursive https://github.com/autch/kpisrla.kpi.git
```

Ensure you have Visual Studio 2022 installed with the `Desktop Development with C++` and `C++ CMake tools for Windows` features enabled.

1. Open the `x64 Native Tools Command Prompt for VS 2022` from the Start menu.
2. Navigate to the repository folder that you just cloned.
3. Create a build directory and configure the build with CMake:

```
md build-srla
cd build-srla
cmake ../SRLA -G"Visual Studio 17 2022" -A <platform>
```

Replace `<platform>` with `x64` or `Win32` depending on your target architecture.

Next, build the SRLA libraries:

```
msbuild ALL_BUILD.vcxproj /p:Configuration=Debug
msbuild ALL_BUILD.vcxproj /p:Configuration=Release
```

Once the build completes successfully, open `kpisrla.sln` in Visual Studio. In the Property Manager, locate and edit the `KBMPPath` property to specify the path to `Kbmplay.exe`

After setting the path, you can build `kpisrla.kpi`. The plugin will be automatically installed into the KbMedia Player’s Plugins folder.

To (re)build `kpisrla.kpi` for a different platform, delete the existing `build-srla` directory, recreate it, and re-run the `cmake` command with a different `-A` option. Note that CMake does not support multiple architectures within a single build directory. For more details, refer to [CMake #18450](https://gitlab.kitware.com/cmake/cmake/-/issues/18450)


## License

MIT
