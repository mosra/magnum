#if (!defined(GL_ES) && __VERSION__ >= 130) || (defined(GL_ES) && __VERSION__ >= 300)
#define NEW_GLSL
#endif

/* On NVidia and GLSL 1.20 layout qualifiers result in parsing error, even if
   the extension is defined as supported */
#if !defined(GL_ES) && __VERSION__ >= 120

#ifdef GL_ARB_explicit_attrib_location
    #extension GL_ARB_explicit_attrib_location: enable
    #define EXPLICIT_ATTRIB_LOCATION
#endif
#ifdef GL_ARB_explicit_uniform_location
    #extension GL_ARB_explicit_uniform_location: enable
    #define EXPLICIT_UNIFORM_LOCATION
#endif

#endif

#if defined(GL_ES) && __VERSION__ >= 300
#define EXPLICIT_ATTRIB_LOCATION
/* EXPLICIT_UNIFORM_LOCATION is not available in OpenGL ES */
#endif

/* Precision qualifiers are not supported in GLSL 1.20 */
#if !defined(GL_ES) && __VERSION__ == 120
#define highp
#define mediump
#define lowp
#endif
