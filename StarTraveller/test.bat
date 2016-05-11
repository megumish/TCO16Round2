for /l %%i in (11,1,100) do java -jar Tester.jar -exec "..\Release\TCOMM_CutTheRoots.exe" -seed %%i > score\output%%i_ver2.txt
@pause