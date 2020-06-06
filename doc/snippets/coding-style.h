/* [glcompatibility] */
#ifndef MAGNUM_TARGET_GLES
/**
@brief Set polygon mode

@requires_gl Polygon mode is not available in OpenGL ES.
*/
void setPolygonMode(PolygonMode mode);
#endif
/* [glcompatibility] */

/* [extension] */
/** @gl_extension{ARB,timer_query} */
/* [extension] */

/* [extension2] */
/** @gl_extension2{NV,read_buffer_front,NV_read_buffer} */
/* [extension2] */

/* [webgl_extension] */
/** @webgl_extension{EXT,color_buffer_float} */
/* [webgl_extension] */

/* [vk_extension] */
/** @vk_extension{EXT,debug_report} */
/* [vk_extension] */

/* [al_extension] */
/** @al_extension{EXT,float32}, @alc_extension{SOFT,HRTF} */
/* [al_extension] */

/* [fn_gl] */
/**
...

@see @fn_gl{Enable}/@fn_gl{Disable} with @def_gl{TEXTURE_CUBE_MAP_SEAMLESS}
*/
static void setSeamless(bool enabled) {
    enabled ? glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS) : glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
}
/* [fn_gl] */

/* [fn_gl2] */
/** @fn_gl2_keyword{CopyTextureSubImage2D,CopyTexSubImage2D} */
/* [fn_gl2] */

/* [fn_gl_extension] */
/** @fn_gl_extension{LabelObject,EXT,debug_label} */
/* [fn_gl_extension] */

/* [fn_vk] */
/**
-   @fn_vk{CreateInstance}
-   @type_vk{InstanceCreateInfo}
-   @val_vk{STRUCTURE_TYPE_INSTANCE_CREATE_INFO,StructureType}
-   @def_vk{VERSION_MINOR}
*/
/* [fn_vk] */

/* [fn_al] */
/**
...

@see @fn_al{Listenerfv} with @def_al{VELOCITY}
*/
static void setListenerVelocity(const Vector3& velocity) {
    alListenerfv(AL_VELOCITY, velocity.data());
}
/* [fn_al] */

/* [requires_gl] */
/** @requires_gl33 Extension @gl_extension{ARB,timer_query} */
/* [requires_gl] */

/* [requires_vk_extension] */
/** @requires_vk_extension Extension @vk_extension{EXT,debug_report} */
/* [requires_vk_extension] */

/* [requires_al_extension] */
/** @requires_al_extension Extension @al_extension{EXT,MCFORMATS} */
/* [requires_al_extension] */
