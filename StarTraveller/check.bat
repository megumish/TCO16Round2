set /A NUM = 10
java -jar Tester.jar -exec "..\Release\TCOMM_CutTheRoots.exe" -seed %NUM% -vis 
@pause