set PATH=C:\MinGW\bin;%PATH%
C:\MinGW\bin\gcc main.c -Wall ext\mpc.c -o cispy && cispy.exe & del cispy.exe
