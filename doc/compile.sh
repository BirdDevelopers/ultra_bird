#!/bin/bash

CWD=$(pwd)

mkdir ../tmp/tex
cp * ../tmp/tex
cp -r ../doc_compiled/drawings ../tmp/tex
cd ../tmp/tex

#Compile all files

for file in ${CWD}/*.tex
do
    if [[ -f $file ]]; then
        base_fname=$(basename $file)
        pdflatex -shell-escape ${base_fname}

        compiled_name=`basename "$file" | cut -d. -f1`.pdf
        cp ${compiled_name} ${CWD}/../doc_compiled/
    fi
done

cd ${CWD}