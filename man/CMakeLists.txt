cmake_minimum_required(VERSION 2.8)
project(objectivemoon)


FIND_PROGRAM(GZIP_TOOL
	NAMES gzip
	PATHS /bin
	/usr/bin
	/usr/local/bin)
IF(NOT GZIP_TOOL)
       MESSAGE(FATAL_ERROR "Unable to find 'gzip' program") 
ENDIF(NOT GZIP_TOOL)

SET(objectivemoon_man_src mooner.1 moonpasswd.1 moontop.1)

# Compression of the manuals
FOREACH(man ${objectivemoon_man_src})
  ADD_CUSTOM_COMMAND(OUTPUT ${man}.gz
                     COMMAND ${GZIP_TOOL} -c ${man} > ${man}.gz
                     DEPENDS ${man}
                     COMMENT "Building ${man}.gz")
ENDFOREACH(man)

# Installation of the manuals

FOREACH(man ${objectivemoon_man_gzi})
	 INSTALL(FILES ${man}.gz
         	       DESTINATION "share/man/man1"
        	       PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)
ENDFOREACH(man)


