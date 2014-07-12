#!/bin/bash

# Create the subject folder
cd ./data/

mkdir $1
if [[ $? -ne 0 ]] ; then
	exit 1
fi

cd ..

# Run the series of experiments
trials=40
for i in {1..6}
do
	log=./data/$1/$i.log
	echo Executing: ./../flanker/flanker ${trials} ${log}
	./../flanker/flanker ${trials} ${log}
	if [[ i -ne 6 ]] ; then
		sleep 5
	fi
done
