rem Developer Command Prompt for VS2015
C:\php-sdk\bin\phpsdk_setvars.bat
cd C:\php-sdk\php70dev\vc14\x64\php*\
buildconf
cscript /nologo configure.js --disable-all --enable-cli --enable-beanstalk=shared --disable-zts
if %errorlevel% == 1 ( goto error ) else ( goto compile )
:error
echo configure.js error
echo please check the configure.js and recompile
pause
exit
:compile
nmake clean
nmake
nmake install
echo compile success!