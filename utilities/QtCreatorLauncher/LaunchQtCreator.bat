@echo off
reg add "HKCU\Software\Microsoft\Command Processor" /v AutoRun /t reg_expand_sz /d "chcp 1251 1>nul" /f
C:\Qt\Tools\QtCreator\bin\qtcreator.exe
reg delete "HKCU\Software\Microsoft\Command Processor" /v AutoRun /f