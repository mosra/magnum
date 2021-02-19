#ifndef Magnum_ShaderTools_Stage_h
#define Magnum_ShaderTools_Stage_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021 Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

/** @file
 * @brief Enum @ref Magnum::ShaderTools::Stage
 * @m_since_latest
 */

#include "Magnum/Magnum.h"
#include "Magnum/ShaderTools/visibility.h"

namespace Magnum { namespace ShaderTools {

/**
@brief Shader stage
@m_since_latest

@see @ref AbstractConverter
*/
enum class Stage: UnsignedInt {
    /**
     * Unspecified stage. When used in the
     * @ref AbstractConverter::validateFile(),
     * @ref AbstractConverter::convertFileToFile() "convertFileToFile()",
     * @ref AbstractConverter::convertFileToData() "convertFileToData()",
     * @ref AbstractConverter::linkFilesToFile() "linkFilesToFile()" or
     * @ref AbstractConverter::linkFilesToData() "linkFilesToData()" APIs,
     * particular plugins may attempt to detect the stage from filename, the
     * shader stage might also be encoded directly in certain
     * @ref Format "Format"s. Leaving the stage unspecified might limit
     * validation and conversion capabilities, see documentation of a
     * particular converter for concrete behavior.
     *
     * This value is guaranteed to be @cpp 0 @ce, which means you're encouraged
     * to simply use @cpp {} @ce in function calls and elsewhere.
     */
    Unspecified = 0,

    Vertex,                     /**< Vertex stage */
    Fragment,                   /**< Fragment stage */
    Geometry,                   /**< Geometry stage */
    TessellationControl,        /**< Tessellation control stage */
    TessellationEvaluation,     /**< Tessellation evaluation stage */
    Compute,                    /**< Compute stage */

    RayGeneration,              /**< Ray generation stage */
    RayAnyHit,                  /**< Ray any hit stage */
    RayClosestHit,              /**< Ray closest hit stage */
    RayMiss,                    /**< Ray miss stage */
    RayIntersection,            /**< Ray intersection stage */
    RayCallable,                /**< Ray callable stage */

    MeshTask,                   /**< Mesh task stage */
    Mesh,                       /**< Mesh stage */

    /**
     * Compute kernel.
     *
     * Compared to @ref Stage::Compute, which is used by graphics APIs such as
     * Vulkan or OpenGL, this one is for use by OpenCL.
     */
    Kernel
};

/**
@debugoperatorenum{Stage}
@m_since_latest
*/
MAGNUM_SHADERTOOLS_EXPORT Debug& operator<<(Debug& debug, Stage value);

}}

#endif
