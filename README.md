Unofficial HSA SDK
==================

This repository hosts unofficial and unsupported headers for Heterogenious System Architecture (HSA) libraries.

The headers let you write applications which directly access low-level HSA interface, bypassing the OpenCL framework on top of it.

These headers are **intended only for research**. There is no guarantee that the API are stable and will not change in a  public release of the official HSA SDK and HSA specs. Use it in production code only if you absolutely hate yourself and your users.

How did we get it
-----------------

These headers were assembled from multiple open publications and then refined by trial and error. Here are the sources:

* Tutorials on HSA from Hot Chips 25 (August 2013):
  * [HSA Introduction](http://www.slideshare.net/hsafoundation/hsa-intro-hot-chips2013-final) by Phil Rogers (AMD)
  * [HSAIL Virtual Parallel ISA](http://www.slideshare.net/hsafoundation/hsa-hsail-hot-chips2013-final2) by Ben Sander (AMD)
  * [HSA Memory Model](http://www.slideshare.net/hsafoundation/hsa-memory-model-hotchips-2013) by Benedict Gaster (Qualcomm)
  * [HSA Queuing](http://www.slideshare.net/hsafoundation/hsa-queuing-hot-chips-2013) by Ian Bratt (ARM)
* Presentations on AMD Fusion Developer Summit (November 2013):
  * [HSA Enablement of Aparapi](http://www.slideshare.net/DevCentralAMD/cc4001-garyfrost) by Gary Frost and Vignesh Ravi (AMD)
  * [HSA and Fabric Engine](http://www.slideshare.net/DevCentralAMD/pl4050-an-introduction-to-spir-for-opencl-application-developers-and-compiler-developers-by-peter-zion) by Peter Zion (Fabric Engine)
* HSA Instruction Set emulator (interface details do not exactly match AMD HSA libraries, but the high-level structure is the same)
  * HSA API defined in [newcore.h](https://github.com/HSAFoundation/HSAIL-Instruction-Set-Simulator/blob/master/include/newcore.h) header
  * Example of HSA API usage in [aqlDemo.cc](https://github.com/HSAFoundation/HSAIL-Instruction-Set-Simulator/blob/master/demo/aqlDemo.cc) demo
* Okra HSA interface:
  * The ELF DSO [libnewhsacore64.so](https://github.com/HSAFoundation/Okra-Interface-to-HSA-Device/blob/master/okra/dist/bin/libnewhsacore64.so) in Okra distribution has default visibility for internal C++ methods, which have parameter types mangled into their symbol names.
  * The ELF DSO [libokra_x86_64.so](https://github.com/HSAFoundation/Okra-Interface-to-HSA-Device/blob/master/okra/dist/bin/libokra_x86_64.so) includes Dwarf records with descriptions of most useful HSA structures.

System requirements
-------------------

You will need to use AMD OpenCL 2.0 SDK with special drivers from here

We successfully used the headers on the following system configuration

* AMD A10-7850K APU (probably works with different models - we didn't test)
* ASUS A8X Pro motherboard with the recent BIOS (this limitation is important: we couldn't make it work on a Gigabyte GA-F2A88XM-D3H motherboard).
* 32GB (4 x 8GB) G.SKILL Ares Series DDR3 2133 (Model F3-2133C10Q-32GAB). The RAM choice doesn't seem important: HSA drivers run with half of RAM or different modules just fine.
* Cleanly installed 64-bit Windows 8.1 (AMD provides HSA driver only for this Windows version). Clean install is important.
  *  Probably you could use Ubuntu instead. See below.
* Beta OpenCL 2.0 drivers with HSA support. [Downdload here](http://developer.amd.com/tools-and-sdks/opencl-zone/opencl-tools-sdks/opencl-1-2-beta-driver/) and follow the instructions inside to enable HSA runtime.
  *  Alternatively, AMD provides HSA driver for Ubuntu [here](https://github.com/HSAFoundation/Linux-HSA-Drivers-And-Images-AMD). We never tried and don't know if these headers would work on Linux. Also, [these instructions](https://code.google.com/p/aparapi/wiki/SettingUpLinuxHSAMachineForAparapi) might be helpful.

How to compile code for HSA
---------------------------

There are two options to assemble code for HSA:

* Use open-source HSAIL assembler from HSA Foundation. Clone it from [GitHub repository](https://github.com/HSAFoundation/HSAIL-Tools), switch to `hsail0.98plus` branch, and compile. Important: other branches do not work with the AMD HSA driver on Windows. The situation on Ubuntu might be different.
* Compile and OpenCL kernel and save the produced image with `clGetProgramInfo(..., CL_PROGRAM_BINARIES, ...)`. You may use [`clcc`](http://clcc.sourceforge.net/) for that. The entry points of the resulting program will have three additional argument in the beginning of the argument list. They denote the global offset returned by [`get_global_offset`](http://www.khronos.org/registry/cl/sdk/1.2/docs/man/xhtml/) whenever it is called in the kernel (but the arguments are passed even if the kernel does not call `get_global_offset`.
