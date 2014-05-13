@echo off
REM change back to the current directory (if run as administrator)
cd %~dp0
echo You may need to specify a 32-bit installation of java if you get an UnsatisfiedLinkError
echo Make sure this and ChaosLauncher are running as Administrator!
REM start the java bot
java -jar jnibwapi.jar
pause
