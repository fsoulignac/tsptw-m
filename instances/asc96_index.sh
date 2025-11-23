#!/bin/bash

add_comma=0
echo "["
# Loop through all .json files in the current directory
for file in $(ls ascheuer_1996/); do
    # Skip 'index.json'
    if [ "$file" != "index.json" ]; then
        if [ $add_comma -eq 1 ]; then
            echo ","
        fi

        echo -e "\t{"
        # Remove the .json extension
        filename_without_extension="${file%.json}"
        
        # Output both filename with and without .json and "pepe"
        echo -e  "\t\t\"instance_name\":\"$filename_without_extension\","
        echo -e  "\t\t\"file_name\":\"$file\","
        echo -e  "\t\t\"tags\":[\"N${file:3:3}\", \"$filename_without_extension\"]"
        echo -e  -n "\t}"
        add_comma=1
    fi
done
echo ""
echo "]"
