# A file with helper code for farsa tree building

function(add_farsa_version targetName)
	set_target_properties(${targetName} PROPERTIES VERSION ${FARSA_VER_MAJ}.${FARSA_VER_MIN}.${FARSA_VER_PAT} SOVERSION ${FARSA_VER_MAJ}.${FARSA_VER_MIN})
endfunction()