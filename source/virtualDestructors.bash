#!/bin/bash

arg=$1

if [[ $arg == "fix" ]]; then
	fix=1
else
	fix=0
fi

input=T1.txt
if [ -e $input ]; then
	echo T1.txt already exists
	input=temp_file_T1_01f23c0.txt #random name. shouldnt match anything, except itself
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
		if [[ $fix == 1 ]]; then
			constructorLine=`grep -nE "$className\(.*;[^}]*$" $file | cut -d: -f-1 | tail -1`
			echo "Constructor on line $constructorLine"
			if [[ $constructorLine != "" ]]; then
				#grep to the constructor, add the virtual destructor on the next line
				#sed -i "$constructorLine a\ \tvirtual ~$className();" $file
				#as well, add the destructor to the .cpp file
				if [[ `echo $file | grep "\.h"` ]]; then
					echo "Destructor declared in .h"
					cFile=${file:0:-1}cpp
					if [[ -e $cFile ]]; then
						echo $cFile Exists
						if ! [[ `grep -E "~$className" $cFile` ]]; then
							echo "Destructor does not exist"
								definitionConstructorLine=`grep -nE "$className::$className" $cFile | tail -1 | cut -d: -f-1`
								#add it to the line above
								#sed -i "$definitionConstructorLine i\$className::~$className{}" $cFile
							echo "Constructor is on line $definitionConstructorLine "
							
						else
							echo "Destructor already exists"
						fi
					else
						echo $cFile does not exist
						#declare it in line
					fi
				else
					echo "Destructor declared in .cpp"
				fi
			else
				echo "Class has no constructor, does not need a destructor"
			fi
		fi
		echo "Done empty destructor"
	else
		if ! `echo ${destructor} | grep "virtual" 1>/dev/null 2>&1`; then
			
			echo $file $line
			echo $className
			echo "Destructor needs to be virtual" 
			destructorLine=`grep -nE "~$className" $file | cut -d: -f-1`
			echo "Destructor on line: $destructorLine"
			if [[ $fix == 1 ]]; then
				sed -i "s/~$className/virtual ~$className/g" $file	
			fi	
				
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

if [[ $fix == 1 ]]; then
	echo "in fix mode"
else
	echo "in report mode"
fi
