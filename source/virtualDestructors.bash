#!/bin/bash

input=T1.txt
if [ -e $input ]; then
	echo T1.txt already exits
	input=temp_file_T1_afjslksekjlskdf.txt #random name. shouldnt match anything ever. except itself
fi
	

grep -RnE '^\s*class [a-zA-Z]+[^;]*$' * | cut -d: -f1,2,3 | sed 's/:/ /g' | sed 's/{//g' | sed "s/[ \t]\{1,\}/ /g" > $input

while read file line class className
do
	destructor=`grep -E "~$className" $file`
	if `echo ${file} | grep -E "qjson|qttelnet" 1>/dev/null 2>&1`; then
		echo $file
		echo "its a qjson or qttelnet"
	elif [[ $destructor == "" ]]; then
		echo $file $line
		echo $className
		echo "No Destructor"
		#grep to the constructor, add the virtual destructor on the next line
		#as well, add the destructor to the .cpp file
	else
		if ! `echo ${destructor} | grep "virtual" 1>/dev/null 2>&1`; then
			echo $file $line
			echo $className
			echo "Destructor needs to be virtual" 
			#grep to the destructor, add virtual in front of it
			
			if `echo ${destructor} | grep '{.*}' 1>/dev/null 2>&1`; then
				echo "Destructor declared in line"
			elif `echo ${destructor} | grep ';' 1>/dev/null 2>&1`; then
				echo "Destructor declared somewhere else"
			else
				echo "Error - unknown destructor location"
			fi
		fi
	fi
done < "$input"

rm -f  $input
