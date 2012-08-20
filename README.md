Community Core Vision 2.0
=====================

Introduction
------------
nuiFramework is a cross platform/cross device framework for rapidly creating and testing interaction scenarios in natural user interfaces. 
It allows for general purpose signal processing from multiple sensor sources and processing them into global events and actions. 
Each platform will have its own specific runtimes and communication methods dependent on best available options. 
Our first implementation will be focused on Computer Vision (CCV).

Libs not included in package
------------
Not all the libraries necessary to build ccv2 are located in libs/3rdParty folder.
Some of them should be obtained and built separately.

1. You should have boost 1.50.0 located in libs/3rdParty/boost_1_50_0.  Build by running libs/3rdparty/boost/build_boost.bat
2. You should have opencv 2.4.0 library located in libs/3rdParty/opencv
3. You should build ptypes library with the solution provided in libs/3rdParty/ptypes-2.1.1/win32libs
