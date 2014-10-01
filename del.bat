
del /F /Q .\bin\*.ilk
del /F /Q .\bin\*.pdb

del /F /Q .\*.ncb
attrib -R -S -A -H .\*.suo
del /F /Q .\*.suo
del /F /Q .\*.user

rd /S /Q .\debug
rd /S /Q .\Release

del /F /Q .\2005\*.ncb
attrib -R -S -A -H .\2005\*.suo
del /F /Q .\2005\*.suo
del /F /Q .\2005\*.user

rd /S /Q .\2005\debug
rd /S /Q .\2005\Release

del /F /Q .\2008\*.ncb
attrib -R -S -A -H .\2008\*.suo
del /F /Q .\2008\*.suo
del /F /Q .\2008\*.user

rd /S /Q .\2008\debug
rd /S /Q .\2008\Release


attrib -R -S -A -H .\2010\*.suo
del /F /Q .\2010\*.suo
del /F /Q .\2010\*.user
rd /S /Q .\2010\debug
rd /S /Q .\2010\Release

del /F /Q .\2012\*.vsp
rem del /F /Q .\2012\*.filters
del /F /Q .\2012\*.psess
del /F /Q .\2012\*.sdf
del /F /Q .\2012\*.ncb
attrib -R -S -A -H .\2012\*.suo
del /F /Q .\2012\*.suo
del /F /Q .\2012\*.user

rd /S /Q .\2012\debug
rd /S /Q .\2012\Release

del /F /Q .\rppw\*.vsp
rem del /F /Q .\rppw\*.filters
del /F /Q .\rppw\*.psess
del /F /Q .\rppw\*.sdf
del /F /Q .\rppw\*.ncb
attrib -R -S -A -H .\rppw\*.suo
del /F /Q .\rppw\*.suo
del /F /Q .\rppw\*.user

rd /S /Q .\rppw\debug
rd /S /Q .\rppw\Release

pause