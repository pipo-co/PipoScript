# !/bin/bash

index=0

FILES=./*.pipo
error=0
for f in $FILES
do
    
    $COMPILER $f 2> /dev/null > /dev/null
    error=$?
    if [ "$error" != 0 ]
    then
        echo "File #"$index":" "Error Code: " $error" "$f "cause an error as expected"
    else
        echo "\nAn error was expected :C. \nFile" $f "\n"
        exit 1
    fi

    index=`expr $index + 1`

done