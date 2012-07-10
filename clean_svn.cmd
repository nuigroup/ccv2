#Pause
for /f "tokens=* delims=" %%i in ('dir /s /b /a:d *svn') do (rd /s /q "%%i")
for /f "tokens=* delims=" %%i in ('dir /s /b /a:d *hg') do (rd /s /q "%%i")
for /f "tokens=* delims=" %%i in ('dir /s /b /a:d *obj') do (rd /s /q "%%i")
attrib /s -r -h -s "*.DS_Store"
del /q /s "*.DS_Store"
del /q /s "Desktop.ini"
pause