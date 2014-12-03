
function cleanBuild() {
	echo $"Clean project files"
	
	make clean

	echo rm *.o
	rm *.o

	echo rm moc_*
	rm moc_*

	echo rm qrc_*
	rm qrc_*

	echo rm ui_*
	rm ui_*
	
	echo rm Makefile*
	rm Makefile*

	echo
}

function buildProject() {
	echo "Build project $2"

	source /home/beamline/tools/qt/startup/startup.qt-4.7.3.sh
	if [ $1 -ne 1 ]; then
		qmake -r $1
	fi
	make

	echo
}

function usage() {
	echo "Usage:"
	echo "	buildProject.sh [-c] [-m] [-h] [-p project.pro]"
	echo 
	echo "	-h help"
	echo "	-c make a clean build"
	echo "	-m make the current project. No Makefile will be updated."
	echo "	-p make the given project. Makefile will be updated."
	echo
}

#### the main process ####

if [ $# -lt 1 ] ; then
   echo "You mush provide at least one argument."
   usage
   exit 1
fi

makeOnly=0
while getopts ":cmhp:" opt; do
	case $opt in
		c)
			echo "-c was triggered. Make a clean build"
			cleanBuild
			;;
      
		p)
			echo "-p was triggered, project name: $OPTARG"
			project_name=$OPTARG
			;;

		m)
			echo "-m was triggered. Make only."
			makeOnly=1
			;;
   	  	
		h)
			usage
			exit 1
			;;		
 		:)
			echo "Option -$OPTARG requires an argument."
			usage
			exit 1
			;;
			
		\?)
			echo "Invalid option: -$OPTARG."
			exit 1
			;;

	esac
done 

if [ "$project_name" == "" ] && [ $makeOnly -ne 1 ]; then
	echo "No project name provided. Stop building."
	exit 1
fi

buildProject $makeOnly $project_name

