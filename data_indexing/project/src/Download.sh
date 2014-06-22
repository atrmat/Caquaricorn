#!/bin/bash

if [ $# -ne 3 ];
then
	echo "Usage: ./download.sh #_of_gram from_# #_of_items"
	exit 1
fi
for ((i = $2; i < $3; i++));
do
	axel -a -n 16 -s 2097152 http://storage.googleapis.com/books/ngrams/books/googlebooks-eng-all-$1gram-20090715-$i.csv.zip ;
#unzip googlebooks-eng-all-3gram-20090715-$i.csv.zip
#rm googlebooks-eng-all-3gram-20090715-$i.csv.zip ;
done;
