@echo off
rem ---------------------------------------------------------------------------
rem File  : regression/win32-regress.bat
rem Date  : Tuesday, May 31, 2016, 14:48 pm
rem Author: Kelly Thompson
rem Note  : Copyright (C) 2016-2018, Los Alamos National Security, LLC.
rem         All rights are reserved.
rem ---------------------------------------------------------------------------

rem This file copied from c:\program files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat.  
rem It establishes a Visual Studio environment in a command prompt.  The 
rem Windows shortcut runs the following command:
rem
rem %comspec% /[k|c] @call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86 %*
rem
rem This fill is called from win32-regression-master.bat so that all outpout
rem can be captured in a log file.

:setupvs17commenv
rem 32-bit builds ==> x86
@call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86 %*

rem -------------------------------------------------------------------------------------------
rem The main regression script starts here.
rem -------------------------------------------------------------------------------------------

:vendorsetup
call e:\work\vendors\setupvendors.bat

:cdash
rem set dashboard_type=Experimental
set dashboard_type=Nightly
set base_dir=e:\regress
set comp=cl
set ctestparts=Configure,Build,Test,Submit

rem print some information
echo Environment:  > %logdir%\environment.log 2>&1
rem echo .
set > %logdir%\environment.log 2>&1
rem echo .
rem echo -----     -----     -----     -----     -----

rem goto :jayennedebug

rem -------------------------------------------------------------------------------------------
:dracodebug

set subproj=draco
set build_type=Debug
set script_name=Draco_Win32.cmake
set script_dir=e:\regress\draco\regression
set work_dir=%base_dir%\cdash\%subproj%\%dashboard_type%_%comp%\%build_type%

rem navigate to the workdir
if not exist %work_dir% mkdir %work_dir%
cd /d %work_dir%

rem clear the build directory (need to do this here to avoid a hang).
rem if exist %work_dir%\build rmdir /s /q build
if not exist %work_dir%\build mkdir build
if not exist %work_dir%\source mkdir source
if not exist %work_dir%\target mkdir target

echo "ctest -VV -S %script_dir%\%script_name%,%dashboard_type%,%build_type%,%ctestparts% > %base_dir%\logs\draco-%build_type%-cbts.log"
ctest -VV -S %script_dir%\%script_name%,%dashboard_type%,%build_type%,%ctestparts% > %base_dir%\logs\draco-%build_type%-cbts.log 2>&1

rem goto :done

rem -------------------------------------------------------------------------------------------
:jayennedebug

set subproj=jayenne
set build_type=Debug
set script_name=Jayenne_Win32.cmake
set script_dir=e:\regress\jayenne\regression
set work_dir=%base_dir%\cdash\%subproj%\%dashboard_type%_%comp%\%build_type%

rem navigate to the workdir
if not exist %work_dir% mkdir %work_dir%
cd /d %work_dir%

rem clear the build directory (need to do this here to avoid a hang).
rem if exist %work_dir%\build rmdir /s /q build
if not exist %work_dir%\build mkdir build
if not exist %work_dir%\source mkdir source
if not exist %work_dir%\target mkdir target

rem run the ctest script

echo "ctest -VV -S %script_dir%\%script_name%,%dashboard_type%,%build_type%,%ctestparts% > %base_dir%\logs\%subproj%-%build_type%-cbts.log"
ctest -VV -S %script_dir%\%script_name%,%dashboard_type%,%build_type%,%ctestparts% > %base_dir%\logs\%subproj%-%build_type%-cbts.log 2>&1

rem goto :done

REM rem -------------------------------------------------------------------------------------------
:dracorelease

set subproj=draco
set build_type=Release
set script_name=Draco_Win32.cmake
set script_dir=e:\regress\draco\regression
set work_dir=%base_dir%\cdash\%subproj%\%dashboard_type%_%comp%\%build_type%

rem navigate to the workdir
if not exist %work_dir% mkdir %work_dir%
cd /d %work_dir%

rem clear the build directory (need to do this here to avoid a hang).
rem if exist %work_dir%\build rmdir /s /q build
if not exist %work_dir%\build mkdir build
if not exist %work_dir%\source mkdir source
if not exist %work_dir%\target mkdir target

rem run the ctest script

echo "ctest -VV -S %script_dir%\%script_name%,%dashboard_type%,%build_type%,%ctestparts% > %base_dir%\logs\draco-%build_type%-cbts.log"
ctest -VV -S %script_dir%\%script_name%,%dashboard_type%,%build_type%,%ctestparts% > %base_dir%\logs\draco-%build_type%-cbts.log 2>&1

rem release builds are failing so stop here.
rem goto :done

REM rem --------------------------------------------------------------------------
:jayennerelease

set subproj=jayenne
set build_type=Release
set script_dir=e:\regress\jayenne\regression
set script_name=Jayenne_Win32.cmake
set work_dir=%base_dir%\cdash\%subproj%\%dashboard_type%_%comp%\%build_type%

rem navigate to the workdir
if not exist %work_dir% mkdir %work_dir%
cd /d %work_dir%

rem clear the build directory (need to do this here to avoid a hang).
rem if exist %work_dir%\build rmdir /s /q build
if not exist %work_dir%\build mkdir build
if not exist %work_dir%\source mkdir source
if not exist %work_dir%\target mkdir target

rem run the ctest script

echo "ctest -VV -S %script_dir%\%script_name%,%dashboard_type%,%build_type%,%ctestparts% > %base_dir%\logs\%subproj%-%build_type%-cbts.log"
ctest -VV -S %script_dir%\%script_name%,%dashboard_type%,%build_type%,%ctestparts% > %base_dir%\logs\%subproj%-%build_type%-cbts.log 2>&1

rem -------------------------------------------------------------------------------------------
:done
rem echo You need to remove -k from script launch to let this window close automatically.
echo All done.


