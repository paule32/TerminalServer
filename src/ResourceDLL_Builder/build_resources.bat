@echo off
:: Kompiliert die Sprach-Ressourcen als DLLs
rc /nologo /fo resources_de.res resources\resources_de.rc2
link /NOLOGO /DLL /NOENTRY /MACHINE:X86 /DEF:resources\resources_de.def /OUT:resources\resources_de.dll resources_de.res

rc /nologo /fo resources_en.res resources\resources_en.rc2
link /NOLOGO /DLL /NOENTRY /MACHINE:X86 /DEF:resources\resources_en.def /OUT:resources\resources_en.dll resources_en.res

echo.
echo Sprach-Ressourcen wurden kompiliert.
pause
