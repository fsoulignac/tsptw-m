#!/bin/bash

add_comma=0
echo "["
# Loop through all .json files in the current directory
for file in $(ls gendreau_et_al_1998/); do
    # Skip 'index.json'
    if [ "$file" != "index.json" ]; then
        if [ $add_comma -eq 1 ]; then
            echo ","
        fi

        echo -e "\t{"
        # Remove the .json extension
        filename_without_extension="${file%.json}"

	N=$(echo "$filename_without_extension" | sed -E 's/^n([a-zA-Z0-9]+)w([a-zA-Z0-9]+)\.00([a-zA-Z0-9]+)$/\1/')
	W=$(echo "$filename_without_extension" | sed -E 's/^n([a-zA-Z0-9]+)w([a-zA-Z0-9]+)\.00([a-zA-Z0-9]+)$/\2/')
	I=$(echo "$filename_without_extension" | sed -E 's/^n([a-zA-Z0-9]+)w([a-zA-Z0-9]+)\.00([a-zA-Z0-9]+)$/\3/')

        
        # Output both filename with and without .json and "pepe"
        echo -e  "\t\t\"instance_name\":\"$filename_without_extension\","
        echo -e  "\t\t\"file_name\":\"$file\","
        echo -e  "\t\t\"tags\":[\"N$N\", \"W$W\", \"I$I\", \"$filename_without_extension\"]"
        echo -e  -n "\t}"
        add_comma=1
    fi
done
echo ""
echo "]"
