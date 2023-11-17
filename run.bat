@echo off

rem Run C++ program
CppMeasurements.exe

rem Run Python program
python PythonMeasurements.py

rem Run Java program
java -Djava.compiler=NONE JavaMeasurements

pause
