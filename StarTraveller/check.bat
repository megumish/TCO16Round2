set /A NUM = 2001
java -jar Tester.jar -exec "..\Release\StarTraveller.exe" -seed %NUM% -save output%NUM%.png
@pause