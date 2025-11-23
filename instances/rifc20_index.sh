#!/bin/bash

add_comma=0
echo "["
# Loop through all .json files in the current directory
for file in $(ls rifki_et_al_2020/); do
    # Skip 'index.json'
    if [ "$file" != "index.json" ]; then
        if [ $add_comma -eq 1 ]; then
            echo ","
        fi

        echo -e "\t{"
        # Remove the .json extension
        filename_without_extension="${file%.json}"
        
        IFS='_' read -r -a tags <<< "$filename_without_extension"

        # Output both filename with and without .json and "pepe"
        echo -e  "\t\t\"instance_name\":\"$filename_without_extension\","
        echo -e  "\t\t\"file_name\":\"$file\","
        echo -e  "\t\t\"tags\":[\"N${tags[1]}\", \"W${tags[3]}\", \"I${tags[4]}\", \"$filename_without_extension\"]"
        echo -e  -n "\t}"
        add_comma=1
    fi
done
echo ""
echo "]"
