C:\php-sdk\bin\phpsdk_setvars.bat
cd C:\php-sdk\php55dev\vc11\x64\php*\
buildconf
cscript /nologo configure.js --disable-all --enable-cli --enable-example=shared --disable-zts
nmake clean
nmake
nmake install