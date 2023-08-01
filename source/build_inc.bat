@echo off 
set /p var= <..\source\build.h
set /a var= %var:~14,5%+1
echo #define BUILD %var% >..\source\build.h
echo Build Number: %var%
