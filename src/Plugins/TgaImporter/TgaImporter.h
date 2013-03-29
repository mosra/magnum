#ifndef Magnum_Trade_TgaImporter_TgaImporter_h
#define Magnum_Trade_TgaImporter_TgaImporter_h
/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013 Vladimír Vondruš <mosra@centrum.cz>

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
 * @brief Class Magnum::Trade::TgaImporter::TgaImporter
 */

#include <Trade/AbstractImporter.h>

#ifndef DOXYGEN_GENERATING_OUTPUT
#ifdef _WIN32
    #ifdef TgaImporter_EXPORTS
        #define MAGNUM_TGAIMPORTER_EXPORT __declspec(dllexport)
    #else
        #define MAGNUM_TGAIMPORTER_EXPORT __declspec(dllimport)
    #endif
    #define MAGNUM_TGAIMPORTER_LOCAL
#else
    #define MAGNUM_TGAIMPORTER_EXPORT __attribute__ ((visibility ("default")))
    #define MAGNUM_TGAIMPORTER_LOCAL __attribute__ ((visibility ("hidden")))
#endif
#endif

namespace Magnum { namespace Trade { namespace TgaImporter {

/** @brief TGA importer plugin */
class MAGNUM_TGAIMPORTER_EXPORT TgaImporter: public AbstractImporter {
    public:
        /** @brief Default constructor */
        explicit TgaImporter();

        /** @brief Plugin manager constructor */
        explicit TgaImporter(Corrade::PluginManager::AbstractPluginManager* manager, std::string plugin);

        virtual ~TgaImporter();

        Features features() const override;
        bool open(std::istream& in) override;
        bool open(const std::string& filename) override;
        void close() override;
        UnsignedInt image2DCount() const override;
        ImageData2D* image2D(UnsignedInt id) override;

        #pragma pack(1)
        /** @brief TGA file header */
        /** @todoc Enable @c INLINE_SIMPLE_STRUCTS again when unclosed &lt;component&gt; in tagfile is fixed*/
        struct MAGNUM_TGAIMPORTER_LOCAL Header {
            UnsignedByte    identsize;      /**< @brief Size of ID field that follows header (0) */
            UnsignedByte    colorMapType;   /**< @brief 0 = None, 1 = paletted */
            UnsignedByte    imageType;      /**< @brief 0 = none, 1 = indexed, 2 = rgb, 3 = grey, +8=rle */
            UnsignedShort   colorMapStart;  /**< @brief First color map entry */
            UnsignedShort   colorMapLength; /**< @brief Number of colors */
            UnsignedByte    colorMapBpp;    /**< @brief Bits per palette entry */
            UnsignedShort   beginX;         /**< @brief %Image x origin */
            UnsignedShort   beginY;         /**< @brief %Image y origin */
            UnsignedShort   width;          /**< @brief %Image width */
            UnsignedShort   height;         /**< @brief %Image height */
            UnsignedByte    bpp;            /**< @brief Bits per pixel (8, 16, 24, 32) */
            UnsignedByte    descriptor;     /**< @brief %Image descriptor */
        };
        #pragma pack(8)

    private:
        ImageData2D* _image;
};

}}}

#endif
