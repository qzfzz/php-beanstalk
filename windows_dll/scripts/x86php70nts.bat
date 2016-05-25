C:\php-sdk\bin\phpsdk_setvars.bat
cd C:\php-sdk\php70dev\vc11\x86\php*\
buildconf
cscript /nologo configure.js --disable-all --enable-cli --enable-example=shared
nmake clean
nmake
nmake install