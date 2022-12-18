cmake -BBUILD . -A Win32
cmake --build BUILD --config Release
IF EXIST BUILD\Release\TextShaping.dll DEL /f BUILD\Release\TextShaping.dll
copy BUILD\Release\azangara-mod.dll BUILD\Release\TextShaping.dll