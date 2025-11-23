#!/bin/bash

add_comma=0
echo "["
# Loop through all .json files in the current directory
for file in $(ls langevin_et_al_1993/); do
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
        echo -e  "\t\t\"tags\":[\"N${file:1:2}\"]"
        echo -e  -n "\t}"
        add_comma=1
    fi
done
echo ""
echo "]"
