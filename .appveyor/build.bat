@echo on
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86
mkdir opt32 && cd opt32
C:\python27\python.exe ..\configure.py --enable-optimize
C:\python27\python.exe C:\python27\Scripts\ambuild
cd ..
opt32\tests\testrunner\testrunner.exe
