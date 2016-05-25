cd C:\php-sdk\
setenv /xp /x86 /release
C:\php-sdk\bin\phpsdk_setvars.bat
cd C:\php-sdk\php54dev\vc9\x86\php*\
buildconf
cscript /nologo configure.js --disable-all --enable-cli --enable-example=shared --disable-zts
nmake clean
nmake
nmake install
