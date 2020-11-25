# !/bin/bash

index=0

FILES=./*.pipo

for f in $FILES
do
    
    $COMPILER -o /dev/null $f 2> /dev/null > /dev/null
    
    if [ $? != 0 ]
    then
        echo "File #"$index":" $f "cause an error as expected"
    else
        echo "An error was expected :C. File" $f
        exit 1
    fi

    index=`expr $index + 1`

done