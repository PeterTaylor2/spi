# I have found that git is not very nice regarding line endings

echo "Converting file types to unix line endings"
echo ".h .hpp .cpp .c Makefile .git* .mk .py .txt .lex .in"

fns=$( find . -type d \( -name .git -o -name .hg -o -name .vs \) -prune -o -type f \( -name '*.h' -o -name '*.c' -o -name '*.hpp' -o -name '*.cpp' -o -name '*.txt' -o -name '*.py' -o -name '*.lex' -o -name '*.in' -o -name '*.inp' -o -name '*.cs' -o -name '*.out' -o -name '*.mk' -o -name Makefile -o -name '.git*' \) -print | xargs dos2unix -ic )

if [ -z "$fns" ]; then
    echo "No files need converting"
else
    for fn in $fns
    do
	dos2unix -k $fn
    done
fi


echo "Converting file types to dos line endings"
echo ".bas .frm .sln .csproj .vcproj .pyproj .guid .vcxproj .vcxproj.filters .bat .cmd .psess"

fns=$( find . -type d \( -name .git -o -name .hg -o -name .vs \) -prune -o -type f \( -name '*.bas' -o -name '*.frm' -o -name '*.sln' -o -name '*.psess' -o -name '*.csproj' -o -name '*.vcproj' -o -name '*.pyproj' -o -name '*.guid' -o -name '*.vcxproj' -o -name '*.vcxproj.filters' -o -name '*.bat' -o -name '*.cmd' \) -print | xargs unix2dos -ic )

if [ -z "$fns" ]; then
    echo "No files need converting"
else
    for fn in $fns
    do
	unix2dos -k $fn
    done
fi

