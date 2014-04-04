#!/bin/bash

declare -a arr

ls dumps/buckets > temp/listfile

readarray -t arr < temp/listfile

#This does a simple sort on each bucket and saves it in the sorted file and deletes the original file.
for i in "${arr[@]}"
do
	if [[ -z "$i" ]] || [[ "$i" == " " ]]; then
		echo $i
    else
		echo $i
		sort -i -s -k1,1 dumps/buckets/$i > dumps/sorted/$i
	fi	
	#rm dumps/buckets/$i
done

