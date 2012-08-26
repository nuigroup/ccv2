Changelog Aug 25th 2012
====

Core
------------
 - Removed boost dependancies
 - Removed event/xopt dependacies
 - Cleaned and included Ptypes and OpenCV libs
 - Refactored CCV.cpp/ nuiFactor and nuiFrameworkManager to load modules from JSON instead of scanning the disk.
 - Fixed issue with JSON-RPC compilation in VS2010
 - Removed anything that should'nt be on repo (*.obj etc)
 - Moved JSON Config to data/default_config.json

Modules
------------
Removed These Modules - They will be compiled in seperate project as they require boost and other libs not required in core.

 - nuiTemporalFusionModule
 - nuiSpiritParserModule
 - nuiDebugGestureModule
 - nuiDebugInputModule
 - nuiJSONINteraction
 - nuiSpiriteParserModule
 - nuiSphinxModule

Todo
------------
Refactor module names (... Available Modules, Module Libraries and Active Modules) 
Run through Doxygen
Merge Refactor Branch 
	
		   1) nuiJSonRPCApi lines (341-346)
		   2) nuiModule (N/A) Comment Removal
		   3) nuiThread (N/A) Comment Removal
		   4) nuiDebugVideoSink (Line 28 added Try/Catch)
		   5) nuiEdgeFilterModule (Try/Catch and more OpenCV)
		   6) nuiGaussianBlurFilter (Line 68 added opencv)