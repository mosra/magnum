#include "Magnum/AbstractShaderProgram.h"
#include "Magnum/Buffer.h"
#include "Magnum/Context.h"
#include "Magnum/Extensions.h"
#include "Magnum/Mesh.h"
#include "Magnum/Texture.h"
#include "Magnum/TextureFormat.h"

using namespace Magnum;

std::tuple<Mesh, Buffer, Buffer> importSomeMesh();

struct Foo {
    void setSomeBuffer(GLuint);
    GLuint someBuffer();
} externalLib;

void foo();
void foo() {

{
/* [nocreate] */
Mesh mesh{NoCreate};
Buffer vertices{NoCreate}, indices{NoCreate};
std::tie(mesh, vertices, indices) = importSomeMesh();
/* [nocreate] */
}

{
char someData[1];
/* [transfer] */
/* Transferring the instance to external library */
{
    Buffer buffer;
    buffer.setData(someData, BufferUsage::StaticDraw);
    GLuint id = buffer.release();
    externalLib.setSomeBuffer(id); /* The library is responsible for deletion */
}

/* Acquiring an instance from external library */
{
    GLuint id = externalLib.someBuffer();
    Buffer buffer = Buffer::wrap(id, ObjectFlag::DeleteOnDestruction);
    /* The buffer instance now handles deletion */
}
/* [transfer] */
}

#ifndef MAGNUM_TARGET_GLES
{
struct: AbstractShaderProgram {} someShader;
/* [state] */
Buffer buffer;
Mesh mesh;
// ...
mesh.draw(someShader);

{
    /* Entering a section with 3rd-party OpenGL code -- clean up all state that
       could cause accidental modifications of our objects from outside */
    Context::current().resetState(Context::State::EnterExternal);

    /* Raw OpenGL calls */
    glBindBuffer(GL_ARRAY_BUFFER, buffer.id());
    glBufferStorage(GL_ARRAY_BUFFER, 32768, nullptr, GL_MAP_READ_BIT|GL_MAP_WRITE_BIT);
    // ...

    /* Exiting a section with 3rd-party OpenGL code -- reset our state tracker */
    Context::current().resetState(Context::State::ExitExternal);
}

/* Use the buffer through Magnum again */
auto data = buffer.map(0, 32768, Buffer::MapFlag::Read|Buffer::MapFlag::Write);
// ...
/* [state] */
static_cast<void>(data);
}
#endif

#ifndef MAGNUM_TARGET_GLES
{
/* [extensions] */
TextureFormat format;
if(Context::current().isExtensionSupported<Extensions::GL::ARB::depth_buffer_float>())
    format = TextureFormat::DepthComponent32F;
else
    format = TextureFormat::DepthComponent24;
/* [extensions] */
static_cast<void>(format);
}
#endif

#if !(defined(MAGNUM_TARGET_WEBGL) && defined(MAGNUM_TARGET_GLES2))
{
/* [dsa] */
Texture2D texture;

/* - on OpenGL 4.5+/ARB_direct_state_access this calls glTextureStorage2D()
   - if EXT_direct_state_access is available, calls glTextureStorage2DEXT()
   - on OpenGL 4.2+/ARB_texture_storage and OpenGL ES 3.0+ calls glTexStorage2D()
   - on OpenGL ES 2.0 with EXT_texture_storage calls glTexStorage2DEXT()
   - otherwise emulated using a sequence of four glTexImage2D() calls */
texture.setStorage(4, TextureFormat::RGBA8, {256, 256});
/* [dsa] */
}
#endif

}
