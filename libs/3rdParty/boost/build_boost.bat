cd ..\boost_1_50_0
CALL bootstrap.bat
b2 toolset=msvc-10.0 variant=debug link=static threading=multi stage