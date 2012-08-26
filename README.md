![Header Image](http://nuigroup.com/create/960-180-111111-ccv2)

Community Core Vision 2.0
=====================

Introduction
------------
Community Core Vision 2.0 is build on the nuiFramework - A cross platform/cross device framework for rapidly creating and testing interaction scenarios in natural user interfaces. It allows for general purpose signal processing from multiple sensor sources and processing them into global events and actions.
Each platform will have its own specific runtime and communication methods dependent on best available options. 
Our first implementation will be focused on Computer Vision (CV).

Components
------------
 - Core Engine - Pure C++ portable module engine.
 - CV Module Library - Base Modules for vision related applications.
 - Chrome Extension - Used for testing (TDD) of the JSON RPC API. (Requires Chrome Canary)
 - Example Client - An example client API implementation in C#.

Compiling 
------------
We currently recommend using VS2010 to compile. The following solutions are provided:

  - **nuiServerApplication** (Compiles main server engine executable)
  - **nuiCVModuleLibrary** (Compiles associated CV modules)
     - **Dependencies:** nuiModule.lib/nuiSystem.lib

Changelog
------------
View CHANGES.md for latest updates.

Links
------------
 - http://nuigroup.com - NUI Group Community Homepage
 - http://ccv.nuigroup.com - CCV 1.* Homepage
 - http://gsoc.nuigc.com/about/projects - GSoC Projects
