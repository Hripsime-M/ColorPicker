PROJECT( color_picker )

SET( PROGRAMS main )

FIND_PACKAGE( GLUT )
FIND_PACKAGE( OPENGL )

FOREACH( program ${PROGRAMS} )
  ADD_EXECUTABLE( ${program} ${program}.cpp )
  TARGET_LINK_LIBRARIES( ${program} ${GLUT_LIBRARY} ${OPENGL_LIBRARY} )
ENDFOREACH( program )
