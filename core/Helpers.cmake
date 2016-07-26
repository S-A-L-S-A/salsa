# A file with helper code for salsa tree building

function(add_salsa_version targetName)
	set_target_properties(${targetName} PROPERTIES VERSION ${SALSA_VER_MAJ}.${SALSA_VER_MIN}.${SALSA_VER_PAT} SOVERSION ${SALSA_VER_MAJ}.${SALSA_VER_MIN})
endfunction()
