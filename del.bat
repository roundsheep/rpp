
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

del /F /Q .\proj\rpp_win\*.vsp
rem del /F /Q .\proj\rpp_win\*.filters
del /F /Q .\proj\rpp_win\*.psess
del /F /Q .\proj\rpp_win\*.sdf
del /F /Q .\proj\rpp_win\*.ncb
attrib -R -S -A -H .\proj\rpp_win\*.suo
del /F /Q .\proj\rpp_win\*.suo
del /F /Q .\proj\rpp_win\*.user

rd /S /Q .\proj\rpp_win\debug
rd /S /Q .\proj\rpp_win\Release

del /F /Q .\proj\*.vsp
rem del /F /Q .\proj\*.filters
del /F /Q .\proj\*.psess
del /F /Q .\proj\*.sdf
del /F /Q .\proj\*.ncb
attrib -R -S -A -H .\proj\*.suo
del /F /Q .\proj\*.suo
del /F /Q .\proj\*.user

rd /S /Q .\proj\debug
rd /S /Q .\proj\Release

pause