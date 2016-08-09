# A file with helper code for salsa tree building

function(add_salsa_version targetName)
	set_target_properties(${targetName} PROPERTIES VERSION ${SALSA_VER_MAJ}.${SALSA_VER_MIN}.${SALSA_VER_PAT} SOVERSION ${SALSA_VER_MAJ}.${SALSA_VER_MIN})
endfunction()

# Simply adds a 0 in front of a number if it is smaller than 0 and returns it
# as a string (e.g. if num is 7, outStr will be "07"
function(pad_number_smaller_than_10 outStr num)
	if(${num} LESS 10)
		set(${outStr} "0${num}" PARENT_SCOPE)
	else()
		set(${outStr} "${num}" PARENT_SCOPE)
	endif()
endfunction()
