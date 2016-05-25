rem If compiling PHP 5.4, open the “Windows SDK 6.1 shell” and execute the following command in it:
rem setenv /x86 /xp /release

cd C:\php-sdk\
setenv /xp /x86 /release
C:\php-sdk\bin\phpsdk_setvars.bat
cd C:\php-sdk\php54dev\vc9\x86\php*\
buildconf
cscript /nologo configure.js --disable-all --enable-cli --enable-beanstalk=shared
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