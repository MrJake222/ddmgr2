#!/bin/bash

# output
DIR_PREFIX="/usr/local"
PROJ_NAME="ddmgr2"
DIR_BIN="${DIR_PREFIX}/bin"
DIR_CONF="${DIR_PREFIX}/etc/${PROJ_NAME}"
DIR_SERVICE="/etc/systemd/system"
FILE_EXEC="${DIR_BIN}/${PROJ_NAME}"
FILE_SCRIPT="${DIR_BIN}/${PROJ_NAME}_config.sh"
FILE_SERVICE="${DIR_SERVICE}/${PROJ_NAME}.service"

# input
IN_BUILD="build"
IN_EXEC="build/${PROJ_NAME}"
IN_SCRIPT="${PROJ_NAME}_config.sh"
IN_SERVICE="${PROJ_NAME}.service"
IN_EXAMPLES="../examples"

if [[ $# != 1 ]]; then
	echo "1 argument expected: [build/install/uninstall]"
	exit 1
fi

if [[ $1 == "build" ]]; then
    mkdir ${IN_BUILD}
    cd ${IN_BUILD}
    cmake ../..
    make -j4
    echo "build done, now run $0 install"
    
elif [[ $1 == "install" ]]; then
    if [[ ! -d ${IN_BUILD} ]]; then
        echo "run $0 build first"
        exit 1
    fi

	echo -n "installing exec in ${FILE_EXEC}... "
	cp ${IN_EXEC} ${FILE_EXEC}
	echo "done"
	
	echo -n "installing config script in ${FILE_SCRIPT}... "
	cp ${IN_SCRIPT} ${FILE_SCRIPT}
	echo "done"
	
	echo -n "installing service in ${FILE_SERVICE}... "
	cp ${IN_SERVICE} ${FILE_SERVICE}
	echo "done"

	if [[ ! -d ${DIR_CONF} ]]; then
		echo -n "conf dir ${DIR_CONF} doesn't exists, creating & copying examples... "
		mkdir ${DIR_CONF}
		cp -r ${IN_EXAMPLES}/* ${DIR_CONF}
		echo "done"
	else
		echo "conf dir exists, pass."
	fi

elif [[ $1 == "uninstall" ]]; then
	echo -n "uninstalling... "
	rm ${FILE_EXEC}
	rm ${FILE_SCRIPT}
	rm ${FILE_SERVICE}
	#rm -r ${DIR_CONF}
	echo "done"

else
	echo "invalid argument: $1"
fi

