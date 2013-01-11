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

Terms and definitions for developers
------------
In the past there were great misunderstanding and mess with definitions used 
around the project. Currently the following definitions are proposed:
1. Plugin - shared library, that may contain 1 or more funcitonal modules.
2. Module - functional part of system, providing some processing functionality.
3. Pipeline - functional part of system that can consist of one or more modules.

Changelog
------------
View CHANGES.md for latest updates.

Links
------------
 - http://nuigroup.com - NUI Group Community Homepage
 - http://ccv.nuigroup.com - CCV 1.* Homepage
 - http://gsoc.nuigc.com/about/projects - GSoC Projects
