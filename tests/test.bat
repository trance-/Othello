mv -rv output output.bak
mkdir output
awk -f tests\test.awk tests\tests.lst
tests\parse.bat
