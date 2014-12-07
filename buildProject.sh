
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
	echo "Build project $1"

	source /home/beamline/tools/qt/startup/startup.qt-4.7.3.sh
	qmake -r $1
	make

	echo
}


#### the main process ####

if [ $# -lt 1 ] ; then
   echo "You mush provide at least one argument. Usage:"
   echo "buildProject.sh [-c] [-p projectname.pro]"
   exit 1
fi
 
while getopts ":cp:" opt; do
	case $opt in
		c)
			echo "-c was triggered. Make a clean build"
			cleanBuild
			;;
      
		p)
			echo "-p was triggered, project name: $OPTARG"
			project_name=$OPTARG
			;;
   	  				
 		:)
			echo "Option -$OPTARG requires an argument."
			exit 1
			;;
			
		\?)
			echo "Invalid option: -$OPTARG."
			exit 1
			;;

	esac
done 

if [ "$project_name" == "" ]; then
	echo "No project name provided. Stop building."
	exit 1
fi

buildProject $project_name

