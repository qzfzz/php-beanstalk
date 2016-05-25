rem VS2012 x86 Native Tools Command Prompt
cd C:\php-sdk\php55dev\vc11\x86\php*\
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