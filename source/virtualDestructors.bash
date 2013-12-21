#!/bin/bash

arg=$1

#set whether in fix or report mode
if [[ $arg == "fix" ]]; then
	fix=1
else
	fix=0
fi

#create a temp file to read from
input=T1.txt
#make sure not overwriting something that already exists
if [ -e $input ]; then
	echo T1.txt already exists
	input=temp_file_T1_01f23c0.txt #random name. shouldnt match anything, except itself
fi
	
# get a list of *file* *line* *"class"* *className* (Should get a list of all classes and what file they are in)
grep -RnE '^\s*class [a-zA-Z]+[^;]*$' * | cut -d: -f1,2,3 | sed 's/:/ /g' | sed 's/{//g' | sed "s/[ \t]\{1,\}/ /g" > $input

inComment=false
#go through each line
while read file line class className
do
	#check to see if it is in comments
	commentOpen=`grep -nE '/\*' $file | cut -d: -f1`
	commentClose=`grep -nE '\*/' $file | cut -d: -f1`
	# look for the largest commentOpen line smaller than line
	# look for the smallest commentClose line larger than the above commentOpen line
	# if line is between the two, it is commented out
	
	if [[ $commentOpen != "" ]]; then
		openLine=0
		for openNumber in $commentOpen
		do
			if [[ $openNumber -lt $line ]]; then
				openLine=$openNumber
			elif [[ $openLine -eq 0 ]]; then
				openLine=$openNumber
			fi
		done
		smaller=1
		closeLine=0
		for closeNumber in $commentClose
		do
			#smaller is 1 while the numbers are less than the openCommentLine
			# at the first larger number, set smaller to 0 and pick that line
			# as the next largest line. Could be equal to openLine.
			if [[ $closeNumber -lt $openLine ]]; then
				smaller=1
			elif [[ $smaller == 1 ]]; then
				smaller=0
				closeLine=$closeNumber
			fi
		done
		if [[ $line -ge $openLine ]]; then
			if [[ $line -le $closeLine ]]; then
				inComment=true
				echo $className $file
				echo its in a comment
			else 
				inComment=false
			fi
		else
			inComment=false
		fi
	fi

	# look for the destructor
	destructor=`grep -E "~$className" $file`
	#disregard qjson, muParser, and qttelnet classes
	if `echo ${file} | grep -E "qjson|qttelnet|muParser" 1>/dev/null 2>&1`; then
		echo $file > /dev/null
	# disregard classes in comments
	elif [[ "$inComment" == "true" ]]; then
		echo $className in comment $inComment
		echo $file > /dev/null
	# if there is no destructor, need to add it.
	elif [[ $destructor == "" ]]; then
		#has no destructor, look for a constructor don't match new or return statements or inline descendent constructors
		constructorLine=`grep -nE "[^a-zA-Z]$className\(.*$" $file | cut -d: -f1-2 | grep -E "$className\(" | grep -vE "new $className|return $className" | cut -d: -f-1 | head -1`
		if [[ $constructorLine == "" ]]; then
			#check for template constructors
			constructorLine=`grep -nE "[^a-zA-Z]$className<.*>\(.*$" $file | cut -d: -f-1 | head -1`
		fi
		#does the constructor exist?
		privateConstructor=false
		echo searched for constructor in $className in $file
		if [[ $constructorLine != "" ]]; then
			echo constructor line not empty
			#is the constructor public? because the destructor needs to be
			# it is private if there is a private after class line but before constructor line
			# and there is no public line in between
			privateLine=`grep -nE "private:" $file | cut -d: -f1`
			publicLine=`grep -nE "public:" $file | cut -d: -f1`
			private=0
			publicSelected=false
			publicLocation=1
			if [[ $privateLine -ne "" ]]; then
				echo has private
				for privateLines in $privateLine
				do
					if [[ $privateLines -gt $line ]]; then
						if [[ $privateLines -lt $constructorLine ]]; then
							private=$privateLines
							privateConstructor=true
							echo constructor is private
						fi	
					fi
				done
				if [[ $private -ne 0 ]]; then
					for publicLines in $publicLine
					do
						if [[ $publicSelected == false ]]; then
							if [[ $publicLine -gt $line ]]; then
								publicSelected=true
								publicLocation=$publicLine
								echo going to place on line $publicLocation if private constructor
							fi
						fi
						if [[ $publicLines -gt private ]]; then
							if [[ $publicLines -lt $constructorLine ]]; then
								privateConstructor=false
								echo actually not
							fi
						fi
				
					done
				fi
			fi
			# is it in fix mode?
			if [[ $fix == 1 ]]; then
				if [[ $constructorLine == "" ]]; then
					echo Error - ConstructorLine blank
				elif [[ $privateConstructor == true ]]; then
					echo adding destructor at $publicLocation in $file \(private constructor\)
					sed -i "$publicLocation a\ \tvirtual ~$className();" $file
				else
					#grep to the constructor, add the virtual destructor on the line above
					echo adding destructor at $constructorLine in $file
					sed -i "$constructorLine i\ \tvirtual ~$className();" $file
				fi
				#as well, add the destructor to the .cpp file
				if [[ `echo $file | grep "\.h"` ]]; then
					cFile=${file:0:-1}cpp
				else
					#destructor was not declared in the .h file
					echo "Destructor declared in .cpp"
					if [[ `echo $file | grep "\.cpp"` ]]; then
						cFile=$file
					fi
				fi
				# does the .cpp file exist?
				if [[ -e $cFile ]]; then
					#make sure the destructor is not already declared
					if ! [[ `grep -E "$className::~$className" $cFile` ]]; then
							#find the constructor
							definitionConstructorLine=`grep -nE "$className::$className" $cFile | head -1 | cut -d: -f-1`
							if [[ $definitionConstructorLine == "" ]]; then
							#could not find the constructor
								#add it to the end
								sed -i "$ a\ $className::~$className(){}" $cFile
							else
							
								#add it to the line above the constructor
								sed -i "$definitionConstructorLine i\ $className::~$className(){}" $cFile
							fi
						
					else
						echo "Destructor already exists"
					fi
				else
					echo $cFile does not exist
					#declare it in line
					sed -i "s/~$className();/~$className(){}/g" $file	
				fi
			fi
		else #has no destructor and no constructor
			echo $file $className
			echo has no constructor
		fi
		
	else
		#the destructor exists, see if it is virtual
		if ! `echo ${destructor} | grep "virtual" 1>/dev/null 2>&1`; then
			
			echo $file $line
			echo $className
			echo "Destructor needs to be virtual" 
			destructorLine=`grep -nE "~$className" $file | cut -d: -f-1`
			echo "Destructor on line: $destructorLine"
			#just add virtual in front of the destructor
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
