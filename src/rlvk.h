/**********************************************************************************************
*
*   rlvk v0.1 - A Vulkan abstraction layer
*
*   DESCRIPTION:
*       This is a modification of rlgl v6.0, and is not wholly original software.
*
*       #define RLVK_IMPLEMENTATION
*           Generates the implementation of the library into the included file
*           If not defined, the library is in header only mode and can be included in other headers
*           or source files without problems. But only ONE file should hold the implementation
*
*
*       #define RL_DEFAULT_BATCH_BUFFER_ELEMENTS   8192    // Default internal render batch elements limits
*       #define RL_DEFAULT_BATCH_BUFFERS              1    // Default number of batch buffers (multi-buffering)
*       #define RL_DEFAULT_BATCH_DRAWCALLS          256    // Default number of batch draw calls (by state changes: mode, texture)
*       #define RL_DEFAULT_BATCH_MAX_TEXTURE_UNITS    4    // Maximum number of texture units that can be activated on batch drawing (SetShaderValueTexture())
*
*       #define RL_MAX_MATRIX_STACK_SIZE             32    // Maximum size of internal Matrix stack
*       #define RL_MAX_SHADER_LOCATIONS              32    // Maximum number of shader locations supported
*       #define RL_CULL_DISTANCE_NEAR              0.05    // Default projection matrix near cull distance
*       #define RL_CULL_DISTANCE_FAR             4000.0    // Default projection matrix far cull distance
*
*       When loading a shader, the following vertex attributes and uniform location names are tried to be set automatically:
*       WARNING: Pre-defined names can not be changed, they are used by default shaders and all raylib examples shaders, they are just listed here for reference
*
*       #define RL_DEFAULT_SHADER_ATTRIB_NAME_POSITION     "vertexPosition"    // Bound by default to shader location: RL_DEFAULT_SHADER_ATTRIB_LOCATION_POSITION
*       #define RL_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD     "vertexTexCoord"    // Bound by default to shader location: RL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD
*       #define RL_DEFAULT_SHADER_ATTRIB_NAME_NORMAL       "vertexNormal"      // Bound by default to shader location: RL_DEFAULT_SHADER_ATTRIB_LOCATION_NORMAL
*       #define RL_DEFAULT_SHADER_ATTRIB_NAME_COLOR        "vertexColor"       // Bound by default to shader location: RL_DEFAULT_SHADER_ATTRIB_LOCATION_COLOR
*       #define RL_DEFAULT_SHADER_ATTRIB_NAME_TANGENT      "vertexTangent"     // Bound by default to shader location: RL_DEFAULT_SHADER_ATTRIB_LOCATION_TANGENT
*       #define RL_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD2    "vertexTexCoord2"   // Bound by default to shader location: RL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD2
*       #define RL_DEFAULT_SHADER_ATTRIB_NAME_BONEINDICES  "vertexBoneIndices" // Bound by default to shader location: RL_DEFAULT_SHADER_ATTRIB_LOCATION_BONEINDICES
*       #define RL_DEFAULT_SHADER_ATTRIB_NAME_BONEWEIGHTS  "vertexBoneWeights" // Bound by default to shader location: RL_DEFAULT_SHADER_ATTRIB_LOCATION_BONEWEIGHTS
*       #define RL_DEFAULT_SHADER_ATTRIB_NAME_INSTANCETRANSFORM "instanceTransform" // Bound by default to shader location: RL_DEFAULT_SHADER_ATTRIB_LOCATION_INSTANCETRANSFORM
*
*       #define RL_DEFAULT_SHADER_UNIFORM_NAME_MVP         "mvp"               // model-view-projection matrix
*       #define RL_DEFAULT_SHADER_UNIFORM_NAME_VIEW        "matView"           // view matrix
*       #define RL_DEFAULT_SHADER_UNIFORM_NAME_PROJECTION  "matProjection"     // projection matrix
*       #define RL_DEFAULT_SHADER_UNIFORM_NAME_MODEL       "matModel"          // model matrix
*       #define RL_DEFAULT_SHADER_UNIFORM_NAME_NORMAL      "matNormal"         // normal matrix (transpose(inverse(matModelView)))
*       #define RL_DEFAULT_SHADER_UNIFORM_NAME_COLOR       "colDiffuse"        // color diffuse (base tint color, multiplied by texture color)
*       #define RL_DEFAULT_SHADER_UNIFORM_NAME_BONEMATRICES "boneMatrices"     // bone matrices
*
*       #define RL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE0  "texture0"          // texture0 (texture slot active 0)
*       #define RL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE1  "texture1"          // texture1 (texture slot active 1)
*       #define RL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE2  "texture2"          // texture2 (texture slot active 2)
*
*   DEPENDENCIES:
*      - Vulkan
*      - Shaderc
*
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2014-2026 Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#include <vulkan/vulkan_core.h>
#ifndef USING_RLVK
    #error "raylib.h must be included before rlvk.h"
#endif

#ifdef USING_RLGL
    #error "Cannot include rlvk.h when using RLGL"
#endif

#ifndef RLVK_H
#define RLVK_H

#define RLVK_VERSION  "0.1"

// Function specifiers in case library is build/used as a shared library
// NOTE: Microsoft specifiers to tell compiler that symbols are imported/exported from a .dll
// NOTE: visibility(default) attribute makes symbols "visible" when compiled with -fvisibility=hidden
#if defined(_WIN32) && defined(BUILD_LIBTYPE_SHARED)
    #define RLAPI __declspec(dllexport)     // Building the library as a Win32 shared library (.dll)
#elif defined(BUILD_LIBTYPE_SHARED)
    #define RLAPI __attribute__((visibility("default"))) // Building the library as a Unix shared library (.so/.dylib)
#elif defined(_WIN32) && defined(USE_LIBTYPE_SHARED)
    #define RLAPI __declspec(dllimport)     // Using the library as a Win32 shared library (.dll)
#endif

// Function specifiers definition
#ifndef RLAPI
    #define RLAPI       // Functions defined as 'extern' by default (implicit specifiers)
#endif

// Support TRACELOG macros
#ifndef TRACELOG
    #define TRACELOG(level, ...) (void)0
#endif

// Allow custom memory allocators
#ifndef RL_MALLOC
    #define RL_MALLOC(sz)     malloc(sz)
#endif
#ifndef RL_CALLOC
    #define RL_CALLOC(n,sz)   calloc(n,sz)
#endif
#ifndef RL_REALLOC
    #define RL_REALLOC(n,sz)  realloc(n,sz)
#endif
#ifndef RL_FREE
    #define RL_FREE(p)        free(p)
#endif

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------

// Default internal render batch elements limits
#ifndef RL_DEFAULT_BATCH_BUFFER_ELEMENTS
    #define RL_DEFAULT_BATCH_BUFFER_ELEMENTS  8192
#endif
#ifndef RL_DEFAULT_BATCH_BUFFERS
    #define RL_DEFAULT_BATCH_BUFFERS                 1      // Default number of batch buffers (multi-buffering)
#endif
#ifndef RL_DEFAULT_BATCH_COUNT_PER_FRAME
    #define RL_DEFAULT_BATCH_COUNT_PER_FRAME                 16      // Default number of batches that can be drawn per frame
#endif
#ifndef RL_DEFAULT_BATCH_DRAWCALLS
    #define RL_DEFAULT_BATCH_DRAWCALLS             256      // Default number of batch draw calls (by state changes: mode, texture)
#endif
#ifndef RL_DEFAULT_BATCH_MAX_TEXTURE_UNITS
    #define RL_DEFAULT_BATCH_MAX_TEXTURE_UNITS       4      // Maximum number of textures units that can be activated on batch drawing (SetShaderValueTexture())
#endif
#ifndef RL_DEFAULT_MAX_TEXTURES
    #define RL_DEFAULT_MAX_TEXTURES  32 // Maximum number of loaded textures at once
#endif

// Internal Matrix stack
#ifndef RL_MAX_MATRIX_STACK_SIZE
    #define RL_MAX_MATRIX_STACK_SIZE                32      // Maximum size of Matrix stack
#endif

// Shader limits
#ifndef RL_MAX_SHADER_LOCATIONS
    #define RL_MAX_SHADER_LOCATIONS                 32      // Maximum number of shader locations supported
#endif

// Projection matrix culling
#ifndef RL_CULL_DISTANCE_NEAR
    #define RL_CULL_DISTANCE_NEAR                 0.05      // Default near cull distance
#endif
#ifndef RL_CULL_DISTANCE_FAR
    #define RL_CULL_DISTANCE_FAR                4000.0      // Default far cull distance
#endif

// Texture parameters (equivalent to OpenGL defines)
#define RL_TEXTURE_WRAP_S                       0x2802      // GL_TEXTURE_WRAP_S
#define RL_TEXTURE_WRAP_T                       0x2803      // GL_TEXTURE_WRAP_T
#define RL_TEXTURE_MAG_FILTER                   0x2800      // GL_TEXTURE_MAG_FILTER
#define RL_TEXTURE_MIN_FILTER                   0x2801      // GL_TEXTURE_MIN_FILTER

#define RL_TEXTURE_FILTER_NEAREST               0x2600      // GL_NEAREST
#define RL_TEXTURE_FILTER_LINEAR                0x2601      // GL_LINEAR
#define RL_TEXTURE_FILTER_MIP_NEAREST           0x2700      // GL_NEAREST_MIPMAP_NEAREST
#define RL_TEXTURE_FILTER_NEAREST_MIP_LINEAR    0x2702      // GL_NEAREST_MIPMAP_LINEAR
#define RL_TEXTURE_FILTER_LINEAR_MIP_NEAREST    0x2701      // GL_LINEAR_MIPMAP_NEAREST
#define RL_TEXTURE_FILTER_MIP_LINEAR            0x2703      // GL_LINEAR_MIPMAP_LINEAR
#define RL_TEXTURE_FILTER_ANISOTROPIC           0x3000      // Anisotropic filter (custom identifier)
#define RL_TEXTURE_MIPMAP_BIAS_RATIO            0x4000      // Texture mipmap bias, percentage ratio (custom identifier)

#define RL_TEXTURE_WRAP_REPEAT                  0x2901      // GL_REPEAT
#define RL_TEXTURE_WRAP_CLAMP                   0x812F      // GL_CLAMP_TO_EDGE
#define RL_TEXTURE_WRAP_MIRROR_REPEAT           0x8370      // GL_MIRRORED_REPEAT
#define RL_TEXTURE_WRAP_MIRROR_CLAMP            0x8742      // GL_MIRROR_CLAMP_EXT

// Matrix modes (equivalent to OpenGL)
#define RL_MODELVIEW                            0x1700      // GL_MODELVIEW
#define RL_PROJECTION                           0x1701      // GL_PROJECTION
#define RL_TEXTURE                              0x1702      // GL_TEXTURE

// Primitive assembly draw modes
#define RL_LINES                                0x0001      // GL_LINES
#define RL_TRIANGLES                            0x0004      // GL_TRIANGLES
#define RL_QUADS                                0x0007      // GL_QUADS

// GL equivalent data types
#define RL_UNSIGNED_BYTE                        0x1401      // GL_UNSIGNED_BYTE
#define RL_FLOAT                                0x1406      // GL_FLOAT

// GL buffer usage hint
#define RL_STREAM_DRAW                          0x88E0      // GL_STREAM_DRAW
#define RL_STREAM_READ                          0x88E1      // GL_STREAM_READ
#define RL_STREAM_COPY                          0x88E2      // GL_STREAM_COPY
#define RL_STATIC_DRAW                          0x88E4      // GL_STATIC_DRAW
#define RL_STATIC_READ                          0x88E5      // GL_STATIC_READ
#define RL_STATIC_COPY                          0x88E6      // GL_STATIC_COPY
#define RL_DYNAMIC_DRAW                         0x88E8      // GL_DYNAMIC_DRAW
#define RL_DYNAMIC_READ                         0x88E9      // GL_DYNAMIC_READ
#define RL_DYNAMIC_COPY                         0x88EA      // GL_DYNAMIC_COPY

// shaderc Shader type
#define RL_FRAGMENT_SHADER                      1
#define RL_VERTEX_SHADER                        0
#define RL_COMPUTE_SHADER                       2

// GL blending factors
#define RL_ZERO                                 0           // GL_ZERO
#define RL_ONE                                  1           // GL_ONE
#define RL_SRC_COLOR                            0x0300      // GL_SRC_COLOR
#define RL_ONE_MINUS_SRC_COLOR                  0x0301      // GL_ONE_MINUS_SRC_COLOR
#define RL_SRC_ALPHA                            0x0302      // GL_SRC_ALPHA
#define RL_ONE_MINUS_SRC_ALPHA                  0x0303      // GL_ONE_MINUS_SRC_ALPHA
#define RL_DST_ALPHA                            0x0304      // GL_DST_ALPHA
#define RL_ONE_MINUS_DST_ALPHA                  0x0305      // GL_ONE_MINUS_DST_ALPHA
#define RL_DST_COLOR                            0x0306      // GL_DST_COLOR
#define RL_ONE_MINUS_DST_COLOR                  0x0307      // GL_ONE_MINUS_DST_COLOR
#define RL_SRC_ALPHA_SATURATE                   0x0308      // GL_SRC_ALPHA_SATURATE
#define RL_CONSTANT_COLOR                       0x8001      // GL_CONSTANT_COLOR
#define RL_ONE_MINUS_CONSTANT_COLOR             0x8002      // GL_ONE_MINUS_CONSTANT_COLOR
#define RL_CONSTANT_ALPHA                       0x8003      // GL_CONSTANT_ALPHA
#define RL_ONE_MINUS_CONSTANT_ALPHA             0x8004      // GL_ONE_MINUS_CONSTANT_ALPHA

// GL blending functions/equations
#define RL_FUNC_ADD                             0x8006      // GL_FUNC_ADD
#define RL_MIN                                  0x8007      // GL_MIN
#define RL_MAX                                  0x8008      // GL_MAX
#define RL_FUNC_SUBTRACT                        0x800A      // GL_FUNC_SUBTRACT
#define RL_FUNC_REVERSE_SUBTRACT                0x800B      // GL_FUNC_REVERSE_SUBTRACT
#define RL_BLEND_EQUATION                       0x8009      // GL_BLEND_EQUATION
#define RL_BLEND_EQUATION_RGB                   0x8009      // GL_BLEND_EQUATION_RGB   // (Same as BLEND_EQUATION)
#define RL_BLEND_EQUATION_ALPHA                 0x883D      // GL_BLEND_EQUATION_ALPHA
#define RL_BLEND_DST_RGB                        0x80C8      // GL_BLEND_DST_RGB
#define RL_BLEND_SRC_RGB                        0x80C9      // GL_BLEND_SRC_RGB
#define RL_BLEND_DST_ALPHA                      0x80CA      // GL_BLEND_DST_ALPHA
#define RL_BLEND_SRC_ALPHA                      0x80CB      // GL_BLEND_SRC_ALPHA
#define RL_BLEND_COLOR                          0x8005      // GL_BLEND_COLOR

#define RL_READ_FRAMEBUFFER                     0x8CA8      // GL_READ_FRAMEBUFFER
#define RL_DRAW_FRAMEBUFFER                     0x8CA9      // GL_DRAW_FRAMEBUFFER

// Default shader vertex attribute locations
// NOTE: Locations can be redefined by user if required
#ifndef RL_DEFAULT_SHADER_ATTRIB_LOCATION_POSITION
    #define RL_DEFAULT_SHADER_ATTRIB_LOCATION_POSITION    0
#endif
#ifndef RL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD
    #define RL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD    1
#endif
#ifndef RL_DEFAULT_SHADER_ATTRIB_LOCATION_NORMAL
    #define RL_DEFAULT_SHADER_ATTRIB_LOCATION_NORMAL      2
#endif
#ifndef RL_DEFAULT_SHADER_ATTRIB_LOCATION_COLOR
    #define RL_DEFAULT_SHADER_ATTRIB_LOCATION_COLOR       3
#endif
    #ifndef RL_DEFAULT_SHADER_ATTRIB_LOCATION_TANGENT
#define RL_DEFAULT_SHADER_ATTRIB_LOCATION_TANGENT         4
#endif
#ifndef RL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD2
    #define RL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD2   5
#endif
#ifndef RL_DEFAULT_SHADER_ATTRIB_LOCATION_INDICES
    #define RL_DEFAULT_SHADER_ATTRIB_LOCATION_INDICES     6
#endif
#ifndef RL_DEFAULT_SHADER_ATTRIB_LOCATION_BONEINDICES
    #define RL_DEFAULT_SHADER_ATTRIB_LOCATION_BONEINDICES 7
#endif
#ifndef RL_DEFAULT_SHADER_ATTRIB_LOCATION_BONEWEIGHTS
    #define RL_DEFAULT_SHADER_ATTRIB_LOCATION_BONEWEIGHTS 8
#endif
#ifndef RL_DEFAULT_SHADER_ATTRIB_LOCATION_INSTANCETRANSFORM
    #define RL_DEFAULT_SHADER_ATTRIB_LOCATION_INSTANCETRANSFORM 9
#endif

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
#if !defined(__cplusplus)
#if (defined(__STDC__) && __STDC_VERSION__ >= 199901L) || (defined(_MSC_VER) && _MSC_VER >= 1800)
    #include <stdbool.h>
#elif !defined(bool)
    typedef enum bool { false = 0, true = !false } bool;
    #define RL_BOOL_TYPE
#endif
#endif
#include <stdint.h>

#if !defined(RL_MATRIX_TYPE)
// Matrix, 4x4 components, column major, OpenGL style, right handed
typedef struct Matrix {
    float m0, m4, m8, m12;      // Matrix first row (4 components)
    float m1, m5, m9, m13;      // Matrix second row (4 components)
    float m2, m6, m10, m14;     // Matrix third row (4 components)
    float m3, m7, m11, m15;     // Matrix fourth row (4 components)
} Matrix;
#define RL_MATRIX_TYPE
#endif

#include "rlvk_handles.h"

typedef struct rlTexture {
    rlvkSampler sampler;
    rlvkImage image;
    rlvkImageView view;
    rlvkAllocation allocation;
    rlvkDescriptorSet descriptorSet;
} rlTexture;

// Dynamic vertex buffers (position + texcoords + colors + indices arrays)
typedef struct rlVertexBuffer {
    int elementCount;           // Number of elements in the buffer (QUADS)
    int vertexOffset;           // Number of vertex offset to start writing

    float *vertices;            // Vertex position (XYZ - 3 components per vertex) (shader-location = 0)
    float *texcoords;           // Vertex texture coordinates (UV - 2 components per vertex) (shader-location = 1)
    float *normals;             // Vertex normal (XYZ - 3 components per vertex) (shader-location = 2)
    uint8_t *colors;      // Vertex colors (RGBA - 4 components per vertex) (shader-location = 3)

    rlvkBuffer stagingBuffers[4];
    rlvkAllocation stagingAllocations[4];
    rlvkBuffer deviceBuffers[4];
    rlvkAllocation deviceAllocations[4];
} rlVertexBuffer;

// Draw call type
// NOTE: Only texture changes register a new draw, other state-change-related elements are not
// used at this moment (vaoId, shaderId, matrices), raylib forces a batch draw call if any
// of those state-change happens (this is done in core module)
typedef struct rlDrawCall {
    int mode;                   // Drawing mode: LINES, TRIANGLES, QUADS
    int vertexCount;            // Number of vertex of the draw
    int vertexAlignment;        // Number of vertex required for index alignment (LINES, TRIANGLES)
    //unsigned int vaoId;       // Vertex array id to be used on the draw -> Using RLVK.currentBatch->vertexBuffer.vaoId
    //unsigned int shaderId;    // Shader id to be used on the draw -> Using RLVK.currentShaderId
    rlvkDescriptorSet textureDescriptorSet;     // Texture descriptor set to be used on the draw -> Use to create new draw call if changes

    //Matrix projection;        // Projection matrix for this draw -> Using RLVK.projection by default
    //Matrix modelview;         // Modelview matrix for this draw -> Using RLVK.modelview by default
} rlDrawCall;

// rlRenderBatch type
typedef struct rlRenderBatch {
    int bufferCount;            // Number of vertex buffers (multi-buffering support)
    int currentBuffer;          // Current buffer tracking in case of multi-buffering
    rlVertexBuffer *vertexBuffer; // Dynamic buffer(s) for vertex data
    rlvkBuffer quadIndexBuffer;
    rlvkAllocation quadIndexAllocation;

    rlDrawCall *draws;          // Draw calls array, depends on textureId
    int drawCounter;            // Draw calls counter
    float currentDepth;         // Current depth value for next draw
} rlRenderBatch;

typedef struct rlShaderProgram {
    rlvkPipeline pipeline;
    rlvkPipelineLayout pipelineLayout;
    rlvkDescriptorSet descriptorSet;
    uint32_t descriptorSize;
    uint32_t uniformCount;

    void *uniformBufferData; // Data to upload to the uniform buffer when drawing
    void *mappedUniformBuffer;
    rlvkDescriptorPool descriptorPool;
    rlvkDescriptorSetLayout descriptorSetLayout;
    char **structNames;
    uint32_t *structOffsets;
    rlvkBuffer uniformBuffer;
    rlvkAllocation uniformAllocation;
} rlShaderProgram;

// OpenGL version
typedef enum {
    RL_OPENGL_SOFTWARE = 0,  // Software rendering
    RL_OPENGL_11,               // OpenGL 1.1
    RL_OPENGL_21,               // OpenGL 2.1 (GLSL 120)
    RL_OPENGL_33,               // OpenGL 3.3 (GLSL 330)
    RL_OPENGL_43,               // OpenGL 4.3 (using GLSL 330)
    RL_OPENGL_ES_20,            // OpenGL ES 2.0 (GLSL 100)
    RL_OPENGL_ES_30,             // OpenGL ES 3.0 (GLSL 300 es)
    RL_VULKAN
} rlGlVersion;

// Trace log level
// NOTE: Organized by priority level
typedef enum {
    RL_LOG_ALL = 0,             // Display all logs
    RL_LOG_TRACE,               // Trace logging, intended for internal use only
    RL_LOG_DEBUG,               // Debug logging, used for internal debugging, it should be disabled on release builds
    RL_LOG_INFO,                // Info logging, used for program execution info
    RL_LOG_WARNING,             // Warning logging, used on recoverable failures
    RL_LOG_ERROR,               // Error logging, used on unrecoverable failures
    RL_LOG_FATAL,               // Fatal logging, used to abort program: exit(EXIT_FAILURE)
    RL_LOG_NONE                 // Disable logging
} rlTraceLogLevel;

// Texture pixel formats
// NOTE: Support depends on OpenGL version
typedef enum {
    RL_PIXELFORMAT_UNCOMPRESSED_GRAYSCALE = 1,     // 8 bit per pixel (no alpha)
    RL_PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA,        // 8*2 bpp (2 channels)
    RL_PIXELFORMAT_UNCOMPRESSED_R5G6B5,            // 16 bpp
    RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8,            // 24 bpp
    RL_PIXELFORMAT_UNCOMPRESSED_R5G5B5A1,          // 16 bpp (1 bit alpha)
    RL_PIXELFORMAT_UNCOMPRESSED_R4G4B4A4,          // 16 bpp (4 bit alpha)
    RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,          // 32 bpp
    RL_PIXELFORMAT_UNCOMPRESSED_R32,               // 32 bpp (1 channel - float)
    RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32,         // 32*3 bpp (3 channels - float)
    RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32A32,      // 32*4 bpp (4 channels - float)
    RL_PIXELFORMAT_UNCOMPRESSED_R16,               // 16 bpp (1 channel - half float)
    RL_PIXELFORMAT_UNCOMPRESSED_R16G16B16,         // 16*3 bpp (3 channels - half float)
    RL_PIXELFORMAT_UNCOMPRESSED_R16G16B16A16,      // 16*4 bpp (4 channels - half float)
    RL_PIXELFORMAT_COMPRESSED_DXT1_RGB,            // 4 bpp (no alpha)
    RL_PIXELFORMAT_COMPRESSED_DXT1_RGBA,           // 4 bpp (1 bit alpha)
    RL_PIXELFORMAT_COMPRESSED_DXT3_RGBA,           // 8 bpp
    RL_PIXELFORMAT_COMPRESSED_DXT5_RGBA,           // 8 bpp
    RL_PIXELFORMAT_COMPRESSED_ETC1_RGB,            // 4 bpp
    RL_PIXELFORMAT_COMPRESSED_ETC2_RGB,            // 4 bpp
    RL_PIXELFORMAT_COMPRESSED_ETC2_EAC_RGBA,       // 8 bpp
    RL_PIXELFORMAT_COMPRESSED_PVRT_RGB,            // 4 bpp
    RL_PIXELFORMAT_COMPRESSED_PVRT_RGBA,           // 4 bpp
    RL_PIXELFORMAT_COMPRESSED_ASTC_4x4_RGBA,       // 8 bpp
    RL_PIXELFORMAT_COMPRESSED_ASTC_8x8_RGBA        // 2 bpp
} rlPixelFormat;

// Texture parameters: filter mode
// NOTE 1: Filtering considers mipmaps if available in the texture
// NOTE 2: Filter is accordingly set for minification and magnification
typedef enum {
    RL_TEXTURE_FILTER_POINT = 0,        // No filter, pixel approximation
    RL_TEXTURE_FILTER_BILINEAR,         // Linear filtering
    RL_TEXTURE_FILTER_TRILINEAR,        // Trilinear filtering (linear with mipmaps)
    RL_TEXTURE_FILTER_ANISOTROPIC_4X,   // Anisotropic filtering 4x
    RL_TEXTURE_FILTER_ANISOTROPIC_8X,   // Anisotropic filtering 8x
    RL_TEXTURE_FILTER_ANISOTROPIC_16X,  // Anisotropic filtering 16x
} rlTextureFilter;

// Color blending modes (pre-defined)
typedef enum {
    RL_BLEND_ALPHA = 0,                 // Blend textures considering alpha (default)
    RL_BLEND_ADDITIVE,                  // Blend textures adding colors
    RL_BLEND_MULTIPLIED,                // Blend textures multiplying colors
    RL_BLEND_ADD_COLORS,                // Blend textures adding colors (alternative)
    RL_BLEND_SUBTRACT_COLORS,           // Blend textures subtracting colors (alternative)
    RL_BLEND_ALPHA_PREMULTIPLY,         // Blend premultiplied textures considering alpha
    RL_BLEND_CUSTOM,                    // Blend textures using custom src/dst factors (use rlSetBlendFactors())
    RL_BLEND_CUSTOM_SEPARATE            // Blend textures using custom src/dst factors (use rlSetBlendFactorsSeparate())
} rlBlendMode;

// Shader location point type
typedef enum {
    RL_SHADER_LOC_VERTEX_POSITION = 0,  // Shader location: vertex attribute: position
    RL_SHADER_LOC_VERTEX_TEXCOORD01,    // Shader location: vertex attribute: texcoord01
    RL_SHADER_LOC_VERTEX_TEXCOORD02,    // Shader location: vertex attribute: texcoord02
    RL_SHADER_LOC_VERTEX_NORMAL,        // Shader location: vertex attribute: normal
    RL_SHADER_LOC_VERTEX_TANGENT,       // Shader location: vertex attribute: tangent
    RL_SHADER_LOC_VERTEX_COLOR,         // Shader location: vertex attribute: color
} rlShaderAttributeLocationIndex;

// Shader location point type
typedef enum {
    RL_SHADER_LOC_MATRIX_MVP = 0,           // Shader location: matrix uniform: model-view-projection
    RL_SHADER_LOC_MATRIX_VIEW,          // Shader location: matrix uniform: view (camera transform)
    RL_SHADER_LOC_MATRIX_PROJECTION,    // Shader location: matrix uniform: projection
    RL_SHADER_LOC_MATRIX_MODEL,         // Shader location: matrix uniform: model (transform)
    RL_SHADER_LOC_MATRIX_NORMAL,        // Shader location: matrix uniform: normal
    RL_SHADER_LOC_VECTOR_VIEW,          // Shader location: vector uniform: view
    RL_SHADER_LOC_COLOR_DIFFUSE,        // Shader location: vector uniform: diffuse color
    RL_SHADER_LOC_COLOR_SPECULAR,       // Shader location: vector uniform: specular color
    RL_SHADER_LOC_COLOR_AMBIENT,        // Shader location: vector uniform: ambient color
    RL_SHADER_LOC_MAP_ALBEDO,           // Shader location: sampler2d texture: albedo (same as: RL_SHADER_LOC_MAP_DIFFUSE)
    RL_SHADER_LOC_MAP_METALNESS,        // Shader location: sampler2d texture: metalness (same as: RL_SHADER_LOC_MAP_SPECULAR)
    RL_SHADER_LOC_MAP_NORMAL,           // Shader location: sampler2d texture: normal
    RL_SHADER_LOC_MAP_ROUGHNESS,        // Shader location: sampler2d texture: roughness
    RL_SHADER_LOC_MAP_OCCLUSION,        // Shader location: sampler2d texture: occlusion
    RL_SHADER_LOC_MAP_EMISSION,         // Shader location: sampler2d texture: emission
    RL_SHADER_LOC_MAP_HEIGHT,           // Shader location: sampler2d texture: height
    RL_SHADER_LOC_MAP_CUBEMAP,          // Shader location: samplerCube texture: cubemap
    RL_SHADER_LOC_MAP_IRRADIANCE,       // Shader location: samplerCube texture: irradiance
    RL_SHADER_LOC_MAP_PREFILTER,        // Shader location: samplerCube texture: prefilter
    RL_SHADER_LOC_MAP_BRDF              // Shader location: sampler2d texture: brdf
} rlShaderUniformLocationIndex;

#define RL_SHADER_LOC_MAP_DIFFUSE       RL_SHADER_LOC_MAP_ALBEDO
#define RL_SHADER_LOC_MAP_SPECULAR      RL_SHADER_LOC_MAP_METALNESS

// Shader uniform data type
typedef enum {
    RL_SHADER_UNIFORM_FLOAT = 0,        // Shader uniform type: float
    RL_SHADER_UNIFORM_VEC2,             // Shader uniform type: vec2 (2 float)
    RL_SHADER_UNIFORM_VEC3,             // Shader uniform type: vec3 (3 float)
    RL_SHADER_UNIFORM_VEC4,             // Shader uniform type: vec4 (4 float)
    RL_SHADER_UNIFORM_INT,              // Shader uniform type: int
    RL_SHADER_UNIFORM_IVEC2,            // Shader uniform type: ivec2 (2 int)
    RL_SHADER_UNIFORM_IVEC3,            // Shader uniform type: ivec3 (3 int)
    RL_SHADER_UNIFORM_IVEC4,            // Shader uniform type: ivec4 (4 int)
    RL_SHADER_UNIFORM_UINT,             // Shader uniform type: unsigned int
    RL_SHADER_UNIFORM_UIVEC2,           // Shader uniform type: uivec2 (2 unsigned int)
    RL_SHADER_UNIFORM_UIVEC3,           // Shader uniform type: uivec3 (3 unsigned int)
    RL_SHADER_UNIFORM_UIVEC4,           // Shader uniform type: uivec4 (4 unsigned int)
    RL_SHADER_UNIFORM_SAMPLER2D         // Shader uniform type: sampler2d
} rlShaderUniformDataType;

// Shader attribute data types
typedef enum {
    RL_SHADER_ATTRIB_FLOAT = 0,         // Shader attribute type: float
    RL_SHADER_ATTRIB_VEC2,              // Shader attribute type: vec2 (2 float)
    RL_SHADER_ATTRIB_VEC3,              // Shader attribute type: vec3 (3 float)
    RL_SHADER_ATTRIB_VEC4               // Shader attribute type: vec4 (4 float)
} rlShaderAttributeDataType;

// Framebuffer attachment type
// NOTE: By default up to 8 color channels defined, but it can be more
typedef enum {
    RL_ATTACHMENT_COLOR_CHANNEL0 = 0,       // Framebuffer attachment type: color 0
    RL_ATTACHMENT_COLOR_CHANNEL1 = 1,       // Framebuffer attachment type: color 1
    RL_ATTACHMENT_COLOR_CHANNEL2 = 2,       // Framebuffer attachment type: color 2
    RL_ATTACHMENT_COLOR_CHANNEL3 = 3,       // Framebuffer attachment type: color 3
    RL_ATTACHMENT_COLOR_CHANNEL4 = 4,       // Framebuffer attachment type: color 4
    RL_ATTACHMENT_COLOR_CHANNEL5 = 5,       // Framebuffer attachment type: color 5
    RL_ATTACHMENT_COLOR_CHANNEL6 = 6,       // Framebuffer attachment type: color 6
    RL_ATTACHMENT_COLOR_CHANNEL7 = 7,       // Framebuffer attachment type: color 7
    RL_ATTACHMENT_DEPTH = 100,              // Framebuffer attachment type: depth
    RL_ATTACHMENT_STENCIL = 200,            // Framebuffer attachment type: stencil
} rlFramebufferAttachType;

// Framebuffer texture attachment type
typedef enum {
    RL_ATTACHMENT_CUBEMAP_POSITIVE_X = 0,   // Framebuffer texture attachment type: cubemap, +X side
    RL_ATTACHMENT_CUBEMAP_NEGATIVE_X = 1,   // Framebuffer texture attachment type: cubemap, -X side
    RL_ATTACHMENT_CUBEMAP_POSITIVE_Y = 2,   // Framebuffer texture attachment type: cubemap, +Y side
    RL_ATTACHMENT_CUBEMAP_NEGATIVE_Y = 3,   // Framebuffer texture attachment type: cubemap, -Y side
    RL_ATTACHMENT_CUBEMAP_POSITIVE_Z = 4,   // Framebuffer texture attachment type: cubemap, +Z side
    RL_ATTACHMENT_CUBEMAP_NEGATIVE_Z = 5,   // Framebuffer texture attachment type: cubemap, -Z side
    RL_ATTACHMENT_TEXTURE2D = 100,          // Framebuffer texture attachment type: texture2d
    RL_ATTACHMENT_RENDERBUFFER = 200,       // Framebuffer texture attachment type: renderbuffer
} rlFramebufferAttachTextureType;

// Face culling mode
typedef enum {
    RL_CULL_FACE_FRONT = 0,
    RL_CULL_FACE_BACK
} rlCullMode;

typedef struct rlShaderUniform {
    const char *name;
    uint32_t size;
} rlShaderUniform;

//------------------------------------------------------------------------------------
// Functions Declaration - Matrix operations
//------------------------------------------------------------------------------------

#if defined(__cplusplus)
extern "C" {            // Prevents name mangling of functions
#endif

RLAPI void rlMatrixMode(int mode);                      // Choose the current matrix to be transformed
RLAPI void rlPushMatrix(void);                          // Push the current matrix to stack
RLAPI void rlPopMatrix(void);                           // Pop latest inserted matrix from stack
RLAPI void rlLoadIdentity(void);                        // Reset current matrix to identity matrix
RLAPI void rlTranslatef(float x, float y, float z);     // Multiply the current matrix by a translation matrix
RLAPI void rlRotatef(float angle, float x, float y, float z); // Multiply the current matrix by a rotation matrix
RLAPI void rlScalef(float x, float y, float z);         // Multiply the current matrix by a scaling matrix
RLAPI void rlMultMatrixf(const float *matf);            // Multiply the current matrix by another matrix
RLAPI void rlFrustum(double left, double right, double bottom, double top, double znear, double zfar);
RLAPI void rlOrtho(double left, double right, double bottom, double top, double znear, double zfar);
RLAPI void rlViewport(int x, int y, int width, int height); // Set the viewport area
RLAPI void rlSetClipPlanes(double nearPlane, double farPlane);    // Set clip planes distances
RLAPI double rlGetCullDistanceNear(void);               // Get cull plane distance near
RLAPI double rlGetCullDistanceFar(void);                // Get cull plane distance far

//------------------------------------------------------------------------------------
// Functions Declaration - Vertex level operations
//------------------------------------------------------------------------------------
RLAPI void rlBegin(int mode);                           // Initialize drawing mode (how to organize vertex)
RLAPI void rlEnd(void);                                 // Finish vertex providing
RLAPI void rlVertex2i(int x, int y);                    // Define one vertex (position) - 2 int
RLAPI void rlVertex2f(float x, float y);                // Define one vertex (position) - 2 float
RLAPI void rlVertex3f(float x, float y, float z);       // Define one vertex (position) - 3 float
RLAPI void rlTexCoord2f(float x, float y);              // Define one vertex (texture coordinate) - 2 float
RLAPI void rlNormal3f(float x, float y, float z);       // Define one vertex (normal) - 3 float
RLAPI void rlColor4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a); // Define one vertex (color) - 4 byte
RLAPI void rlColor3f(float x, float y, float z);        // Define one vertex (color) - 3 float
RLAPI void rlColor4f(float x, float y, float z, float w); // Define one vertex (color) - 4 float

//------------------------------------------------------------------------------------
// Functions Declaration - OpenGL style functions (common to 1.1, 3.3+, ES2)
// NOTE: This functions are used to completely abstract raylib code from OpenGL layer,
// some of them are direct wrappers over OpenGL calls, some others are custom
//------------------------------------------------------------------------------------

// Vertex buffers state
RLAPI bool rlEnableVertexArray(unsigned int vaoId);     // Enable vertex array (VAO, if supported)
RLAPI void rlDisableVertexArray(void);                  // Disable vertex array (VAO, if supported)
RLAPI void rlEnableVertexBuffer(unsigned int id);       // Enable vertex buffer (VBO)
RLAPI void rlDisableVertexBuffer(void);                 // Disable vertex buffer (VBO)
RLAPI void rlEnableVertexBufferElement(unsigned int id); // Enable vertex buffer element (VBO element)
RLAPI void rlDisableVertexBufferElement(void);          // Disable vertex buffer element (VBO element)
RLAPI void rlEnableVertexAttribute(unsigned int index); // Enable vertex attribute index
RLAPI void rlDisableVertexAttribute(unsigned int index); // Disable vertex attribute index
RLAPI void rlEnableStatePointer(int vertexAttribType, void *buffer); // Enable attribute state pointer
RLAPI void rlDisableStatePointer(int vertexAttribType); // Disable attribute state pointer

// Textures state
RLAPI void rlActiveTextureSlot(int slot);               // Select and active a texture slot
RLAPI void rlEnableTexture(const rlTexture* texture);            // Enable texture
RLAPI void rlDisableTexture(void);                      // Disable texture
RLAPI void rlEnableTextureCubemap(const rlTexture* texture);     // Enable texture cubemap
RLAPI void rlDisableTextureCubemap(void);               // Disable texture cubemap
RLAPI void rlTextureParameters(const rlTexture* texture, int param, int value); // Set texture parameters (filter, wrap)
RLAPI void rlCubemapParameters(const rlTexture* texture, int param, int value); // Set cubemap parameters (filter, wrap)

// Shader state
RLAPI void rlEnableShader(const rlShaderProgram *program);             // Enable shader program
RLAPI void rlDisableShader(void);                       // Disable shader program

// Framebuffer state
RLAPI void rlEnableFramebuffer(unsigned int id);        // Enable render texture (fbo)
RLAPI void rlDisableFramebuffer(void);                  // Disable render texture (fbo), return to default framebuffer
RLAPI unsigned int rlGetActiveFramebuffer(void);        // Get the currently active render texture (fbo), 0 for default framebuffer
RLAPI void rlActiveDrawBuffers(int count);              // Activate multiple draw color buffers
RLAPI void rlBlitFramebuffer(int srcX, int srcY, int srcWidth, int srcHeight, int dstX, int dstY, int dstWidth, int dstHeight, int bufferMask); // Blit active framebuffer to main framebuffer
RLAPI void rlBindFramebuffer(unsigned int target, unsigned int framebuffer); // Bind framebuffer (FBO)

// General render state
RLAPI void rlEnableColorBlend(void);                    // Enable color blending
RLAPI void rlDisableColorBlend(void);                   // Disable color blending
RLAPI void rlEnableDepthTest(void);                     // Enable depth test
RLAPI void rlDisableDepthTest(void);                    // Disable depth test
RLAPI void rlEnableDepthMask(void);                     // Enable depth write
RLAPI void rlDisableDepthMask(void);                    // Disable depth write
RLAPI void rlEnableBackfaceCulling(void);               // Enable backface culling
RLAPI void rlDisableBackfaceCulling(void);              // Disable backface culling
RLAPI void rlColorMask(bool r, bool g, bool b, bool a); // Color mask control
RLAPI void rlSetCullFace(int mode);                     // Set face culling mode
RLAPI void rlEnableScissorTest(void);                   // Enable scissor test
RLAPI void rlDisableScissorTest(void);                  // Disable scissor test
RLAPI void rlScissor(int x, int y, int width, int height); // Scissor test
RLAPI void rlEnablePointMode(void);                     // Enable point mode
RLAPI void rlDisablePointMode(void);                    // Disable point mode
RLAPI void rlSetPointSize(float size);                  // Set the point drawing size
RLAPI float rlGetPointSize(void);                       // Get the point drawing size
RLAPI void rlEnableWireMode(void);                      // Enable wire mode
RLAPI void rlDisableWireMode(void);                     // Disable wire mode
RLAPI void rlSetLineWidth(float width);                 // Set the line drawing width
RLAPI float rlGetLineWidth(void);                       // Get the line drawing width
RLAPI void rlEnableSmoothLines(void);                   // Enable line aliasing
RLAPI void rlDisableSmoothLines(void);                  // Disable line aliasing
RLAPI void rlEnableStereoRender(void);                  // Enable stereo rendering
RLAPI void rlDisableStereoRender(void);                 // Disable stereo rendering
RLAPI bool rlIsStereoRenderEnabled(void);               // Check if stereo render is enabled

RLAPI void rlClearColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a); // Clear color buffer with color
RLAPI void rlClearScreenBuffers(void);                  // Clear used screen buffers (color and depth)
RLAPI void rlCheckErrors(void);                         // Check and log OpenGL error codes
RLAPI void rlSetBlendMode(int mode);                    // Set blending mode
RLAPI void rlSetBlendFactors(int glSrcFactor, int glDstFactor, int glEquation); // Set blending mode factor and equation (using OpenGL factors)
RLAPI void rlSetBlendFactorsSeparate(int glSrcRGB, int glDstRGB, int glSrcAlpha, int glDstAlpha, int glEqRGB, int glEqAlpha); // Set blending mode factors and equations separately (using OpenGL factors)

typedef struct GLFWwindow GLFWwindow;

//------------------------------------------------------------------------------------
// Functions Declaration - rlvk functionality
//------------------------------------------------------------------------------------
// rlvk initialization functions
RLAPI void rlvkInit(int width, int height, GLFWwindow *windowHandle);             // Initialize rlvk (instance, device, surface, swapchain, etc.)
RLAPI void rlvkClose(void);                             // De-initialize rlvk (instance, device, surface, swapchain, etc.)
RLAPI void rlLoadExtensions(void *loader);              // Load OpenGL extensions (loader function required)
RLAPI void *rlGetProcAddress(const char *procName);     // Get OpenGL procedure address
RLAPI int rlGetVersion(void);                           // Get current OpenGL version
RLAPI void rlSetFramebufferWidth(int width);            // Set current framebuffer width
RLAPI int rlGetFramebufferWidth(void);                  // Get default framebuffer width
RLAPI void rlSetFramebufferHeight(int height);          // Set current framebuffer height
RLAPI int rlGetFramebufferHeight(void);                 // Get default framebuffer height

RLAPI rlTexture *rlGetTextureIdDefault(void);         // Get default texture id
RLAPI rlShaderProgram *rlGetShaderIdDefault(void);          // Get default shader id
RLAPI int *rlGetShaderLocsDefault(void);                // Get default shader locations

// Render batch management
RLAPI rlRenderBatch rlLoadRenderBatch(int numBuffers, int bufferElements); // Load a render batch system
RLAPI void rlUnloadRenderBatch(rlRenderBatch batch);    // Unload render batch system
RLAPI void rlDrawRenderBatch(rlRenderBatch *batch);     // Draw render batch data (Update->Draw->Reset)
RLAPI void rlSetRenderBatchActive(rlRenderBatch *batch); // Set the active render batch for rlvk (NULL for default internal)
RLAPI void rlDrawRenderBatchActive(void);               // Update and draw internal render batch
RLAPI bool rlCheckRenderBatchLimit(int vCount);         // Check internal buffer overflow for a given number of vertex

RLAPI void rlSetTexture(const rlTexture *texture);               // Set current texture for render batch and check buffers limits

//------------------------------------------------------------------------------------------------------------------------

// Vertex buffers management
RLAPI unsigned int rlLoadVertexArray(void);             // Load vertex array (vao) if supported
RLAPI unsigned int rlLoadVertexBuffer(const void *buffer, int size, bool dynamic); // Load a vertex buffer object
RLAPI unsigned int rlLoadVertexBufferElement(const void *buffer, int size, bool dynamic); // Load vertex buffer elements object
RLAPI void rlUpdateVertexBuffer(unsigned int bufferId, const void *data, int dataSize, int offset); // Update vertex buffer object data on GPU buffer
RLAPI void rlUpdateVertexBufferElements(unsigned int id, const void *data, int dataSize, int offset); // Update vertex buffer elements data on GPU buffer
RLAPI void rlUnloadVertexArray(unsigned int vaoId);     // Unload vertex array (vao)
RLAPI void rlUnloadVertexBuffer(unsigned int vboId);    // Unload vertex buffer object
RLAPI void rlSetVertexAttribute(unsigned int index, int compSize, int type, bool normalized, int stride, int offset); // Set vertex attribute data configuration
RLAPI void rlSetVertexAttributeDivisor(unsigned int index, int divisor); // Set vertex attribute data divisor
RLAPI void rlSetVertexAttributeDefault(int locIndex, const void *value, int attribType, int count); // Set vertex attribute default value, when attribute to provided
RLAPI void rlDrawVertexArray(int offset, int count);    // Draw vertex array (currently active vao)
RLAPI void rlDrawVertexArrayElements(int offset, int count, const void *buffer); // Draw vertex array elements
RLAPI void rlDrawVertexArrayInstanced(int offset, int count, int instances); // Draw vertex array (currently active vao) with instancing
RLAPI void rlDrawVertexArrayElementsInstanced(int offset, int count, const void *buffer, int instances); // Draw vertex array elements with instancing

// Textures management
RLAPI rlTexture rlLoadTexture(const void *data, int width, int height, int format, int mipmapCount); // Load texture data
RLAPI rlTexture rlLoadTextureDepth(int width, int height, bool useRenderBuffer); // Load depth texture/renderbuffer (to be attached to fbo)
RLAPI rlTexture rlLoadTextureCubemap(const void *data, int size, int format, int mipmapCount); // Load texture cubemap data
RLAPI void rlUpdateTexture(const rlTexture *texture, int offsetX, int offsetY, int width, int height, int format, const void *data); // Update texture with new data on GPU
RLAPI void rlGetGlTextureFormats(int format, unsigned int *glInternalFormat, unsigned int *glFormat, unsigned int *glType); // Get OpenGL internal formats
RLAPI const char *rlGetPixelFormatName(unsigned int format);              // Get name string for pixel format
RLAPI void rlUnloadTexture(const rlTexture *texture);                              // Unload texture from GPU memory
RLAPI void rlGenTextureMipmaps(const rlTexture *texture, int width, int height, int format, int *mipmaps); // Generate mipmap data for selected texture
RLAPI void *rlReadTexturePixels(const rlTexture *texture, int width, int height, int format); // Read texture pixel data
RLAPI unsigned char *rlReadScreenPixels(int width, int height);           // Read screen pixel data (color buffer)

// Framebuffer management (fbo)
RLAPI unsigned int rlLoadFramebuffer(void);                               // Load an empty framebuffer
RLAPI void rlFramebufferAttach(unsigned int id, const rlTexture *texture, int attachType, int texType, int mipLevel); // Attach texture/renderbuffer to a framebuffer
RLAPI bool rlFramebufferComplete(unsigned int id);                        // Verify framebuffer is complete
RLAPI void rlUnloadFramebuffer(unsigned int id);                          // Delete framebuffer from GPU
// WARNING: Copy and resize framebuffer functionality only defined for software backend
RLAPI void rlCopyFramebuffer(int x, int y, int width, int height, int format, void *pixels); // Copy framebuffer pixel data to internal buffer
RLAPI void rlResizeFramebuffer(int width, int height);                    // Resize internal framebuffer

// Shaders management
RLAPI rlvkShaderModule rlLoadShader(const char *code, int type);                    // Load (compile) shader and return shader id (type: RL_VERTEX_SHADER, RL_FRAGMENT_SHADER, RL_COMPUTE_SHADER)
RLAPI rlShaderProgram rlLoadShaderProgram(const char *vsCode, const char *fsCode); // Load shader from code strings
RLAPI rlShaderProgram rlLoadShaderProgramEx(rlvkShaderModule vsModule, rlvkShaderModule fsModule); // Load shader program, using already loaded shader modules
RLAPI rlShaderProgram rlLoadShaderProgramExA(const char *vsCode, const char *fsCode, uint32_t uniformCount, const rlShaderUniform *uniforms);               // Load shader with descriptor layout from code strings
RLAPI rlShaderProgram rlLoadShaderProgramPro(rlvkShaderModule vsModule, rlvkShaderModule fsModule, uint32_t uniformCount, const rlShaderUniform *uniforms); // Load shader program with descriptor layout, using already loaded shader modules
RLAPI unsigned int rlLoadShaderProgramCompute(unsigned int csId);               // Load compute shader program
RLAPI void rlUnloadShader(rlvkShaderModule shaderModule);                                     // Unload shader, loaded with rlLoadShader()
RLAPI void rlUnloadShaderProgram(const rlShaderProgram *program);                              // Unload shader program
RLAPI int rlGetLocationUniform(const rlShaderProgram *program, const char *uniformName);       // Get shader location uniform, requires shader program id
RLAPI int rlGetLocationAttrib(const rlShaderProgram *program, const char *attribName);         // Get shader location attribute, requires shader program id
RLAPI void rlSetUniform(int locIndex, const void *value, int uniformType, int count); // Set shader value uniform
RLAPI void rlSetUniformMatrix(int locIndex, Matrix mat);                        // Set shader value matrix
RLAPI void rlSetUniformMatrices(int locIndex, const Matrix *mat, int count);    // Set shader value matrices
RLAPI void rlSetUniformSampler(int locIndex, const rlTexture *texture);           // Set shader value sampler
RLAPI void rlSetShader(const rlShaderProgram *program);                             // Set shader currently active (id and locations)

// Compute shader management
RLAPI void rlComputeShaderDispatch(unsigned int groupX, unsigned int groupY, unsigned int groupZ); // Dispatch compute shader (equivalent to *draw* for graphics pipeline)

// Shader buffer storage object management (ssbo)
RLAPI unsigned int rlLoadShaderBuffer(unsigned int size, const void *data, int usageHint); // Load shader storage buffer object (SSBO)
RLAPI void rlUnloadShaderBuffer(unsigned int ssboId);                           // Unload shader storage buffer object (SSBO)
RLAPI void rlUpdateShaderBuffer(unsigned int id, const void *data, unsigned int dataSize, unsigned int offset); // Update SSBO buffer data
RLAPI void rlBindShaderBuffer(unsigned int id, unsigned int index);             // Bind SSBO buffer
RLAPI void rlReadShaderBuffer(unsigned int id, void *dest, unsigned int count, unsigned int offset); // Read SSBO buffer data (GPU->CPU)
RLAPI void rlCopyShaderBuffer(unsigned int destId, unsigned int srcId, unsigned int destOffset, unsigned int srcOffset, unsigned int count); // Copy SSBO data between buffers
RLAPI unsigned int rlGetShaderBufferSize(unsigned int id);                      // Get SSBO buffer size

// Buffer management
RLAPI void rlBindImageTexture(const rlTexture *texture, unsigned int index, int format, bool readonly);  // Bind image texture

// Matrix state management
RLAPI Matrix rlGetMatrixModelview(void);                                  // Get internal modelview matrix
RLAPI Matrix rlGetMatrixProjection(void);                                 // Get internal projection matrix
RLAPI Matrix rlGetMatrixTransform(void);                                  // Get internal accumulated transform matrix
RLAPI Matrix rlGetMatrixProjectionStereo(int eye);                        // Get internal projection matrix for stereo render (selected eye)
RLAPI Matrix rlGetMatrixViewOffsetStereo(int eye);                        // Get internal view offset matrix for stereo render (selected eye)
RLAPI void rlSetMatrixProjection(Matrix proj);                            // Set a custom projection matrix (replaces internal projection matrix)
RLAPI void rlSetMatrixModelview(Matrix view);                             // Set a custom modelview matrix (replaces internal modelview matrix)
RLAPI void rlSetMatrixProjectionStereo(Matrix right, Matrix left);        // Set eyes projection matrices for stereo rendering
RLAPI void rlSetMatrixViewOffsetStereo(Matrix right, Matrix left);        // Set eyes view offsets matrices for stereo rendering

// Quick and dirty cube/quad buffers load->draw->unload
RLAPI void rlLoadDrawCube(void);     // Load and draw a cube
RLAPI void rlLoadDrawQuad(void);     // Load and draw a quad

// Extra Vulkan specific functions
RLAPI void rlBeginFrame(void);
RLAPI void rlEndFrame(void);
RLAPI void rlWaitIdle(void);

#if defined(__cplusplus)
}
#endif

#endif // RLVK_H

/***********************************************************************************
*
*   RLVK IMPLEMENTATION
*
************************************************************************************/

#if defined(RLVK_IMPLEMENTATION)

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h> // Exactly what you think this is for
#include <shaderc/shaderc.h>
#include "external/vk_mem_alloc.h"

#include <stdlib.h>             // Required for: calloc(), free()
#include <string.h>             // Required for: strcmp(), strlen()
#include <math.h>               // Required for: sqrtf(), sinf(), cosf(), floor(), log()
#include <limits.h>             // Required for: INT_MAX

#define RLVK_ENABLE_VULKAN_VALIDATION_LAYER // TODO: Remove

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#ifndef PI
    #define PI 3.14159265358979323846f
#endif
#ifndef DEG2RAD
    #define DEG2RAD (PI/180.0f)
#endif
#ifndef RAD2DEG
    #define RAD2DEG (180.0f/PI)
#endif

// Default shader vertex attribute names to set location points
// WARNING: Pre-defined names can not be changed, they are used by default shaders and all raylib examples shaders, they are just listed here for reference
#define RL_DEFAULT_SHADER_ATTRIB_NAME_POSITION          "vertexPosition"    // Bound by default to shader location: RL_DEFAULT_SHADER_ATTRIB_LOCATION_POSITION
#define RL_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD          "vertexTexCoord"    // Bound by default to shader location: RL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD
#define RL_DEFAULT_SHADER_ATTRIB_NAME_NORMAL            "vertexNormal"      // Bound by default to shader location: RL_DEFAULT_SHADER_ATTRIB_LOCATION_NORMAL
#define RL_DEFAULT_SHADER_ATTRIB_NAME_COLOR             "vertexColor"       // Bound by default to shader location: RL_DEFAULT_SHADER_ATTRIB_LOCATION_COLOR
#define RL_DEFAULT_SHADER_ATTRIB_NAME_TANGENT           "vertexTangent"     // Bound by default to shader location: RL_DEFAULT_SHADER_ATTRIB_LOCATION_TANGENT
#define RL_DEFAULT_SHADER_ATTRIB_NAME_TEXCOORD2         "vertexTexCoord2"   // Bound by default to shader location: RL_DEFAULT_SHADER_ATTRIB_LOCATION_TEXCOORD2
#define RL_DEFAULT_SHADER_ATTRIB_NAME_BONEINDICES       "vertexBoneIndices" // Bound by default to shader location: RL_DEFAULT_SHADER_ATTRIB_LOCATION_BONEINDICES
#define RL_DEFAULT_SHADER_ATTRIB_NAME_BONEWEIGHTS       "vertexBoneWeights" // Bound by default to shader location: RL_DEFAULT_SHADER_ATTRIB_LOCATION_BONEWEIGHTS
#define RL_DEFAULT_SHADER_ATTRIB_NAME_INSTANCETRANSFORM "instanceTransform" // Bound by default to shader location: RL_DEFAULT_SHADER_ATTRIB_LOCATION_INSTANCETRANSFORM

#define RL_DEFAULT_SHADER_UNIFORM_NAME_MVP              "mvp"               // model-view-projection matrix
#define RL_DEFAULT_SHADER_UNIFORM_NAME_VIEW             "matView"           // view matrix
#define RL_DEFAULT_SHADER_UNIFORM_NAME_PROJECTION       "matProjection"     // projection matrix
#define RL_DEFAULT_SHADER_UNIFORM_NAME_MODEL            "matModel"          // model matrix
#define RL_DEFAULT_SHADER_UNIFORM_NAME_NORMAL           "matNormal"         // normal matrix (transpose(inverse(matModelView))
#define RL_DEFAULT_SHADER_UNIFORM_NAME_COLOR            "colDiffuse"        // color diffuse (base tint color, multiplied by texture color)
#define RL_DEFAULT_SHADER_UNIFORM_NAME_BONEMATRICES     "boneMatrices"      // bone matrices (required for GPU skinning)

#define RL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE0       "texture0"          // texture0 (texture slot active 0)
#define RL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE1       "texture1"          // texture1 (texture slot active 1)
#define RL_DEFAULT_SHADER_SAMPLER2D_NAME_TEXTURE2       "texture2"          // texture2 (texture slot active 2)

#define RL_UNIFORM_BUFFER_COUNT 1

//----------------------------------------------------------------------------------
// Module Types and Structures Definition
//----------------------------------------------------------------------------------
typedef struct rlvkData {
    bool inited;
    bool vSync;

    //VULKAN DATA
    
    VkInstance instance;
#ifdef RLVK_ENABLE_VULKAN_VALIDATION_LAYER
    VkDebugUtilsMessengerEXT debugMessenger;
#endif
    VkSurfaceKHR surface;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkSwapchainKHR swapChain;
    VkExtent2D swapChainExtent;
    uint32_t swapChainImageCount;
    VkImage* swapChainImages;
    VkImageView* swapChainImageViews;
    VkImage depthImage;
    VmaAllocation depthImageAllocation;
    VkImageView depthImageView;
    VkFormat depthImageFormat;

    shaderc_compiler_t shaderCompiler;
    VmaAllocator allocator;
    VkRenderPass renderPass;
    VkFramebuffer* swapChainFramebuffers;
    VkCommandPool commandPool;
    VkCommandBuffer transferCommandBuffer;
    VkCommandBuffer renderCommandBuffer;
    uint32_t imageIndex;
    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;
    VkFence transferFence;
    VkPhysicalDeviceProperties physicalDeviceProperties;
    VkDescriptorPool descriptorPool;
    VkDescriptorSetLayout bufferDescriptorSetLayout;
    VkDescriptorSetLayout textureDescriptorSetLayout;

    VkDescriptorSet bufferDescriptorSet;
    void *mappedUniformBuffers[RL_UNIFORM_BUFFER_COUNT];

    rlvkBuffer uniformBuffers[RL_UNIFORM_BUFFER_COUNT];
    rlvkAllocation uniformAllocations[RL_UNIFORM_BUFFER_COUNT];

    //-----------

    rlRenderBatch *currentBatch;            // Current render batch
    rlRenderBatch defaultBatch;             // Default internal render batch

    struct {
        int vertexCounter;                  // Current active render batch vertex counter (generic, used for all batches)
        int batchCounter;                   // Number of batches that have been drawn so far. Used for indexing the uniform buffers.
        float texcoordx, texcoordy;         // Current active texture coordinate (added on glVertex*())
        float normalx, normaly, normalz;    // Current active normal (added on glVertex*())
        unsigned char colorr, colorg, colorb, colora;   // Current active color (added on glVertex*())

        int currentMatrixMode;              // Current matrix mode
        Matrix *currentMatrix;              // Current matrix pointer
        Matrix modelview;                   // Default modelview matrix
        Matrix projection;                  // Default projection matrix
        Matrix transform;                   // Transform matrix to be used with rlTranslate, rlRotate, rlScale
        bool transformRequired;             // Require transform matrix application to current draw-call vertex (if required)
        Matrix stack[RL_MAX_MATRIX_STACK_SIZE];// Matrix stack for push/pop
        int stackCounter;                   // Matrix stack counter

        VkDescriptorSet currentTextureDescriptorSet;      // Current texture descriptor set to be used on rlBegin
        rlTexture defaultTexture;      // Default texture used on shapes/poly drawing (required by shader)
        unsigned int activeTextureId[RL_DEFAULT_BATCH_MAX_TEXTURE_UNITS];    // Active texture ids to be enabled on batch drawing (0 active by default)
        VkShaderModule defaultVShaderModule;      // Default vertex shader module
        VkShaderModule defaultFShaderModule;      // Default fragment shader module
        rlShaderProgram defaultShaderProgram;       // Default graphics shader program, supports vertex color and diffuse texture

        const rlShaderProgram *descriptorManagingCurrentBoundShaderProgram; // NOTE: This is ONLY for descriptor management. Do not use it for rendering.

        const rlShaderProgram *currentShaderProgram;       // Current shader program to be used on rendering

        bool stereoRender;                  // Stereo rendering flag
        Matrix projectionStereo[2];         // VR stereo rendering eyes projection matrices
        Matrix viewOffsetStereo[2];         // VR stereo rendering eyes view offset matrices

        // Dynamic state variables
        VkViewport viewport;
        VkRect2D scissor;

        // Blending variables
        int currentBlendMode;               // Blending mode active
        int glBlendSrcFactor;               // Blending source factor
        int glBlendDstFactor;               // Blending destination factor
        int glBlendEquation;                // Blending equation
        int glBlendSrcFactorRGB;            // Blending source RGB factor
        int glBlendDestFactorRGB;           // Blending destination RGB factor
        int glBlendSrcFactorAlpha;          // Blending source alpha factor
        int glBlendDestFactorAlpha;         // Blending destination alpha factor
        int glBlendEquationRGB;             // Blending equation for RGB
        int glBlendEquationAlpha;           // Blending equation for alpha
        bool glCustomBlendModeModified;     // Custom blending factor and equation modification status

        int framebufferWidth;               // Current framebuffer width
        int framebufferHeight;              // Current framebuffer height

    } State;            // Renderer state
    struct {
        bool vao;                           // VAO support (OpenGL ES2 could not support VAO extension) (GL_ARB_vertex_array_object)
        bool instancing;                    // Instancing supported (GL_ANGLE_instanced_arrays, GL_EXT_draw_instanced + GL_EXT_instanced_arrays)
        bool texNPOT;                       // NPOT textures full support (GL_ARB_texture_non_power_of_two, GL_OES_texture_npot)
        bool texDepth;                      // Depth textures supported (GL_ARB_depth_texture, GL_OES_depth_texture)
        bool texDepthWebGL;                 // Depth textures supported WebGL specific (GL_WEBGL_depth_texture)
        bool texFloat32;                    // float textures support (32 bit per channel) (GL_OES_texture_float)
        bool texCompDXT;                    // DDS texture compression support (GL_EXT_texture_compression_s3tc, GL_WEBGL_compressed_texture_s3tc, GL_WEBKIT_WEBGL_compressed_texture_s3tc)
        bool texCompETC1;                   // ETC1 texture compression support (GL_OES_compressed_ETC1_RGB8_texture, GL_WEBGL_compressed_texture_etc1)
        bool texCompETC2;                   // ETC2/EAC texture compression support (GL_ARB_ES3_compatibility)
        bool texCompPVRT;                   // PVR texture compression support (GL_IMG_texture_compression_pvrtc)
        bool texCompASTC;                   // ASTC texture compression support (GL_KHR_texture_compression_astc_hdr, GL_KHR_texture_compression_astc_ldr)
        bool texMirrorClamp;                // Clamp mirror wrap mode supported (GL_EXT_texture_mirror_clamp)
        bool texAnisoFilter;                // Anisotropic texture filtering support (GL_EXT_texture_filter_anisotropic)
        bool computeShader;                 // Compute shaders support (GL_ARB_compute_shader)
        bool ssbo;                          // Shader storage buffer object support (GL_ARB_shader_storage_buffer_object)

        float maxAnisotropyLevel;           // Maximum anisotropy level supported (minimum is 2.0f)
        int maxDepthBits;                   // Maximum bits for depth component

    } ExtSupported;     // Extensions supported flags
} rlvkData;

static const VmaAllocationCreateInfo sharedWriteAllocationCreateInfo = {
    .usage = VMA_MEMORY_USAGE_AUTO,
    .flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT,
    .requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
};

static const VmaAllocationCreateInfo deviceAllocationCreateInfo = {
    .usage = VMA_MEMORY_USAGE_AUTO,
    .requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
};

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static bool isGpuReady = false;
static double rlCullDistanceNear = RL_CULL_DISTANCE_NEAR;
static double rlCullDistanceFar = RL_CULL_DISTANCE_FAR;

static rlvkData RLVK = { 0 };

//----------------------------------------------------------------------------------
// Internal Functions
//----------------------------------------------------------------------------------

static Matrix rlMatrixIdentity(void)
{
    Matrix matIdentity = { 0 };
    matIdentity.m0 = 1.0f;
    matIdentity.m5 = 1.0f;
    matIdentity.m10 = 1.0f;
    matIdentity.m15 = 1.0f;

    return matIdentity;
}

typedef struct rl_float16 { float v[16]; } rl_float16;

static rl_float16 rlMatrixToFloatV(Matrix mat)
{
    rl_float16 result = { 0 };

    result.v[0] = mat.m0;
    result.v[1] = mat.m1;
    result.v[2] = mat.m2;
    result.v[3] = mat.m3;
    result.v[4] = mat.m4;
    result.v[5] = mat.m5;
    result.v[6] = mat.m6;
    result.v[7] = mat.m7;
    result.v[8] = mat.m8;
    result.v[9] = mat.m9;
    result.v[10] = mat.m10;
    result.v[11] = mat.m11;
    result.v[12] = mat.m12;
    result.v[13] = mat.m13;
    result.v[14] = mat.m14;
    result.v[15] = mat.m15;

    return result;
}

#define rlMatrixToFloat(mat) (rlMatrixToFloatV(mat).v)      // Get float vector for Matrix

static Matrix rlMatrixMultiply(Matrix left, Matrix right)
{
    Matrix result = { 0 };

    result.m0 = left.m0*right.m0 + left.m1*right.m4 + left.m2*right.m8 + left.m3*right.m12;
    result.m1 = left.m0*right.m1 + left.m1*right.m5 + left.m2*right.m9 + left.m3*right.m13;
    result.m2 = left.m0*right.m2 + left.m1*right.m6 + left.m2*right.m10 + left.m3*right.m14;
    result.m3 = left.m0*right.m3 + left.m1*right.m7 + left.m2*right.m11 + left.m3*right.m15;
    result.m4 = left.m4*right.m0 + left.m5*right.m4 + left.m6*right.m8 + left.m7*right.m12;
    result.m5 = left.m4*right.m1 + left.m5*right.m5 + left.m6*right.m9 + left.m7*right.m13;
    result.m6 = left.m4*right.m2 + left.m5*right.m6 + left.m6*right.m10 + left.m7*right.m14;
    result.m7 = left.m4*right.m3 + left.m5*right.m7 + left.m6*right.m11 + left.m7*right.m15;
    result.m8 = left.m8*right.m0 + left.m9*right.m4 + left.m10*right.m8 + left.m11*right.m12;
    result.m9 = left.m8*right.m1 + left.m9*right.m5 + left.m10*right.m9 + left.m11*right.m13;
    result.m10 = left.m8*right.m2 + left.m9*right.m6 + left.m10*right.m10 + left.m11*right.m14;
    result.m11 = left.m8*right.m3 + left.m9*right.m7 + left.m10*right.m11 + left.m11*right.m15;
    result.m12 = left.m12*right.m0 + left.m13*right.m4 + left.m14*right.m8 + left.m15*right.m12;
    result.m13 = left.m12*right.m1 + left.m13*right.m5 + left.m14*right.m9 + left.m15*right.m13;
    result.m14 = left.m12*right.m2 + left.m13*right.m6 + left.m14*right.m10 + left.m15*right.m14;
    result.m15 = left.m12*right.m3 + left.m13*right.m7 + left.m14*right.m11 + left.m15*right.m15;

    return result;
}


// Transposes provided matrix
static Matrix rlMatrixTranspose(Matrix mat)
{
    Matrix result = { 0 };

    result.m0 = mat.m0;
    result.m1 = mat.m4;
    result.m2 = mat.m8;
    result.m3 = mat.m12;
    result.m4 = mat.m1;
    result.m5 = mat.m5;
    result.m6 = mat.m9;
    result.m7 = mat.m13;
    result.m8 = mat.m2;
    result.m9 = mat.m6;
    result.m10 = mat.m10;
    result.m11 = mat.m14;
    result.m12 = mat.m3;
    result.m13 = mat.m7;
    result.m14 = mat.m11;
    result.m15 = mat.m15;

    return result;
}

// Invert provided matrix
static Matrix rlMatrixInvert(Matrix mat)
{
    Matrix result = { 0 };

    // Cache the matrix values (speed optimization)
    float a00 = mat.m0, a01 = mat.m1, a02 = mat.m2, a03 = mat.m3;
    float a10 = mat.m4, a11 = mat.m5, a12 = mat.m6, a13 = mat.m7;
    float a20 = mat.m8, a21 = mat.m9, a22 = mat.m10, a23 = mat.m11;
    float a30 = mat.m12, a31 = mat.m13, a32 = mat.m14, a33 = mat.m15;

    float b00 = a00*a11 - a01*a10;
    float b01 = a00*a12 - a02*a10;
    float b02 = a00*a13 - a03*a10;
    float b03 = a01*a12 - a02*a11;
    float b04 = a01*a13 - a03*a11;
    float b05 = a02*a13 - a03*a12;
    float b06 = a20*a31 - a21*a30;
    float b07 = a20*a32 - a22*a30;
    float b08 = a20*a33 - a23*a30;
    float b09 = a21*a32 - a22*a31;
    float b10 = a21*a33 - a23*a31;
    float b11 = a22*a33 - a23*a32;

    // Calculate the invert determinant (inlined to avoid double-caching)
    float invDet = 1.0f/(b00*b11 - b01*b10 + b02*b09 + b03*b08 - b04*b07 + b05*b06);

    result.m0 = (a11*b11 - a12*b10 + a13*b09)*invDet;
    result.m1 = (-a01*b11 + a02*b10 - a03*b09)*invDet;
    result.m2 = (a31*b05 - a32*b04 + a33*b03)*invDet;
    result.m3 = (-a21*b05 + a22*b04 - a23*b03)*invDet;
    result.m4 = (-a10*b11 + a12*b08 - a13*b07)*invDet;
    result.m5 = (a00*b11 - a02*b08 + a03*b07)*invDet;
    result.m6 = (-a30*b05 + a32*b02 - a33*b01)*invDet;
    result.m7 = (a20*b05 - a22*b02 + a23*b01)*invDet;
    result.m8 = (a10*b10 - a11*b08 + a13*b06)*invDet;
    result.m9 = (-a00*b10 + a01*b08 - a03*b06)*invDet;
    result.m10 = (a30*b04 - a31*b02 + a33*b00)*invDet;
    result.m11 = (-a20*b04 + a21*b02 - a23*b00)*invDet;
    result.m12 = (-a10*b09 + a11*b07 - a12*b06)*invDet;
    result.m13 = (a00*b09 - a01*b07 + a02*b06)*invDet;
    result.m14 = (-a30*b03 + a31*b01 - a32*b00)*invDet;
    result.m15 = (a20*b03 - a21*b01 + a22*b00)*invDet;

    return result;
}

#ifdef RLVK_ENABLE_VULKAN_VALIDATION_LAYER
static VKAPI_ATTR VkBool32 VKAPI_CALL rlVulkanDebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData)
{
    if (messageSeverity < VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        return VK_FALSE;
    }

    TRACELOG(LOG_WARNING, "Vulkan: Vulkan debug message: %s", pCallbackData->pMessage);
    TRACELOG(LOG_WARNING, "    > Type: %u", messageType);
    TRACELOG(LOG_WARNING, "    > Severity = %u", messageSeverity);

    return VK_FALSE;
}
#endif

void rlBeginFrame(void)
{
    vkWaitForFences(RLVK.device, 1, &RLVK.inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(RLVK.device, 1, &RLVK.inFlightFence);

    vkAcquireNextImageKHR(RLVK.device, RLVK.swapChain, UINT64_MAX, RLVK.imageAvailableSemaphore, VK_NULL_HANDLE, &RLVK.imageIndex);

    vkResetCommandBuffer(RLVK.transferCommandBuffer, 0);
    vkResetCommandBuffer(RLVK.renderCommandBuffer, 0);

    VkCommandBufferBeginInfo beginInfo =
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO
    };

    if (vkBeginCommandBuffer(RLVK.renderCommandBuffer, &beginInfo) != VK_SUCCESS)
    {
        TRACELOG(LOG_WARNING, "Vulkan: Failed to begin recording command buffer");
        return;
    }

    if (vkBeginCommandBuffer(RLVK.transferCommandBuffer, &beginInfo) != VK_SUCCESS)
    {
        TRACELOG(LOG_WARNING, "Vulkan: Failed to begin recording command buffer");
        return;
    }
    
    VkClearValue clearValues[] =
    {
        { .color = { 0, 0, 0, 0 } },
        { .depthStencil = { .depth = 1.0f, .stencil = 0 } }
    };

    VkRenderPassBeginInfo renderPassInfo =
    {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = RLVK.renderPass,
        .framebuffer = RLVK.swapChainFramebuffers[RLVK.imageIndex],
        .renderArea.offset = { 0, 0 },
        .renderArea.extent = RLVK.swapChainExtent,
        .clearValueCount = 2,
        .pClearValues = clearValues
    };

    vkCmdBeginRenderPass(RLVK.renderCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdSetViewport(RLVK.renderCommandBuffer, 0, 1, &RLVK.State.viewport);
    // vkCmdSetScissor(RLVK.renderCommandBuffer, 0, 1, &RLVK.State.scissor);
    rlDisableScissorTest();
    vkCmdSetPrimitiveTopology(RLVK.renderCommandBuffer, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

    rlDisableDepthTest();
    rlEnableDepthMask();
    rlEnableBackfaceCulling();
    rlSetCullFace(RL_CULL_FACE_BACK);
}

static void rlUpdateBatchBuffers(rlRenderBatch *batch)
{
    // NOTE: Vertex buffer offsets are precisely the amount of vertices in said buffer
    for (uint32_t i = 0; i < batch->bufferCount; i++)
    {
        const rlVertexBuffer *vb = &batch->vertexBuffer[i];

        void* mappedData;
        VkBufferCopy bufferCopy = { 0 };

        // Vertex positions buffer
        bufferCopy.size = vb->vertexOffset*3*sizeof(float);
        if (vmaMapMemory(RLVK.allocator, vb->stagingAllocations[0], &mappedData) != VK_SUCCESS)
        {
            TRACELOG(RL_LOG_WARNING, "Vma: Failed to map buffer");
            return;
        }
        memcpy(mappedData, vb->vertices, bufferCopy.size);
        vmaUnmapMemory(RLVK.allocator, vb->stagingAllocations[0]);
        vkCmdCopyBuffer(RLVK.transferCommandBuffer, vb->stagingBuffers[0], vb->deviceBuffers[0], 1, &bufferCopy);

        // Texture coordinates buffer
        bufferCopy.size = vb->vertexOffset*2*sizeof(float);
        if (vmaMapMemory(RLVK.allocator, vb->stagingAllocations[1], &mappedData) != VK_SUCCESS)
        {
            TRACELOG(RL_LOG_WARNING, "Vma: Failed to map buffer");
            return;
        }
        memcpy(mappedData, vb->texcoords, bufferCopy.size);
        vmaUnmapMemory(RLVK.allocator, vb->stagingAllocations[1]);
        vkCmdCopyBuffer(RLVK.transferCommandBuffer, vb->stagingBuffers[1], vb->deviceBuffers[1], 1, &bufferCopy);

        // Normals buffer
        bufferCopy.size = vb->vertexOffset*3*sizeof(float);
        if (vmaMapMemory(RLVK.allocator, vb->stagingAllocations[2], &mappedData) != VK_SUCCESS)
        {
            TRACELOG(RL_LOG_WARNING, "Vma: Failed to map buffer");
            return;
        }
        memcpy(mappedData, vb->normals, bufferCopy.size);
        vmaUnmapMemory(RLVK.allocator, vb->stagingAllocations[2]);
        vkCmdCopyBuffer(RLVK.transferCommandBuffer, vb->stagingBuffers[2], vb->deviceBuffers[2], 1, &bufferCopy);

        // Colors buffer
        bufferCopy.size = vb->vertexOffset*4*sizeof(uint8_t);
        if (vmaMapMemory(RLVK.allocator, vb->stagingAllocations[3], &mappedData) != VK_SUCCESS)
        {
            TRACELOG(RL_LOG_WARNING, "Vma: Failed to map buffer");
            return;
        }
        memcpy(mappedData, vb->colors, bufferCopy.size);
        vmaUnmapMemory(RLVK.allocator, vb->stagingAllocations[3]);
        vkCmdCopyBuffer(RLVK.transferCommandBuffer, vb->stagingBuffers[3], vb->deviceBuffers[3], 1, &bufferCopy);

        // Since we have uploaded the vertex buffer data, reset the vertex buffer offset.
        batch->vertexBuffer[i].vertexOffset = 0;
    }
}

void rlEndFrame(void)
{   
    rlUpdateBatchBuffers(RLVK.currentBatch);

    // Reset batch counter for next frame
    RLVK.State.batchCounter = 0;

    vkCmdEndRenderPass(RLVK.renderCommandBuffer);

    if (vkEndCommandBuffer(RLVK.transferCommandBuffer) != VK_SUCCESS)
    {
        TRACELOG(LOG_WARNING, "Vulkan: Failed to record command buffer");
        return;
    }

    if (vkEndCommandBuffer(RLVK.renderCommandBuffer) != VK_SUCCESS)
    {
        TRACELOG(LOG_WARNING, "Vulkan: Failed to record command buffer");
        return;
    }

    VkSemaphore waitSemaphores[] = { RLVK.imageAvailableSemaphore };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSemaphore signalSemaphores[] = { RLVK.renderFinishedSemaphore };

    VkCommandBuffer commandBuffers[] = { RLVK.transferCommandBuffer, RLVK.renderCommandBuffer };

    VkSubmitInfo submitInfo =
    {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = waitSemaphores,
        .pWaitDstStageMask = waitStages,
        .commandBufferCount = 2,
        .pCommandBuffers = commandBuffers,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = signalSemaphores
    };

    if (vkQueueSubmit(RLVK.graphicsQueue, 1, &submitInfo, RLVK.inFlightFence) != VK_SUCCESS)
    {
        TRACELOG(LOG_WARNING, "Vulkan: Failed to submit draw command buffer");
        return;
    }

    VkSwapchainKHR swapChains[] = { RLVK.swapChain };

    VkPresentInfoKHR presentInfo =
    {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = signalSemaphores,
        .swapchainCount = 1,
        .pSwapchains = swapChains,
        .pImageIndices = &RLVK.imageIndex,
        presentInfo.pResults = 0 //Optional
    };

    vkQueuePresentKHR(RLVK.presentQueue, &presentInfo);
}

//----------------------------------------------------------------------------------
// rlvk Implementation
//----------------------------------------------------------------------------------

void rlMatrixMode(int mode)                       // Choose the current matrix to be transformed 
{
    if (mode == RL_PROJECTION) RLVK.State.currentMatrix = &RLVK.State.projection;
    else if (mode == RL_MODELVIEW) RLVK.State.currentMatrix = &RLVK.State.modelview;
    //else if (mode == RL_TEXTURE) // Not supported

    RLVK.State.currentMatrixMode = mode;
}

void rlPushMatrix(void)                           // Push the current matrix to stack 
{
    if (RLVK.State.stackCounter >= RL_MAX_MATRIX_STACK_SIZE) TRACELOG(RL_LOG_ERROR, "RLVK: Matrix stack overflow (RL_MAX_MATRIX_STACK_SIZE)");

    if (RLVK.State.currentMatrixMode == RL_MODELVIEW)
    {
        RLVK.State.transformRequired = true;
        RLVK.State.currentMatrix = &RLVK.State.transform;
    }

    RLVK.State.stack[RLVK.State.stackCounter] = *RLVK.State.currentMatrix;
    RLVK.State.stackCounter++;
}

RLAPI void rlPopMatrix(void)                            // Pop latest inserted matrix from stack 
{
    if (RLVK.State.stackCounter > 0)
    {
        Matrix mat = RLVK.State.stack[RLVK.State.stackCounter - 1];
        *RLVK.State.currentMatrix = mat;
        RLVK.State.stackCounter--;
    }

    if ((RLVK.State.stackCounter == 0) && (RLVK.State.currentMatrixMode == RL_MODELVIEW))
    {
        RLVK.State.currentMatrix = &RLVK.State.modelview;
        RLVK.State.transformRequired = false;
    }
}

void rlLoadIdentity(void)                         // Reset current matrix to identity matrix 
{
    *RLVK.State.currentMatrix = rlMatrixIdentity();
}

void rlTranslatef(float x, float y, float z)      // Multiply the current matrix by a translation matrix 
{
    Matrix matTranslation = {
        1.0f, 0.0f, 0.0f, x,
        0.0f, 1.0f, 0.0f, y,
        0.0f, 0.0f, 1.0f, z,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    // NOTE: We transpose matrix with multiplication order
    *RLVK.State.currentMatrix = rlMatrixMultiply(matTranslation, *RLVK.State.currentMatrix);
}

void rlRotatef(float angle, float x, float y, float z)  // Multiply the current matrix by a rotation matrix 
{
    Matrix matRotation = rlMatrixIdentity();

    // Axis vector (x, y, z) normalization
    float lengthSquared = x*x + y*y + z*z;
    if ((lengthSquared != 1.0f) && (lengthSquared != 0.0f))
    {
        float inverseLength = 1.0f/sqrtf(lengthSquared);
        x *= inverseLength;
        y *= inverseLength;
        z *= inverseLength;
    }

    // Rotation matrix generation
    float sinres = sinf(DEG2RAD*angle);
    float cosres = cosf(DEG2RAD*angle);
    float t = 1.0f - cosres;

    matRotation.m0 = x*x*t + cosres;
    matRotation.m1 = y*x*t + z*sinres;
    matRotation.m2 = z*x*t - y*sinres;
    matRotation.m3 = 0.0f;

    matRotation.m4 = x*y*t - z*sinres;
    matRotation.m5 = y*y*t + cosres;
    matRotation.m6 = z*y*t + x*sinres;
    matRotation.m7 = 0.0f;

    matRotation.m8 = x*z*t + y*sinres;
    matRotation.m9 = y*z*t - x*sinres;
    matRotation.m10 = z*z*t + cosres;
    matRotation.m11 = 0.0f;

    matRotation.m12 = 0.0f;
    matRotation.m13 = 0.0f;
    matRotation.m14 = 0.0f;
    matRotation.m15 = 1.0f;

    // NOTE: We transpose matrix with multiplication order
    *RLVK.State.currentMatrix = rlMatrixMultiply(matRotation, *RLVK.State.currentMatrix);
}

void rlScalef(float x, float y, float z)          // Multiply the current matrix by a scaling matrix 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlScalef was called.");
}

void rlMultMatrixf(const float *matf)             // Multiply the current matrix by another matrix 
{
        // Matrix creation from array
    Matrix mat = { matf[0], matf[4], matf[8], matf[12],
                   matf[1], matf[5], matf[9], matf[13],
                   matf[2], matf[6], matf[10], matf[14],
                   matf[3], matf[7], matf[11], matf[15] };

    *RLVK.State.currentMatrix = rlMatrixMultiply(*RLVK.State.currentMatrix, mat);
}

void rlFrustum(double left, double right, double bottom, double top, double znear, double zfar)  
{
    Matrix matFrustum = { 0 };

    float rl = (float)(right - left);
    float tb = (float)(top - bottom);
    float fn = (float)(zfar - znear);

    matFrustum.m0 = ((float) znear*2.0f)/rl;
    matFrustum.m1 = 0.0f;
    matFrustum.m2 = 0.0f;
    matFrustum.m3 = 0.0f;

    matFrustum.m4 = 0.0f;
    matFrustum.m5 = ((float) znear*2.0f)/tb;
    matFrustum.m6 = 0.0f;
    matFrustum.m7 = 0.0f;

    matFrustum.m8 = ((float)right + (float)left)/rl;
    matFrustum.m9 = ((float)top + (float)bottom)/tb;
    matFrustum.m10 = -((float)zfar)/fn;
    matFrustum.m11 = -1.0f;

    matFrustum.m12 = 0.0f;
    matFrustum.m13 = 0.0f;
    matFrustum.m14 = -((float)zfar*(float)znear)/fn;
    matFrustum.m15 = 0.0f;

    *RLVK.State.currentMatrix = rlMatrixMultiply(*RLVK.State.currentMatrix, matFrustum);
}

void rlOrtho(double left, double right, double bottom, double top, double znear, double zfar)  
{
    // NOTE: If left-right and top-botton values are equal it could create a division by zero,
    // response to it is platform/compiler dependant
    Matrix matOrtho = { 0 };

    float rl = (float)(right - left);
    float tb = (float)(top - bottom);
    float fn = (float)(zfar - znear);

    matOrtho.m0 = 2.0f/rl;
    matOrtho.m1 = 0.0f;
    matOrtho.m2 = 0.0f;
    matOrtho.m3 = 0.0f;
    matOrtho.m4 = 0.0f;
    matOrtho.m5 = 2.0f/tb;
    matOrtho.m6 = 0.0f;
    matOrtho.m7 = 0.0f;
    matOrtho.m8 = 0.0f;
    matOrtho.m9 = 0.0f;
    matOrtho.m10 = -1.0f/fn;
    matOrtho.m11 = 0.0f;
    matOrtho.m12 = -((float)left + (float)right)/rl;
    matOrtho.m13 = -((float)top + (float)bottom)/tb;
    matOrtho.m14 = -((float)znear)/fn;
    matOrtho.m15 = 1.0f;

    *RLVK.State.currentMatrix = rlMatrixMultiply(*RLVK.State.currentMatrix, matOrtho);
}

void rlViewport(int x, int y, int width, int height)  // Set the viewport area 
{
    RLVK.State.viewport.x = x;
    RLVK.State.viewport.y = y;
    RLVK.State.viewport.width = width;
    RLVK.State.viewport.height = height;

    vkCmdSetViewport(RLVK.renderCommandBuffer, 0, 1, &RLVK.State.viewport);
}

// Set clip planes distances
void rlSetClipPlanes(double nearPlane, double farPlane)
{
    rlCullDistanceNear = nearPlane;
    rlCullDistanceFar = farPlane;
}

// Get cull plane distance near
double rlGetCullDistanceNear(void)
{
    return rlCullDistanceNear;
}

// Get cull plane distance far
double rlGetCullDistanceFar(void)
{
    return rlCullDistanceFar;
}

void rlBegin(int mode)                            // Initialize drawing mode (how to organize vertex) 
{
    // Draw mode can be RL_LINES, RL_TRIANGLES and RL_QUADS
    // NOTE: In all three cases, vertex are accumulated over default internal vertex buffer
    if (RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].mode != mode)
    {
        if (RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].vertexCount > 0)
        {
            // Make sure current RLVK.currentBatch->draws[i].vertexCount is aligned a multiple of 4,
            // that way, following QUADS drawing will keep aligned with index processing
            // It implies adding some extra alignment vertex at the end of the draw,
            // those vertex are not processed but they are considered as an additional offset
            // for the next set of vertex to be drawn
            if (RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].mode == RL_LINES) RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].vertexAlignment = ((RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].vertexCount < 4)? RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].vertexCount : RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].vertexCount%4);
            else if (RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].mode == RL_TRIANGLES) RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].vertexAlignment = ((RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].vertexCount < 4)? 1 : (4 - (RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].vertexCount%4)));
            else RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].vertexAlignment = 0;

            if (!rlCheckRenderBatchLimit(RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].vertexAlignment))
            {
                RLVK.State.vertexCounter += RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].vertexAlignment;
                RLVK.currentBatch->drawCounter++;
            }
        }

        if (RLVK.currentBatch->drawCounter >= RL_DEFAULT_BATCH_DRAWCALLS) rlDrawRenderBatch(RLVK.currentBatch);

        RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].mode = mode;
        // TODO: Implement this
        RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].textureDescriptorSet = RLVK.State.currentTextureDescriptorSet;
        RLVK.State.currentTextureDescriptorSet = RLVK.State.defaultTexture.descriptorSet;
    }
}

void rlEnd(void)                                  // Finish vertex providing 
{
    RLVK.currentBatch->currentDepth += (1.0f/20000.0f);
}

// Define one vertex (position)
// NOTE: Vertex position data is the basic information required for drawing
void rlVertex3f(float x, float y, float z)
{
    float tx = x;
    float ty = y;
    float tz = z;

    // Transform provided vector if required
    if (RLVK.State.transformRequired)
    {
        tx = RLVK.State.transform.m0*x + RLVK.State.transform.m4*y + RLVK.State.transform.m8*z + RLVK.State.transform.m12;
        ty = RLVK.State.transform.m1*x + RLVK.State.transform.m5*y + RLVK.State.transform.m9*z + RLVK.State.transform.m13;
        tz = RLVK.State.transform.m2*x + RLVK.State.transform.m6*y + RLVK.State.transform.m10*z + RLVK.State.transform.m14;
    }

    // WARNING: Be careful with primitives breaking when launching a new batch!
    // RL_LINES comes in pairs, RL_TRIANGLES come in groups of 3 vertices and RL_QUADS come in groups of 4 vertices
    // Checking current draw.mode when a new vertex is required and finish the batch only if the draw.mode draw.vertexCount is %2, %3 or %4
    if (RLVK.State.vertexCounter > (RLVK.currentBatch->vertexBuffer[RLVK.currentBatch->currentBuffer].elementCount*4 - 4))
    {
        if ((RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].mode == RL_LINES) &&
            (RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].vertexCount%2 == 0))
        {
            // Reached the maximum number of vertices for RL_LINES drawing
            // Launch a draw call but keep current state for next vertices comming
            // NOTE: Adding +1 vertex to the check for some safety
            rlCheckRenderBatchLimit(2 + 1);
        }
        else if ((RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].mode == RL_TRIANGLES) &&
            (RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].vertexCount%3 == 0))
        {
            rlCheckRenderBatchLimit(3 + 1);
        }
        else if ((RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].mode == RL_QUADS) &&
            (RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].vertexCount%4 == 0))
        {
            rlCheckRenderBatchLimit(4 + 1);
        }
    }

    rlVertexBuffer *vb = &RLVK.currentBatch->vertexBuffer[RLVK.currentBatch->currentBuffer];
    
    uint32_t vertexCounter = vb->vertexOffset + RLVK.State.vertexCounter;

    // Add vertices
    vb->vertices[3*vertexCounter] = tx;
    vb->vertices[3*vertexCounter + 1] = ty;
    vb->vertices[3*vertexCounter + 2] = tz;

    // Add current texcoord
    vb->texcoords[2*vertexCounter] = RLVK.State.texcoordx;
    vb->texcoords[2*vertexCounter + 1] = RLVK.State.texcoordy;

    // Add current normal
    vb->normals[3*vertexCounter] = RLVK.State.normalx;
    vb->normals[3*vertexCounter + 1] = RLVK.State.normaly;
    vb->normals[3*vertexCounter + 2] = RLVK.State.normalz;

    // Add current color
    vb->colors[4*vertexCounter] = RLVK.State.colorr;
    vb->colors[4*vertexCounter + 1] = RLVK.State.colorg;
    vb->colors[4*vertexCounter + 2] = RLVK.State.colorb;
    vb->colors[4*vertexCounter + 3] = RLVK.State.colora;

    RLVK.State.vertexCounter++;
    RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].vertexCount++;
}

// Define one vertex (position)
void rlVertex2f(float x, float y)
{
    rlVertex3f(x, y, RLVK.currentBatch->currentDepth);
}

// Define one vertex (position)
void rlVertex2i(int x, int y)
{
    rlVertex3f((float)x, (float)y, RLVK.currentBatch->currentDepth);
}

// Define one vertex (texture coordinate)
// NOTE: Texture coordinates are limited to QUADS only
void rlTexCoord2f(float x, float y)
{
    RLVK.State.texcoordx = x;
    RLVK.State.texcoordy = y;
}

// Define one vertex (normal)
// NOTE: Normals limited to TRIANGLES only?
void rlNormal3f(float x, float y, float z)
{
    float normalx = x;
    float normaly = y;
    float normalz = z;
    if (RLVK.State.transformRequired)
    {
        normalx = RLVK.State.transform.m0*x + RLVK.State.transform.m4*y + RLVK.State.transform.m8*z;
        normaly = RLVK.State.transform.m1*x + RLVK.State.transform.m5*y + RLVK.State.transform.m9*z;
        normalz = RLVK.State.transform.m2*x + RLVK.State.transform.m6*y + RLVK.State.transform.m10*z;
    }

    // NOTE: Default behavior assumes the normal vector is in the correct space for what the shader expects,
    // it could be not normalized to 0.0f..1.0f, magnitud can be useed for some effects
    /*
    // WARNING: Vector normalization if required
    float length = sqrtf(normalx*normalx + normaly*normaly + normalz*normalz);
    if (length != 0.0f)
    {
        float ilength = 1.0f/length;
        normalx *= ilength;
        normaly *= ilength;
        normalz *= ilength;
    }
    */
    RLVK.State.normalx = normalx;
    RLVK.State.normaly = normaly;
    RLVK.State.normalz = normalz;
}

// Define one vertex (color)
void rlColor4ub(unsigned char x, unsigned char y, unsigned char z, unsigned char w)
{
    RLVK.State.colorr = x;
    RLVK.State.colorg = y;
    RLVK.State.colorb = z;
    RLVK.State.colora = w;
}

// Define one vertex (color)
void rlColor4f(float r, float g, float b, float a)
{
    rlColor4ub((unsigned char)(r*255), (unsigned char)(g*255), (unsigned char)(b*255), (unsigned char)(a*255));
}

// Define one vertex (color)
void rlColor3f(float x, float y, float z)
{
    rlColor4ub((unsigned char)(x*255), (unsigned char)(y*255), (unsigned char)(z*255), 255);
}

bool rlEnableVertexArray(unsigned int vaoId)      // Enable vertex array (VAO, if supported) 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlEnableVertexArray was called.");
	return false;
}

void rlDisableVertexArray(void)                   // Disable vertex array (VAO, if supported) 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlDisableVertexArray was called.");
}

void rlEnableVertexBuffer(unsigned int id)        // Enable vertex buffer (VBO) 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlEnableVertexBuffer was called.");
}

void rlDisableVertexBuffer(void)                  // Disable vertex buffer (VBO) 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlDisableVertexBuffer was called.");
}

void rlEnableVertexBufferElement(unsigned int id)  // Enable vertex buffer element (VBO element) 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlEnableVertexBufferElement was called.");
}

void rlDisableVertexBufferElement(void)           // Disable vertex buffer element (VBO element) 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlDisableVertexBufferElement was called.");
}

void rlEnableVertexAttribute(unsigned int index)  // Enable vertex attribute index 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlEnableVertexAttribute was called.");
}

void rlDisableVertexAttribute(unsigned int index)  // Disable vertex attribute index 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlDisableVertexAttribute was called.");
}

void rlEnableStatePointer(int vertexAttribType, void *buffer)  // Enable attribute state pointer 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlEnableStatePointer was called.");
}

void rlDisableStatePointer(int vertexAttribType)  // Disable attribute state pointer 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlDisableStatePointer was called.");
}

void rlActiveTextureSlot(int slot)                // Select and active a texture slot 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlActiveTextureSlot was called.");
}

void rlEnableTexture(const rlTexture* texture)             // Enable texture 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlEnableTexture was called.");
}

void rlDisableTexture(void)                       // Disable texture 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlDisableTexture was called.");
}

void rlEnableTextureCubemap(const rlTexture* texture)      // Enable texture cubemap 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlEnableTextureCubemap was called.");
}

void rlDisableTextureCubemap(void)                // Disable texture cubemap 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlDisableTextureCubemap was called.");
}

void rlTextureParameters(const rlTexture* texture, int param, int value)  // Set texture parameters (filter, wrap) 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlTextureParameters was called.");
}

void rlCubemapParameters(const rlTexture* texture, int param, int value)  // Set cubemap parameters (filter, wrap) 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlCubemapParameters was called.");
}

void rlEnableShader(const rlShaderProgram *program)              // Enable shader program 
{
    RLVK.State.descriptorManagingCurrentBoundShaderProgram = program;
    // vkCmdBindPipeline(RLVK.renderCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, program->pipeline);
}

void rlDisableShader(void)                        // Disable shader program 
{
    RLVK.State.descriptorManagingCurrentBoundShaderProgram = NULL;
}

void rlEnableFramebuffer(unsigned int id)         // Enable render texture (fbo) 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlEnableFramebuffer was called.");
}

void rlDisableFramebuffer(void)                   // Disable render texture (fbo), return to default framebuffer 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlDisableFramebuffer was called.");
}

unsigned int rlGetActiveFramebuffer(void)         // Get the currently active render texture (fbo), 0 for default framebuffer 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlGetActiveFramebuffer was called.");
	return 0;
}

void rlActiveDrawBuffers(int count)               // Activate multiple draw color buffers 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlActiveDrawBuffers was called.");
}

void rlBlitFramebuffer(int srcX, int srcY, int srcWidth, int srcHeight, int dstX, int dstY, int dstWidth, int dstHeight, int bufferMask)  // Blit active framebuffer to main framebuffer 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlBlitFramebuffer was called.");
}

void rlBindFramebuffer(unsigned int target, unsigned int framebuffer)  // Bind framebuffer (FBO) 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlBindFramebuffer was called.");
}

void rlEnableColorBlend(void)                     // Enable color blending 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlEnableColorBlend was called.");
}

void rlDisableColorBlend(void)                    // Disable color blending 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlDisableColorBlend was called.");
}

void rlEnableDepthTest(void)                      // Enable depth test 
{
    vkCmdSetDepthTestEnable(RLVK.renderCommandBuffer, VK_TRUE);
}

void rlDisableDepthTest(void)                     // Disable depth test 
{
    vkCmdSetDepthTestEnable(RLVK.renderCommandBuffer, VK_FALSE);
}

void rlEnableDepthMask(void)                      // Enable depth write 
{
    vkCmdSetDepthWriteEnable(RLVK.renderCommandBuffer, VK_TRUE);
}

void rlDisableDepthMask(void)                     // Disable depth write 
{
    vkCmdSetDepthWriteEnable(RLVK.renderCommandBuffer, VK_FALSE);
}

void rlEnableBackfaceCulling(void)                // Enable backface culling 
{
    vkCmdSetCullMode(RLVK.renderCommandBuffer, VK_CULL_MODE_BACK_BIT);
}

void rlDisableBackfaceCulling(void)               // Disable backface culling 
{
    vkCmdSetCullMode(RLVK.renderCommandBuffer, VK_CULL_MODE_NONE);
}

void rlColorMask(bool r, bool g, bool b, bool a)  // Color mask control 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlColorMask was called.");
}

void rlSetCullFace(int mode)                      // Set face culling mode 
{
    switch (mode)
    {
        case RL_CULL_FACE_BACK: vkCmdSetFrontFace(RLVK.renderCommandBuffer, VK_FRONT_FACE_COUNTER_CLOCKWISE); break;
        case RL_CULL_FACE_FRONT: vkCmdSetFrontFace(RLVK.renderCommandBuffer, VK_FRONT_FACE_CLOCKWISE); break;
        default: break;
    }
}

void rlEnableScissorTest(void)                    // Enable scissor test 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlEnableScissorTest was called.");
}

void rlDisableScissorTest(void)                   // Disable scissor test 
{
    VkRect2D scissor =
    {
        .offset = { 0, 0 },
        .extent = RLVK.swapChainExtent
    };

    vkCmdSetScissor(RLVK.renderCommandBuffer, 0, 1, &scissor);
}

void rlScissor(int x, int y, int width, int height)  // Scissor test 
{
    y = RLVK.swapChainExtent.height - y - height;

    if (x < 0) x = 0;
    if (y < 0) y = 0;

    RLVK.State.scissor.offset.x = x;
    RLVK.State.scissor.offset.y = y;
    RLVK.State.scissor.extent.width = width;
    RLVK.State.scissor.extent.height = height;

    vkCmdSetScissor(RLVK.renderCommandBuffer, 0, 1, &RLVK.State.scissor);
}

void rlEnablePointMode(void)                      // Enable point mode 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlEnablePointMode was called.");
}

void rlDisablePointMode(void)                     // Disable point mode 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlDisablePointMode was called.");
}

void rlSetPointSize(float size)                   // Set the point drawing size 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlSetPointSize was called.");
}

float rlGetPointSize(void)                        // Get the point drawing size 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlGetPointSize was called.");
	return 0.0f;
}

void rlEnableWireMode(void)                       // Enable wire mode 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlEnableWireMode was called.");
}

void rlDisableWireMode(void)                      // Disable wire mode 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlDisableWireMode was called.");
}

void rlSetLineWidth(float width)                  // Set the line drawing width 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlSetLineWidth was called.");
}

float rlGetLineWidth(void)                        // Get the line drawing width 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlGetLineWidth was called.");
	return 0.0f;
}

void rlEnableSmoothLines(void)                    // Enable line aliasing 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlEnableSmoothLines was called.");
}

void rlDisableSmoothLines(void)                   // Disable line aliasing 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlDisableSmoothLines was called.");
}

void rlEnableStereoRender(void)                   // Enable stereo rendering 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlEnableStereoRender was called.");
}

void rlDisableStereoRender(void)                  // Disable stereo rendering 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlDisableStereoRender was called.");
}

bool rlIsStereoRenderEnabled(void)                // Check if stereo render is enabled 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlIsStereoRenderEnabled was called.");
	return false;
}

void rlClearColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)  // Clear color buffer with color 
{
    rlBegin(RL_QUADS);

    rlColor4ub(r, g, b, a);
    rlVertex2f(RLVK.swapChainExtent.width, 0.0f);
    
    rlColor4ub(r, g, b, a);
    rlVertex2f(0.0f, 0.0f);
    
    rlColor4ub(r, g, b, a);
    rlVertex2f(0.0f, RLVK.swapChainExtent.height);
    
    rlColor4ub(r, g, b, a);
    rlVertex2f(RLVK.swapChainExtent.width, RLVK.swapChainExtent.height);

    rlEnd();
}

void rlClearScreenBuffers(void)                   // Clear used screen buffers (color and depth) 
{
}

void rlCheckErrors(void)                          // Check and log OpenGL error codes 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlCheckErrors was called.");
}

void rlSetBlendMode(int mode)                     // Set blending mode 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlSetBlendMode was called.");
}

void rlSetBlendFactors(int glSrcFactor, int glDstFactor, int glEquation)  // Set blending mode factor and equation (using OpenGL factors) 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlSetBlendFactors was called.");
}

void rlSetBlendFactorsSeparate(int glSrcRGB, int glDstRGB, int glSrcAlpha, int glDstAlpha, int glEqRGB, int glEqAlpha)  // Set blending mode factors and equations separately (using OpenGL factors) 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlSetBlendFactorsSeparate was called.");
}

static VkFormat rlGetSupportedFormat(VkPhysicalDevice physicalDevice, int formatCount, const VkFormat *formats, VkImageTiling tiling, VkFormatFeatureFlags featureFlags)
{
    for (int i = 0; i < formatCount; i++)
    {
        VkFormat format = formats[i];

        VkFormatProperties properties;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &properties);

        if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & featureFlags) == featureFlags)
        {
            return format;
        }

        if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & featureFlags) == featureFlags)
        {
            return format;
        }
    }

    return VK_FORMAT_MAX_ENUM;
}

void rlvkInit(int width, int height, GLFWwindow *windowHandle)              // Initialize rlvk (instance, device, surface, swapchain, etc.) 
{
    VkApplicationInfo appInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = "Raylib Application", //TODO: Get auctual app name here? Set to null? Keep it like this?
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "Raylib",
        .engineVersion = VK_MAKE_VERSION(4, 5, 0),
        .apiVersion = VK_API_VERSION_1_3
    };

    uint32_t vulkanExtensionCount = 0;
    char** glfwExtensions = (char**)glfwGetRequiredInstanceExtensions(&vulkanExtensionCount);
    char** vulkanExtensions = RL_MALLOC(vulkanExtensionCount * sizeof(char*));

    for (uint32_t i = 0; i < vulkanExtensionCount; ++i)
    {
        vulkanExtensions[i] = glfwExtensions[i];
    }

    VkInstanceCreateFlags createInfoFlags = 0;

#ifdef PLATFORM_APPLE
    //According to https://vulkan.lunarg.com/doc/sdk/1.3.216.0/mac/getting_started.html
    //Beginning with the 1.3.216 Vulkan SDK, the VK_KHR_PORTABILITY_subset extension is mandatory for MoltenVK (Vulkan porting to metal)
    vulkanExtensions = RL_REALLOC(vulkanExtensions, ++vulkanExtensionCount * sizeof(char*));
    vulkanExtensions[vulkanExtensionCount - 1] = VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME;
    createInfoFlags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

    //TODO: Maybe print available Vulkan extensions? Raylib likes printing stuff like this to the console

#ifdef RLVK_ENABLE_VULKAN_VALIDATION_LAYER
    vulkanExtensions = RL_REALLOC(vulkanExtensions, ++vulkanExtensionCount * sizeof(char*));
    vulkanExtensions[vulkanExtensionCount - 1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    
    //TODO: Check for validation layer support?

    char* validationLayers[] =
    {
        "VK_LAYER_KHRONOS_validation"
    };
    
    VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo =
    {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = 
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | 
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = 
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | 
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | 
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = rlVulkanDebugCallback,
        .pUserData = 0 // Optional
    };
#endif

    VkInstanceCreateInfo instanceCreateInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &appInfo,
        .enabledExtensionCount = vulkanExtensionCount,
        .ppEnabledExtensionNames = (char const* const*) vulkanExtensions,
        .flags = createInfoFlags,
#ifdef RLVK_ENABLE_VULKAN_VALIDATION_LAYER
        .enabledLayerCount = 1,
        .ppEnabledLayerNames = (char const* const*)validationLayers,
        .pNext = &debugMessengerCreateInfo
#else
        .enabledLayerCount = 0
#endif
    };

    if (vkCreateInstance(&instanceCreateInfo, 0, &RLVK.instance) != VK_SUCCESS)
    {
        TRACELOG(LOG_WARNING, "Vulkan: Failed to initialize Instance");
        return;
    }

    RL_FREE(vulkanExtensions);
    
#ifdef RLVK_ENABLE_VULKAN_VALIDATION_LAYER
    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = 
        (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(RLVK.instance, "vkCreateDebugUtilsMessengerEXT");

    if (vkCreateDebugUtilsMessengerEXT == 0)
    {
        TRACELOG(LOG_WARNING, "Vulkan: Failed to get vkCreateDebugUtilsMessengerEXT. Vulkan validation layers are probably not supported on this machine.");
        return;
    }
    
    if (vkCreateDebugUtilsMessengerEXT(RLVK.instance, &debugMessengerCreateInfo, 0, &RLVK.debugMessenger) != VK_SUCCESS)
    {
        TRACELOG(LOG_WARNING, "Vulkan: Failed to initialize Debug Messenger");
        return;
    }
#endif

    //Platform dependent surface creation
    if (glfwCreateWindowSurface(RLVK.instance, windowHandle, 0, &RLVK.surface) != VK_SUCCESS)
    {
        TRACELOG(LOG_WARNING, "GLFW, Vulkan: Failed to create a window surface");
        return;
    }

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    uint32_t graphicsFamily = 0;
    uint32_t presentFamily = 0;

    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(RLVK.instance, &physicalDeviceCount, 0);

    if (physicalDeviceCount == 0)
    {
        TRACELOG(LOG_WARNING, "Vulkan: Could not find any GPU with Vulkan support");
        return;
    }

    uint32_t requiredDeviceExtensionCount = 1;
    char* requiredDeviceExtensions[] =
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    //We can only get the devices from Vulkan. There is no feature to let Vulkan pick a device.
    //So we will use a custom scoring system to pick the best device.
    {
        uint32_t* physicaDeviceGraphicsFamilies = RL_MALLOC(physicalDeviceCount * sizeof(uint32_t));
        uint32_t* physicaDevicePresentFamilies = RL_MALLOC(physicalDeviceCount * sizeof(uint32_t));
        int32_t* physicaDeviceScores = RL_MALLOC(physicalDeviceCount * sizeof(int32_t));
        VkPhysicalDevice* physicalDevices = RL_MALLOC(physicalDeviceCount * sizeof(VkPhysicalDevice));
        vkEnumeratePhysicalDevices(RLVK.instance, &physicalDeviceCount, physicalDevices);

        for (uint32_t i = 0; i < physicalDeviceCount; ++i)
        {
            VkPhysicalDevice* device = &physicalDevices[i];

            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceProperties(*device, &deviceProperties);

            RLVK.physicalDeviceProperties = deviceProperties;

            VkPhysicalDeviceFeatures deviceFeatures;
            vkGetPhysicalDeviceFeatures(*device, &deviceFeatures);

            uint32_t queueFamilyCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(*device, &queueFamilyCount, 0);

            VkQueueFamilyProperties* queueFamilies = RL_MALLOC(queueFamilyCount * sizeof(VkQueueFamilyProperties));
            vkGetPhysicalDeviceQueueFamilyProperties(*device, &queueFamilyCount, queueFamilies);

            bool foundGraphicsFamily = false;
            bool foundPresentFamily = false;

            for (uint32_t j = 0; j < queueFamilyCount; ++j)
            {
                VkQueueFamilyProperties* queueFamily = &queueFamilies[j];

                if (queueFamily->queueFlags & VK_QUEUE_GRAPHICS_BIT)
                {
                    foundGraphicsFamily = true;
                    physicaDeviceGraphicsFamilies[i] = j;
                }

                VkBool32 presentSupport = false;
                vkGetPhysicalDeviceSurfaceSupportKHR(*device, j, RLVK.surface, &presentSupport);

                if (presentSupport)
                {
                    foundPresentFamily = true;
                    physicaDevicePresentFamilies[i] = j;
                }
                
                if (foundPresentFamily && foundGraphicsFamily)
                {
                    break;
                }
            }

            RL_FREE(queueFamilies);

            if (!foundGraphicsFamily || !foundPresentFamily)
            {
                physicaDeviceScores[i] = 0;
                continue;
            }

            uint32_t extensionCount;
            vkEnumerateDeviceExtensionProperties(*device, 0, &extensionCount, 0);

            VkExtensionProperties* availableExtensions = RL_MALLOC(extensionCount * sizeof(VkExtensionProperties));
            vkEnumerateDeviceExtensionProperties(*device, 0, &extensionCount, availableExtensions);

            bool* requiredExtensionAvailables = RL_MALLOC(requiredDeviceExtensionCount * sizeof(bool));

            for (uint32_t j = 0; j < requiredDeviceExtensionCount; ++j)
            {
                requiredExtensionAvailables[j] = false;
            }
            
            for (uint32_t j = 0; j < extensionCount; ++j)
            {
                for (uint32_t k = 0; k < requiredDeviceExtensionCount; ++k)
                {
                    if (strcmp(availableExtensions[j].extensionName, requiredDeviceExtensions[k]) == 0)
                    {
                        requiredExtensionAvailables[k] = true;
                    }
                }
            }
            
            bool extensionsSupported = true;

            for (uint32_t j = 0; j < requiredDeviceExtensionCount; ++j)
            {
                if (!requiredExtensionAvailables[j])
                {
                    extensionsSupported = false;
                    break;
                }
            }

            if (!extensionsSupported)
            {
                physicaDeviceScores[i] = 0;
                continue;
            }
            
            uint32_t surfaceFormatCount;
            vkGetPhysicalDeviceSurfaceFormatsKHR(*device, RLVK.surface, &surfaceFormatCount, 0);
            
            uint32_t presentModeCount;
            vkGetPhysicalDeviceSurfacePresentModesKHR(*device, RLVK.surface, &presentModeCount, 0);

            if (surfaceFormatCount == 0 || presentModeCount == 0)
            {
                physicaDeviceScores[i] = 0;
                continue;
            }

            physicaDeviceScores[i] = 1; //Initalize memory to 1, since 0 means the device is not suitable
            
            if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                physicaDeviceScores[i] += 1000;
            }

            //Prefer devices where the same queue family supports drawing and presentation,
            //since these have a significantly higher performance (significantly does not mean a lot here, it means significant)
            if (physicaDeviceGraphicsFamilies[i] == physicaDevicePresentFamilies[i])
            {
                physicaDeviceScores[i] += 1000;
            }

            physicaDeviceScores[i] += deviceProperties.limits.maxImageDimension2D;
        }
        
        int32_t highestScore = 0;

        for (uint32_t i = 0; i < physicalDeviceCount; ++i)
        {
            if (physicaDeviceScores[i] > highestScore)
            {
                highestScore = physicaDeviceScores[i];
                physicalDevice = physicalDevices[i];
                graphicsFamily = physicaDeviceGraphicsFamilies[i];
                presentFamily = physicaDevicePresentFamilies[i];
            }
        }

        RL_FREE(physicalDevices);
        RL_FREE(physicaDeviceScores);
        RL_FREE(physicaDevicePresentFamilies);
        RL_FREE(physicaDeviceGraphicsFamilies);
    }

    if (physicalDevice == 0)
    {
        TRACELOG(LOG_WARNING, "RLVK: Could not find a suitable physical device");
        return;
    }
    
    float queuePriority = 1.0f;

    VkDeviceQueueCreateInfo graphicsQueueCreateInfo =
    {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = graphicsFamily,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority
    };

    VkDeviceQueueCreateInfo presentQueueCreateInfo =
    {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = presentFamily,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority
    };
    
    VkDeviceQueueCreateInfo queueCreateInfos[] =
    {
        graphicsQueueCreateInfo,
        presentQueueCreateInfo
    };

    //If the present family and the graphics family the same we don't need to create seperate queues,
    //because the families are not seperate
    uint32_t queuesToCreate = presentFamily == graphicsFamily ? 1 : 2;

    VkPhysicalDeviceFeatures deviceFeatures = { 0 };

    VkDeviceCreateInfo deviceCreateInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pQueueCreateInfos = queueCreateInfos,
        .queueCreateInfoCount = queuesToCreate,
        .pEnabledFeatures = &deviceFeatures,
        .enabledExtensionCount = requiredDeviceExtensionCount,
        .ppEnabledExtensionNames = (char const* const*)requiredDeviceExtensions,
        .enabledLayerCount = 0
    };
    
    if (vkCreateDevice(physicalDevice, &deviceCreateInfo, 0, &RLVK.device) != VK_SUCCESS)
    {
        TRACELOG(LOG_WARNING, "Vulkan: Failed to create a logical device");
        return;
    }

    vkGetDeviceQueue(RLVK.device, graphicsFamily, 0, &RLVK.graphicsQueue);
    vkGetDeviceQueue(RLVK.device, presentFamily, 0, &RLVK.presentQueue);

    VkFormat swapChainImageFormat;
    
    {
        VkSurfaceCapabilitiesKHR capabilities;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, RLVK.surface, &capabilities);

        if (capabilities.currentExtent.width != UINT32_MAX)
        {
            RLVK.swapChainExtent = capabilities.currentExtent;
        }
        else
        {
            VkExtent2D actualExtent = { (uint32_t)width, (uint32_t)height };

            //Clamp
            if (actualExtent.width < capabilities.minImageExtent.width)
            {
                actualExtent.width = capabilities.minImageExtent.width;
            }

            if (actualExtent.width > capabilities.maxImageExtent.width)
            {
                actualExtent.width = capabilities.maxImageExtent.width;
            }

            //Clamp
            if (actualExtent.height < capabilities.minImageExtent.height)
            {
                actualExtent.height = capabilities.minImageExtent.height;
            }

            if (actualExtent.width > capabilities.maxImageExtent.height)
            {
                actualExtent.height = capabilities.maxImageExtent.height;
            }

            RLVK.swapChainExtent = actualExtent;
        }

        RLVK.swapChainImageCount = capabilities.minImageCount + 1;

        if (capabilities.maxImageCount > 0 && RLVK.swapChainImageCount > capabilities.maxImageCount)
        {
            RLVK.swapChainImageCount = capabilities.maxImageCount;
        }

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, RLVK.surface, &formatCount, 0);

        VkSurfaceFormatKHR surfaceFormat;
        VkSurfaceFormatKHR* formats = RL_MALLOC(formatCount * sizeof(VkSurfaceFormatKHR));
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, RLVK.surface, &formatCount, formats);

        bool foundAvailableFormat = false;

        if (formatCount == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
        {
            //This means we can choose any format
			surfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM;
            surfaceFormat.colorSpace = formats[0].colorSpace; //TODO: Idk what to put here lol
		}

        for (uint32_t i = 0; i < formatCount; ++i)
        {
            VkSurfaceFormatKHR* availableFormat = &formats[i];

            //For UNORM
            if (availableFormat->format == VK_FORMAT_B8G8R8A8_UNORM)
            {
                foundAvailableFormat = true;
                surfaceFormat = *availableFormat;
                break;
            }
        }

        if (!foundAvailableFormat)
        {
            surfaceFormat = formats[0];
        }

        RL_FREE(formats);

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, RLVK.surface, &presentModeCount, 0);

        VkPresentModeKHR presentMode;
        VkPresentModeKHR* presentModes = 0;
        presentModes = RL_MALLOC(presentModeCount * sizeof(VkPresentModeKHR));
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, RLVK.surface, &presentModeCount, presentModes);

        /* Note for people that are new to Vulkan:
            - VK_PRESENT_MODE_IMMEDIATE_KHR: No VSync
            - VK_PRESENT_MODE_FIFO_KHR: There is a queue of the drawn frames and the GPU will just display those frames in VSync timing,
                if the queue is full, the CPU is blocked and it has to wait for the GPU
            - VK_PRESENT_MODE_MAILBOX_KHR: There is a queue of the drawn frames which the GPU will display in a VSync timing,
                if the queue is full, the oldest frame is discraded and the newest frame is inserted

            VK_PRESENT_MODE_FIFO_KHR is guarenteed to be available on every system.
        */

        bool foundAvailablePresentMode = false;

        for (uint32_t i = 0; i < presentModeCount; ++i)
        {
            VkPresentModeKHR availablePresentMode = presentModes[i];

            if (RLVK.vSync && availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                presentMode = availablePresentMode;
                break;
            }

            if (!RLVK.vSync && availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
            {
                presentMode = availablePresentMode;
                break;
            }
        }
        
        if (!foundAvailablePresentMode)
        {
            presentMode = VK_PRESENT_MODE_FIFO_KHR;
        }
        
        RL_FREE(presentModes);

        swapChainImageFormat = surfaceFormat.format;

        VkSwapchainCreateInfoKHR createInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .surface = RLVK.surface,
            .minImageCount = RLVK.swapChainImageCount,
            .imageFormat = surfaceFormat.format,
            .imageColorSpace = surfaceFormat.colorSpace,
            .imageExtent = RLVK.swapChainExtent,
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .preTransform = capabilities.currentTransform,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = presentMode,
            .clipped = VK_TRUE,
            .oldSwapchain = VK_NULL_HANDLE
        };
        
        uint32_t queueFamilyIndices[] = { graphicsFamily, presentFamily };

        if (graphicsFamily != presentFamily)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;
            createInfo.pQueueFamilyIndices = 0;
        }

        if (vkCreateSwapchainKHR(RLVK.device, &createInfo, 0, &RLVK.swapChain) != VK_SUCCESS)
        {
            TRACELOG(LOG_WARNING, "Vulkan: Failed to create a swap chain");
            return;
        }
    }

    vkGetSwapchainImagesKHR(RLVK.device, RLVK.swapChain, &RLVK.swapChainImageCount, 0);

    RLVK.swapChainImages = RL_MALLOC(RLVK.swapChainImageCount * sizeof(VkImage));
    vkGetSwapchainImagesKHR(RLVK.device, RLVK.swapChain, &RLVK.swapChainImageCount, RLVK.swapChainImages);

    RLVK.swapChainImageViews = RL_MALLOC(RLVK.swapChainImageCount * sizeof(VkImageView));

    for (uint32_t i = 0; i < RLVK.swapChainImageCount; ++i)
    {
        VkImageViewCreateInfo createInfo =
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = RLVK.swapChainImages[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = swapChainImageFormat,
            .components.r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .components.a = VK_COMPONENT_SWIZZLE_IDENTITY,
            .subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .subresourceRange.baseMipLevel = 0,
            .subresourceRange.levelCount = 1,
            .subresourceRange.baseArrayLayer = 0,
            .subresourceRange.layerCount = 1
        };

        if (vkCreateImageView(RLVK.device, &createInfo, 0, &RLVK.swapChainImageViews[i]) != VK_SUCCESS)
        {
            TRACELOG(LOG_WARNING, "Vulkan: Failed to create an image view");
            return;
        }
    }

    VmaAllocatorCreateInfo allocatorCreateInfo = {
        .instance = RLVK.instance,
        .physicalDevice = physicalDevice,
        .device = RLVK.device,
        .pAllocationCallbacks = NULL,
        .pDeviceMemoryCallbacks = NULL,
        .vulkanApiVersion = VK_API_VERSION_1_3,
        .flags = 0 // Don't think any are needed
    };

    if (vmaCreateAllocator(&allocatorCreateInfo, &RLVK.allocator) != VK_SUCCESS)
    {
        TRACELOG(LOG_WARNING, "shaderc: Failed to create shaderc compiler");
        return;
    }

    if ((RLVK.shaderCompiler = shaderc_compiler_initialize()) == NULL)
    {
        TRACELOG(LOG_WARNING, "shaderc: Failed to create shaderc compiler");
        return;
    }

    VkFormat depthImageFormats[] = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };

    RLVK.depthImageFormat = rlGetSupportedFormat(physicalDevice, 3, depthImageFormats, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    if (RLVK.depthImageFormat == VK_FORMAT_MAX_ENUM) {
        TRACELOG(LOG_WARNING, "Vulkan: Desired depth format unavailable");
        return;
    }

    VkImageCreateInfo depthImageInfo
    = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .mipLevels = 1,
        .arrayLayers = 1,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .extent.width = RLVK.swapChainExtent.width,
        .extent.height = RLVK.swapChainExtent.height,
        .extent.depth = 1,
        .format = RLVK.depthImageFormat,
        .usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
    };

    if (vmaCreateImage(RLVK.allocator, &depthImageInfo, &deviceAllocationCreateInfo, &RLVK.depthImage, &RLVK.depthImageAllocation, NULL) != VK_SUCCESS)
    {
        TRACELOG(LOG_WARNING, "Vulkan: Failed to create depth image");
        return;
    }

    VkImageViewCreateInfo depthImageViewInfo =
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .components.r = VK_COMPONENT_SWIZZLE_IDENTITY,
        .components.g = VK_COMPONENT_SWIZZLE_IDENTITY,
        .components.b = VK_COMPONENT_SWIZZLE_IDENTITY,
        .components.a = VK_COMPONENT_SWIZZLE_IDENTITY,
        .subresourceRange.baseMipLevel = 0,
        .subresourceRange.levelCount = 1,
        .subresourceRange.baseArrayLayer = 0,
        .subresourceRange.layerCount = 1,
        .subresourceRange.aspectMask = (RLVK.depthImageFormat == VK_FORMAT_D32_SFLOAT_S8_UINT || RLVK.depthImageFormat == VK_FORMAT_D24_UNORM_S8_UINT) ? VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT : VK_IMAGE_ASPECT_DEPTH_BIT,
        .image = RLVK.depthImage,
        .format = RLVK.depthImageFormat,
    };
    
    if (vkCreateImageView(RLVK.device, &depthImageViewInfo, NULL, &RLVK.depthImageView) != VK_SUCCESS) {
        TRACELOG(LOG_WARNING, "Vulkan: Failed to create depth image view");
        return;
    }

    VkAttachmentDescription attachments[] =
    {
        { // Color attachment
            .format = swapChainImageFormat,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
        },
        { // Depth attachment
            .format = RLVK.depthImageFormat,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
            .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
        }
    };

    VkAttachmentReference colorAttachmentRef =
    {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    VkAttachmentReference depthAttachmentRef =
    {
        .attachment = 1,
        .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };

    VkSubpassDescription subpass =
    {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentRef,
        .pDepthStencilAttachment = &depthAttachmentRef
    };

    VkSubpassDependency dependency =
    {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        .srcAccessMask = 0,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT
    };

    VkRenderPassCreateInfo renderPassInfo =
    {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 2,
        .pAttachments = attachments,
        .subpassCount = 1,
        .pSubpasses = &subpass,
        .dependencyCount = 1,
        .pDependencies = &dependency
    };

    if (vkCreateRenderPass(RLVK.device, &renderPassInfo, 0, &RLVK.renderPass) != VK_SUCCESS)
    {
        TRACELOG(LOG_WARNING, "Vulkan: Failed to create render pass");
        return;
    }
    
    const char defaultVShaderCode[] =
    "#version 450                       \n"
    "layout(location = 0) in vec3 vertexPosition;            \n"
    "layout(location = 1) in vec2 vertexTexCoord;            \n"
    "layout(location = 3) in vec4 vertexColor;               \n"
    "layout(location = 0) out vec2 fragTexCoord;             \n"
    "layout(location = 1) out vec4 fragColor;                \n"
    "layout(set = 0, binding = 0) uniform MVP {              \n"
    "    mat4 mvp;                                           \n"
    "};                                                      \n"
    "void main()                        \n"
    "{                                  \n"
    "    fragTexCoord = vertexTexCoord; \n"
    "    fragColor = vertexColor;       \n"
    "    gl_Position = mvp*vec4(vertexPosition, 1.0); \n"
    "}                                  \n\0";

    const char defaultFShaderCode[] =
    "#version 450                       \n"
    "layout(location = 0) in vec2 fragTexCoord;              \n"
    "layout(location = 1) in vec4 fragColor;                 \n"
    "layout(location = 0) out vec4 finalColor;               \n"
    "layout(set = 1, binding = 0) uniform sampler2D texture0;        \n"
    "void main()                        \n"
    "{                                  \n"
    "    vec4 texelColor = texture(texture0, fragTexCoord);   \n"
    "    finalColor = texelColor*fragColor;        \n"
    "}                                  \n";

    if ((RLVK.State.defaultVShaderModule = rlLoadShader(defaultVShaderCode, RL_VERTEX_SHADER)) == VK_NULL_HANDLE)
    {
        return;
    }
    if ((RLVK.State.defaultFShaderModule = rlLoadShader(defaultFShaderCode, RL_FRAGMENT_SHADER)) == VK_NULL_HANDLE)
    {
        return;
    }
    
    RLVK.State.viewport = (VkViewport)
    {
        .x = 0.0f,
        .y = 0.0f,
        .width = (float)RLVK.swapChainExtent.width,
        .height = (float)RLVK.swapChainExtent.height,
        .minDepth = 0.0f,
        .maxDepth = 1.0f
    };

    RLVK.State.scissor = (VkRect2D)
    {
        .offset = { 0, 0 },
        .extent = RLVK.swapChainExtent
    };

    VkDescriptorPoolSize poolSizes[] = {
        {
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
            .descriptorCount = RL_UNIFORM_BUFFER_COUNT
        },
        {
            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = 1
        }
    };

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .poolSizeCount = 2,
        .pPoolSizes = poolSizes,
        .maxSets = RL_DEFAULT_MAX_TEXTURES // TODO: Update this properly
    };

    if (vkCreateDescriptorPool(RLVK.device, &descriptorPoolCreateInfo, NULL, &RLVK.descriptorPool) != VK_SUCCESS)
    {
        TRACELOG(LOG_WARNING, "Vulkan: Failed to create descriptor pool");
        return;
    }

    VkDescriptorSetLayoutBinding descriptorSetBindings[] = {
        {
            .descriptorCount = 1,
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT
        }
    };

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = RL_UNIFORM_BUFFER_COUNT,
        .pBindings = descriptorSetBindings
    };

    if (vkCreateDescriptorSetLayout(RLVK.device, &descriptorSetLayoutCreateInfo, 0, &RLVK.bufferDescriptorSetLayout) != VK_SUCCESS)
    {
        TRACELOG(LOG_WARNING, "Vulkan: Failed to create descriptor set layout");
        return;
    }

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = RLVK.descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &RLVK.bufferDescriptorSetLayout
    };

    if (vkAllocateDescriptorSets(RLVK.device, &descriptorSetAllocateInfo, &RLVK.bufferDescriptorSet) != VK_SUCCESS)
    {
        TRACELOG(RL_LOG_WARNING, "Vma: Failed to create descriptor sets");
        return;
    }

    VkDescriptorBufferInfo uniformBufferInfos[RL_UNIFORM_BUFFER_COUNT];
    VkWriteDescriptorSet uniformWrites[RL_UNIFORM_BUFFER_COUNT];

    for (uint32_t i = 0; i < RL_UNIFORM_BUFFER_COUNT; i++)
    {
        VkWriteDescriptorSet write = {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = RLVK.bufferDescriptorSet,
            .dstArrayElement = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
            .descriptorCount = 1,
            .pBufferInfo = &uniformBufferInfos[i]
        };
        uniformWrites[i] = write;
    }

    VkBufferCreateInfo uniformBufferCreateInfo =
    {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
    };

    // location 0 matrix mvp view everything
    uniformBufferCreateInfo.size = 5*4*4*sizeof(float)*RL_DEFAULT_BATCH_COUNT_PER_FRAME;
    if (vmaCreateBuffer(RLVK.allocator, &uniformBufferCreateInfo, &sharedWriteAllocationCreateInfo, &RLVK.uniformBuffers[0], &RLVK.uniformAllocations[0], NULL) != VK_SUCCESS)
    {
        TRACELOG(RL_LOG_WARNING, "Vma: Failed to create uniform buffer");
        return;
    }
    vmaMapMemory(RLVK.allocator, RLVK.uniformAllocations[0], &RLVK.mappedUniformBuffers[0]);
    uniformBufferInfos[0] = (VkDescriptorBufferInfo) {
        .buffer = RLVK.uniformBuffers[0],
        .range = 5*4*4*sizeof(float)
    };
    uniformWrites[0].dstBinding = 0;

    vkUpdateDescriptorSets(RLVK.device, RL_UNIFORM_BUFFER_COUNT, uniformWrites, 0, NULL);

    VkDescriptorSetLayoutBinding textureDescriptorSetBindings[] = {
        {
            .descriptorCount = 1,
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
        }
    };

    VkDescriptorSetLayoutCreateInfo textureDescriptorSetLayoutCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = 1,
        .pBindings = textureDescriptorSetBindings
    };

    if (vkCreateDescriptorSetLayout(RLVK.device, &textureDescriptorSetLayoutCreateInfo, 0, &RLVK.textureDescriptorSetLayout) != VK_SUCCESS)
    {
        TRACELOG(LOG_WARNING, "Vulkan: Failed to create descriptor set layout");
        return;
    }

    RLVK.State.defaultShaderProgram = rlLoadShaderProgramEx(RLVK.State.defaultVShaderModule, RLVK.State.defaultFShaderModule);
    RLVK.State.currentShaderProgram = &RLVK.State.defaultShaderProgram;
    
    RLVK.swapChainFramebuffers = RL_MALLOC(RLVK.swapChainImageCount * sizeof(VkFramebuffer));

    for (size_t i = 0; i < RLVK.swapChainImageCount; ++i)
    {
        VkImageView attachments[] =
        {
            RLVK.swapChainImageViews[i],
            RLVK.depthImageView
        };

        VkFramebufferCreateInfo framebufferInfo =
        {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = RLVK.renderPass,
            .attachmentCount = 2,
            .pAttachments = attachments,
            .width = RLVK.swapChainExtent.width,
            .height = RLVK.swapChainExtent.height,
            .layers = 1
        };

        if (vkCreateFramebuffer(RLVK.device, &framebufferInfo, 0, &RLVK.swapChainFramebuffers[i]) != VK_SUCCESS)
        {
            TRACELOG(LOG_WARNING, "Vulkan: Failed to create framebuffer");
            return;
        }
    }

    VkCommandPoolCreateInfo poolInfo =
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = graphicsFamily
    };

    if (vkCreateCommandPool(RLVK.device, &poolInfo, 0, &RLVK.commandPool) != VK_SUCCESS)
    {
        TRACELOG(LOG_WARNING, "Vulkan: Failed to create command pool");
        return;
    }

    VkCommandBufferAllocateInfo allocInfo =
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = RLVK.commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 2
    };

    VkCommandBuffer commandBuffers[2];

    if (vkAllocateCommandBuffers(RLVK.device, &allocInfo, commandBuffers) != VK_SUCCESS)
    {
        TRACELOG(LOG_WARNING, "Vulkan: Failed to allocate command buffer");
        return;
    }

    RLVK.transferCommandBuffer = commandBuffers[0];
    RLVK.renderCommandBuffer = commandBuffers[1];

    VkSemaphoreCreateInfo semaphoreInfo =
    {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
    };

    VkFenceCreateInfo fenceInfo =
    {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT
    };

    VkFenceCreateInfo transferFenceInfo =
    {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO
    };

    if (vkCreateSemaphore(RLVK.device, &semaphoreInfo, 0, &RLVK.imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(RLVK.device, &semaphoreInfo, 0, &RLVK.renderFinishedSemaphore) != VK_SUCCESS ||
        vkCreateFence(RLVK.device, &fenceInfo, 0, &RLVK.inFlightFence) != VK_SUCCESS ||
        vkCreateFence(RLVK.device, &transferFenceInfo, 0, &RLVK.transferFence) != VK_SUCCESS
    )
    {
        TRACELOG(LOG_WARNING, "Vulkan: Failed to create semaphores");
        return;
    }

    uint8_t pixels[4] = { 255, 255, 255, 255 };   // 1 pixel RGBA (4 bytes)
    RLVK.State.defaultTexture = rlLoadTexture(pixels, 1, 1, RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, 1);
    RLVK.State.currentTextureDescriptorSet = RLVK.State.defaultTexture.descriptorSet;

    RLVK.defaultBatch = rlLoadRenderBatch(RL_DEFAULT_BATCH_BUFFERS, RL_DEFAULT_BATCH_BUFFER_ELEMENTS);
    RLVK.currentBatch = &RLVK.defaultBatch;

    // Init stack matrices (emulating OpenGL 1.1)
    for (int i = 0; i < RL_MAX_MATRIX_STACK_SIZE; i++) RLVK.State.stack[i] = rlMatrixIdentity();

    // Init internal matrices
    RLVK.State.transform = rlMatrixIdentity();
    RLVK.State.projection = rlMatrixIdentity();
    RLVK.State.modelview = rlMatrixIdentity();
    RLVK.State.currentMatrix = &RLVK.State.modelview;
    RLVK.State.framebufferWidth = width;
    RLVK.State.framebufferHeight = height;

    RLVK.inited = true;
}

void rlvkClose(void)                              // De-initialize rlvk (instance, device, surface, swapchain, etc.) 
{
    for (uint32_t i = 0; i < RL_UNIFORM_BUFFER_COUNT; i++)
    {
        vmaUnmapMemory(RLVK.allocator, RLVK.uniformAllocations[i]);
        vmaDestroyBuffer(RLVK.allocator, RLVK.uniformBuffers[i], RLVK.uniformAllocations[i]);
    }

    vkDestroyDescriptorSetLayout(RLVK.device, RLVK.textureDescriptorSetLayout, NULL);
    vkDestroyDescriptorSetLayout(RLVK.device, RLVK.bufferDescriptorSetLayout, NULL);
    vkDestroyDescriptorPool(RLVK.device, RLVK.descriptorPool, NULL);

    vkDestroySemaphore(RLVK.device, RLVK.imageAvailableSemaphore, 0);
    vkDestroySemaphore(RLVK.device, RLVK.renderFinishedSemaphore, 0);
    vkDestroyFence(RLVK.device, RLVK.inFlightFence, 0);
    vkDestroyFence(RLVK.device, RLVK.transferFence, 0);

    vkDestroyCommandPool(RLVK.device, RLVK.commandPool, 0);

    for (uint32_t i = 0; i < RLVK.swapChainImageCount; ++i)
    {
        vkDestroyFramebuffer(RLVK.device, RLVK.swapChainFramebuffers[i], 0);
    }
    
    RL_FREE(RLVK.swapChainFramebuffers);

    vkDestroyImageView(RLVK.device, RLVK.depthImageView, NULL);
    vmaDestroyImage(RLVK.allocator, RLVK.depthImage, RLVK.depthImageAllocation);

    rlUnloadTexture(&RLVK.State.defaultTexture);

    rlUnloadShaderProgram(&RLVK.State.defaultShaderProgram);
    vkDestroyRenderPass(RLVK.device, RLVK.renderPass, 0);

    rlUnloadRenderBatch(RLVK.defaultBatch);

    rlUnloadShader(RLVK.State.defaultVShaderModule);
    rlUnloadShader(RLVK.State.defaultFShaderModule);

    shaderc_compiler_release(RLVK.shaderCompiler);

    vmaDestroyAllocator(RLVK.allocator);

    for (uint32_t i = 0; i < RLVK.swapChainImageCount; ++i)
    {
        vkDestroyImageView(RLVK.device, RLVK.swapChainImageViews[i], 0);
    }

    RL_FREE(RLVK.swapChainImageViews);
    
    vkDestroySwapchainKHR(RLVK.device, RLVK.swapChain, 0);
    vkDestroySurfaceKHR(RLVK.instance, RLVK.surface, 0);
    vkDestroyDevice(RLVK.device, 0);

#ifdef RLVK_ENABLE_VULKAN_VALIDATION_LAYER
    PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = 
        (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(RLVK.instance, "vkDestroyDebugUtilsMessengerEXT");

    if (vkDestroyDebugUtilsMessengerEXT == 0)
    {
        TRACELOG(LOG_WARNING, "Vulkan: Failed to get vkDestroyDebugUtilsMessengerEXT. Vulkan validation layers are probably not supported on this machine.");
    }

    vkDestroyDebugUtilsMessengerEXT(RLVK.instance, RLVK.debugMessenger, 0);
#endif
    
    vkDestroyInstance(RLVK.instance, 0);
}

void rlLoadExtensions(void *loader)               // Load OpenGL extensions (loader function required) 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlLoadExtensions was called.");
}

void *rlGetProcAddress(const char *procName)      // Get OpenGL procedure address 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlGetProcAddress was called.");
	return NULL;
}

int rlGetVersion(void)                            // Get current OpenGL version 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlGetVersion was called.");
	return 0;
}

void rlSetFramebufferWidth(int width)             // Set current framebuffer width 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlSetFramebufferWidth was called.");
}

int rlGetFramebufferWidth(void)                   // Get default framebuffer width 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlGetFramebufferWidth was called.");
	return 0;
}

void rlSetFramebufferHeight(int height)           // Set current framebuffer height 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlSetFramebufferHeight was called.");
}

int rlGetFramebufferHeight(void)                  // Get default framebuffer height 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlGetFramebufferHeight was called.");
	return 0;
}

rlTexture *rlGetTextureIdDefault(void)          // Get default texture id 
{
    return &RLVK.State.defaultTexture;
}

rlShaderProgram *rlGetShaderIdDefault(void)           // Get default shader id 
{
	return &RLVK.State.defaultShaderProgram;
}

int *rlGetShaderLocsDefault(void)                 // Get default shader locations 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlGetShaderLocsDefault was called.");

	return NULL;
}

rlRenderBatch rlLoadRenderBatch(int numBuffers, int bufferElements)  // Load a render batch system 
{
	rlRenderBatch batch = { 0 };

    // Setup quad index buffer
    uint16_t *quadIndices = (uint16_t *)RL_CALLOC(bufferElements*6, sizeof(uint16_t));

    int k = 0;

    // Indices can be initialized right now
    for (int j = 0; j < (6*bufferElements); j += 6)
    {
        quadIndices[j] = 4*k;
        quadIndices[j + 1] = 4*k + 1;
        quadIndices[j + 2] = 4*k + 2;
        quadIndices[j + 3] = 4*k;
        quadIndices[j + 4] = 4*k + 2;
        quadIndices[j + 5] = 4*k + 3;

        k++;
    }

    VkBufferCreateInfo stagingBufferCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        .size = bufferElements*6*sizeof(uint16_t)
    };

    VkBufferCreateInfo deviceBufferCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        .size = bufferElements*6*sizeof(uint16_t)
    };

    VkBuffer indexStagingBuffer;
    VmaAllocation indexStagingAllocation;

    if (vmaCreateBuffer(RLVK.allocator, &stagingBufferCreateInfo, &sharedWriteAllocationCreateInfo, &indexStagingBuffer, &indexStagingAllocation, NULL) != VK_SUCCESS)
    {
        TRACELOG(RL_LOG_WARNING, "Vma: Failed to create staging buffer");
        return batch;
    }
    
    if (vmaCreateBuffer(RLVK.allocator, &deviceBufferCreateInfo, &deviceAllocationCreateInfo, &batch.quadIndexBuffer, &batch.quadIndexAllocation, NULL) != VK_SUCCESS)
    {
        TRACELOG(RL_LOG_WARNING, "Vma: Failed to create device buffer");
        return batch;
    }

    VkCommandBufferBeginInfo beginInfo =
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };

    if (vkBeginCommandBuffer(RLVK.transferCommandBuffer, &beginInfo) != VK_SUCCESS)
    {
        TRACELOG(LOG_WARNING, "Vulkan: Failed to begin recording command buffer");
        return batch;
    }

    void* mappedData;
    VkBufferCopy bufferCopy = {
        .size = bufferElements*6*sizeof(uint16_t)
    };

    if (vmaMapMemory(RLVK.allocator, indexStagingAllocation, &mappedData) != VK_SUCCESS)
    {
        TRACELOG(RL_LOG_WARNING, "Vma: Failed to map buffer");
        return batch;
    }
    memcpy(mappedData, quadIndices, bufferElements*6*sizeof(uint16_t));
    vmaUnmapMemory(RLVK.allocator, indexStagingAllocation);
    vkCmdCopyBuffer(RLVK.transferCommandBuffer, indexStagingBuffer, batch.quadIndexBuffer, 1, &bufferCopy);

    if (vkEndCommandBuffer(RLVK.transferCommandBuffer) != VK_SUCCESS)
    {
        TRACELOG(LOG_WARNING, "Vulkan: Failed to record command buffer");
        return batch;
    }

    VkSubmitInfo submitInfo =
    {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &RLVK.transferCommandBuffer
    };

    if (vkQueueSubmit(RLVK.graphicsQueue, 1, &submitInfo, RLVK.transferFence) != VK_SUCCESS)
    {
        TRACELOG(LOG_WARNING, "Vulkan: Failed to submit transfer command buffer");
        return batch;
    }

    vkWaitForFences(RLVK.device, 1, &RLVK.transferFence, VK_TRUE, UINT64_MAX);
    vkResetFences(RLVK.device, 1, &RLVK.transferFence);
    vkResetCommandBuffer(RLVK.transferCommandBuffer, 0);

    vmaDestroyBuffer(RLVK.allocator, indexStagingBuffer, indexStagingAllocation);

    // Initialize CPU (RAM) vertex buffers (position, texcoord, color data and indexes)
    //--------------------------------------------------------------------------------------------
    batch.vertexBuffer = (rlVertexBuffer *)RL_CALLOC(numBuffers, sizeof(rlVertexBuffer));

    for (int i = 0; i < numBuffers; i++)
    {
        batch.vertexBuffer[i].elementCount = bufferElements;
        batch.vertexBuffer[i].vertexOffset = 0;

        batch.vertexBuffer[i].vertices = (float *)RL_CALLOC(bufferElements*3*4, sizeof(float));     // 3 float by vertex, 4 vertex by quad
        batch.vertexBuffer[i].texcoords = (float *)RL_CALLOC(bufferElements*2*4, sizeof(float));    // 2 float by texcoord, 4 texcoord by quad
        batch.vertexBuffer[i].normals = (float *)RL_CALLOC(bufferElements*3*4, sizeof(float));      // 3 float by vertex, 4 vertex by quad
        batch.vertexBuffer[i].colors = (uint8_t *)RL_CALLOC(bufferElements*4*4, sizeof(uint8_t));   // 4 float by color, 4 colors by quad

        for (int j = 0; j < (3*4*bufferElements); j++) batch.vertexBuffer[i].vertices[j] = 0.0f;
        for (int j = 0; j < (2*4*bufferElements); j++) batch.vertexBuffer[i].texcoords[j] = 0.0f;
        for (int j = 0; j < (3*4*bufferElements); j++) batch.vertexBuffer[i].normals[j] = 0.0f;
        for (int j = 0; j < (4*4*bufferElements); j++) batch.vertexBuffer[i].colors[j] = 0;

        RLVK.State.vertexCounter = 0;
    }

    TRACELOG(RL_LOG_INFO, "RLVK: Render batch vertex buffers loaded successfully in RAM (CPU)");
    //--------------------------------------------------------------------------------------------

    // Upload to GPU (VRAM) vertex data and initialize VAOs/VBOs
    //--------------------------------------------------------------------------------------------
    stagingBufferCreateInfo = (VkBufferCreateInfo) {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    };

    deviceBufferCreateInfo = (VkBufferCreateInfo) {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
    };
    for (int i = 0; i < numBuffers; i++)
    {
        rlVertexBuffer *vb = &batch.vertexBuffer[i];

        // Vertex position buffer (shader-location = 0)
        stagingBufferCreateInfo.size = deviceBufferCreateInfo.size = bufferElements*3*4*sizeof(float);

        if (vmaCreateBuffer(RLVK.allocator, &stagingBufferCreateInfo, &sharedWriteAllocationCreateInfo, &vb->stagingBuffers[0], &vb->stagingAllocations[0], NULL) != VK_SUCCESS)
        {
            TRACELOG(RL_LOG_WARNING, "Vma: Failed to create staging buffer");
            return batch;
        }
        
        if (vmaCreateBuffer(RLVK.allocator, &deviceBufferCreateInfo, &deviceAllocationCreateInfo, &vb->deviceBuffers[0], &vb->deviceAllocations[0], NULL) != VK_SUCCESS)
        {
            TRACELOG(RL_LOG_WARNING, "Vma: Failed to create device buffer");
            return batch;
        }

        // Vertex texcoord buffer (shader-location = 1)
        stagingBufferCreateInfo.size = deviceBufferCreateInfo.size = bufferElements*2*4*sizeof(float);

        if (vmaCreateBuffer(RLVK.allocator, &stagingBufferCreateInfo, &sharedWriteAllocationCreateInfo, &vb->stagingBuffers[1], &vb->stagingAllocations[1], NULL) != VK_SUCCESS)
        {
            TRACELOG(RL_LOG_WARNING, "Vma: Failed to create staging buffer");
            return batch;
        }
        
        if (vmaCreateBuffer(RLVK.allocator, &deviceBufferCreateInfo, &deviceAllocationCreateInfo, &vb->deviceBuffers[1], &vb->deviceAllocations[1], NULL) != VK_SUCCESS)
        {
            TRACELOG(RL_LOG_WARNING, "Vma: Failed to create device buffer");
            return batch;
        }

        // Vertex normal buffer (shader-location = 2)
        stagingBufferCreateInfo.size = deviceBufferCreateInfo.size = bufferElements*3*4*sizeof(float);

        if (vmaCreateBuffer(RLVK.allocator, &stagingBufferCreateInfo, &sharedWriteAllocationCreateInfo, &vb->stagingBuffers[2], &vb->stagingAllocations[2], NULL) != VK_SUCCESS)
        {
            TRACELOG(RL_LOG_WARNING, "Vma: Failed to create staging buffer");
            return batch;
        }
        
        if (vmaCreateBuffer(RLVK.allocator, &deviceBufferCreateInfo, &deviceAllocationCreateInfo, &vb->deviceBuffers[2], &vb->deviceAllocations[2], NULL) != VK_SUCCESS)
        {
            TRACELOG(RL_LOG_WARNING, "Vma: Failed to create device buffer");
            return batch;
        }

        // Vertex color buffer (shader-location = 3)
        stagingBufferCreateInfo.size = deviceBufferCreateInfo.size = bufferElements*4*4*sizeof(float);

        if (vmaCreateBuffer(RLVK.allocator, &stagingBufferCreateInfo, &sharedWriteAllocationCreateInfo, &vb->stagingBuffers[3], &vb->stagingAllocations[3], NULL) != VK_SUCCESS)
        {
            TRACELOG(RL_LOG_WARNING, "Vma: Failed to create staging buffer");
            return batch;
        }
        
        if (vmaCreateBuffer(RLVK.allocator, &deviceBufferCreateInfo, &deviceAllocationCreateInfo, &vb->deviceBuffers[3], &vb->deviceAllocations[3], NULL) != VK_SUCCESS)
        {
            TRACELOG(RL_LOG_WARNING, "Vma: Failed to create device buffer");
            return batch;
        }
    }

    TRACELOG(RL_LOG_INFO, "RLVK: Render batch vertex buffers loaded successfully in VRAM (GPU)");

    // Init draw calls tracking system
    //--------------------------------------------------------------------------------------------
    batch.draws = (rlDrawCall *)RL_CALLOC(RL_DEFAULT_BATCH_DRAWCALLS, sizeof(rlDrawCall));

    for (int i = 0; i < RL_DEFAULT_BATCH_DRAWCALLS; i++)
    {
        batch.draws[i].mode = RL_QUADS;
        batch.draws[i].vertexCount = 0;
        batch.draws[i].vertexAlignment = 0;
        //batch.draws[i].vaoId = 0;
        //batch.draws[i].shaderId = 0;
        batch.draws[i].textureDescriptorSet = RLVK.State.defaultTexture.descriptorSet;
        //batch.draws[i].RLVK.State.projection = rlMatrixIdentity();
        //batch.draws[i].RLVK.State.modelview = rlMatrixIdentity();
    }

    batch.bufferCount = numBuffers;    // Record buffer count
    batch.drawCounter = 1;             // Reset draws counter
    batch.currentDepth = -1.0f;        // Reset depth value
    //--------------------------------------------------------------------------------------------

    return batch;
}

void rlUnloadRenderBatch(rlRenderBatch batch)     // Unload render batch system 
{
    vmaDestroyBuffer(RLVK.allocator, batch.quadIndexBuffer, batch.quadIndexAllocation);

    // Unload all vertex buffers data
    for (int i = 0; i < batch.bufferCount; i++)
    {
        rlVertexBuffer *vb = &batch.vertexBuffer[i];
        // Delete VBOs from GPU (VRAM)
        vmaDestroyBuffer(RLVK.allocator, vb->stagingBuffers[0], vb->stagingAllocations[0]);
        vmaDestroyBuffer(RLVK.allocator, vb->deviceBuffers[0], vb->deviceAllocations[0]);
        vmaDestroyBuffer(RLVK.allocator, vb->stagingBuffers[1], vb->stagingAllocations[1]);
        vmaDestroyBuffer(RLVK.allocator, vb->deviceBuffers[1], vb->deviceAllocations[1]);
        vmaDestroyBuffer(RLVK.allocator, vb->stagingBuffers[2], vb->stagingAllocations[2]);
        vmaDestroyBuffer(RLVK.allocator, vb->deviceBuffers[2], vb->deviceAllocations[2]);
        vmaDestroyBuffer(RLVK.allocator, vb->stagingBuffers[3], vb->stagingAllocations[3]);
        vmaDestroyBuffer(RLVK.allocator, vb->deviceBuffers[3], vb->deviceAllocations[3]);

        // Free vertex arrays memory from CPU (RAM)
        RL_FREE(batch.vertexBuffer[i].vertices);
        RL_FREE(batch.vertexBuffer[i].texcoords);
        RL_FREE(batch.vertexBuffer[i].normals);
        RL_FREE(batch.vertexBuffer[i].colors);
    }

    // Unload arrays
    RL_FREE(batch.vertexBuffer);
    RL_FREE(batch.draws);
}

void rlDrawRenderBatch(rlRenderBatch *batch)      // Draw render batch data (Update->Draw->Reset) 
{
    const rlVertexBuffer *vb = &batch->vertexBuffer[batch->currentBuffer];
        
    //------------------------------------------------------------------------------------------------------------

    // Draw batch vertex buffers (considering VR stereo if required)
    //------------------------------------------------------------------------------------------------------------
    Matrix matProjection = RLVK.State.projection;
    Matrix matModelView = RLVK.State.modelview;

    // Since Vulkan has a flipped vertical axis from OpenGL, flip it
    Matrix flipVertical = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    int eyeCount = 1;
    if (RLVK.State.stereoRender) eyeCount = 2;
    
    for (int eye = 0; eye < eyeCount; eye++)
    {
        if (eyeCount == 2)
        {
            // Setup current eye viewport (half screen width)
            rlViewport(eye*RLVK.State.framebufferWidth/2, 0, RLVK.State.framebufferWidth/2, RLVK.State.framebufferHeight);

            // Set current eye view offset to modelview matrix
            rlSetMatrixModelview(rlMatrixMultiply(matModelView, RLVK.State.viewOffsetStereo[eye]));
            // Set current eye projection matrix
            rlSetMatrixProjection(RLVK.State.projectionStereo[eye]);
        }

        // Draw buffers
        if (RLVK.State.vertexCounter > 0)
        {
            const rlShaderProgram *program = RLVK.State.currentShaderProgram;

            // TODO: Add location support
            // Set current shader and upload current MVP matrix
            vkCmdBindPipeline(RLVK.renderCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, program->pipeline);

            // Create modelview-projection matrix and upload to shader
            Matrix matMVP = rlMatrixMultiply(RLVK.State.modelview, rlMatrixMultiply(RLVK.State.projection, flipVertical));
            char *buf = RLVK.mappedUniformBuffers[0];
            buf += 5*4*4*sizeof(float)*RLVK.State.batchCounter;
            
            // matrix mvp
            memcpy(buf, rlMatrixToFloat(matMVP), 4*4*sizeof(float));

            // matrix projection
            buf += 4*4*sizeof(float);
            memcpy(buf, rlMatrixToFloat(RLVK.State.projection), 4*4*sizeof(float));

            // matrix view
            buf += 4*4*sizeof(float);
            memcpy(buf, rlMatrixToFloat(RLVK.State.modelview), 4*4*sizeof(float));

            // matrix model
            buf += 4*4*sizeof(float);
            memcpy(buf, rlMatrixToFloat(RLVK.State.transform), 4*4*sizeof(float));

            // matrix normal
            Matrix normal = rlMatrixTranspose(rlMatrixInvert(RLVK.State.transform));
            buf += 4*4*sizeof(float);
            memcpy(buf, rlMatrixToFloat(normal), 4*4*sizeof(float));
            
            uint32_t descriptorOffsets[] = {
                5*4*4*sizeof(float)*RLVK.State.batchCounter
            };

            vkCmdBindDescriptorSets(RLVK.renderCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, program->pipelineLayout, 0, 1, &RLVK.bufferDescriptorSet, RL_UNIFORM_BUFFER_COUNT, descriptorOffsets);

            // Bind shader program custom uniform buffer descriptors (if it has any)
            if (program->uniformCount > 0)
            {
                char *buf = program->mappedUniformBuffer;
                buf += program->descriptorSize*RLVK.State.batchCounter;
                memcpy(buf, program->uniformBufferData, program->descriptorSize);

                uint32_t descriptorOffsets2[] = {
                    program->descriptorSize*RLVK.State.batchCounter
                };
                
                vkCmdBindDescriptorSets(RLVK.renderCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, program->pipelineLayout, 2, 1, &program->descriptorSet, 1, descriptorOffsets2);
            }

            VkDeviceSize offsets[4] = { 0 };

            // Bind all vertex buffers (all in one command!)
            vkCmdBindVertexBuffers(RLVK.renderCommandBuffer, 0, 4, vb->deviceBuffers, offsets);
            vkCmdBindIndexBuffer(RLVK.renderCommandBuffer, batch->quadIndexBuffer, 0, VK_INDEX_TYPE_UINT16);

            // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batch->vertexBuffer[batch->currentBuffer].vboId[4]);

            // // Setup some default shader values
            // glUniform4f(RLVK.State.currentShaderLocs[RL_SHADER_LOC_COLOR_DIFFUSE], 1.0f, 1.0f, 1.0f, 1.0f);
            // glUniform1i(RLVK.State.currentShaderLocs[RL_SHADER_LOC_MAP_DIFFUSE], 0);  // Active default sampler2D: texture0

            // // Activate additional sampler textures
            // // Those additional textures will be common for all draw calls of the batch
            // for (int i = 0; i < RL_DEFAULT_BATCH_MAX_TEXTURE_UNITS; i++)
            // {
            //     if (RLVK.State.activeTextureId[i] > 0)
            //     {
            //         glActiveTexture(GL_TEXTURE0 + 1 + i);
            //         glBindTexture(GL_TEXTURE_2D, RLVK.State.activeTextureId[i]);
            //     }
            // }

            // // Activate default sampler2D texture0 (one texture is always active for default batch shader)
            // // NOTE: Batch system accumulates calls by texture0 changes, additional textures are enabled for all the draw calls
            // glActiveTexture(GL_TEXTURE0);

            for (int i = 0, vertexOffset = vb->vertexOffset; i < batch->drawCounter; i++)
            {
                // Bind current draw call texture, activated as GL_TEXTURE0 and bound to sampler2D texture0 by default
                vkCmdBindDescriptorSets(RLVK.renderCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, program->pipelineLayout, 1, 1, &batch->draws[i].textureDescriptorSet, 0, NULL);

                // TODO: Use indexed draw

                if ((batch->draws[i].mode == RL_LINES) || (batch->draws[i].mode == RL_TRIANGLES))
                {
                    if (batch->draws[i].mode == RL_LINES) vkCmdSetPrimitiveTopology(RLVK.renderCommandBuffer, VK_PRIMITIVE_TOPOLOGY_LINE_LIST);
                    vkCmdDraw(RLVK.renderCommandBuffer, batch->draws[i].vertexCount, 1, vertexOffset, 0);
                    if (batch->draws[i].mode == RL_LINES) vkCmdSetPrimitiveTopology(RLVK.renderCommandBuffer, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
                }
                else
                {
                    // The number of indices to be processed needs to be defined: elementCount*6
                    // NOTE: The final parameter tells the GPU the offset in bytes from the
                    // start of the index buffer to the location of the first index to process
                    vkCmdDrawIndexed(RLVK.renderCommandBuffer, batch->draws[i].vertexCount/4*6, 1, 0, vertexOffset, 0);
                }

                vertexOffset += (batch->draws[i].vertexCount + batch->draws[i].vertexAlignment);
            }
        }
    }

    // Restore viewport to default measures
    if (eyeCount == 2) rlViewport(0, 0, RLVK.State.framebufferWidth, RLVK.State.framebufferHeight);
    //------------------------------------------------------------------------------------------------------------

    // Reset batch buffers
    //------------------------------------------------------------------------------------------------------------
    // Set the current vertex buffer's vertex offset such that when a new batch starts drawing, it writes to the correct location in the vertex buffer.
    batch->vertexBuffer[batch->currentBuffer].vertexOffset += RLVK.State.vertexCounter;

    // Reset vertex counter for next batch
    RLVK.State.vertexCounter = 0;

    // Update batch counter
    RLVK.State.batchCounter++;

    // Reset depth for next draw
    batch->currentDepth = -1.0f;

    // Restore projection/modelview matrices
    RLVK.State.projection = matProjection;
    RLVK.State.modelview = matModelView;

    // Reset RLVK.currentBatch->draws array
    for (int i = 0; i < RL_DEFAULT_BATCH_DRAWCALLS; i++)
    {
        batch->draws[i].mode = RL_QUADS;
        batch->draws[i].vertexCount = 0;
        batch->draws[i].textureDescriptorSet = RLVK.State.defaultTexture.descriptorSet;
    }

    // Reset active texture units for next batch
    for (int i = 0; i < RL_DEFAULT_BATCH_MAX_TEXTURE_UNITS; i++) RLVK.State.activeTextureId[i] = 0;

    // Reset draws counter to one draw for the batch
    batch->drawCounter = 1;
    //------------------------------------------------------------------------------------------------------------

    // Change to next buffer in the list (in case of multi-buffering)
    batch->currentBuffer++;
    if (batch->currentBuffer >= batch->bufferCount) batch->currentBuffer = 0;
}

void rlSetRenderBatchActive(rlRenderBatch *batch)  // Set the active render batch for rlvk (NULL for default internal) 
{
    rlDrawRenderBatch(RLVK.currentBatch);
    rlUpdateBatchBuffers(RLVK.currentBatch);

    if (batch != NULL) RLVK.currentBatch = batch;
    else RLVK.currentBatch = &RLVK.defaultBatch;
}

void rlDrawRenderBatchActive(void)                // Update and draw internal render batch 
{
    rlDrawRenderBatch(RLVK.currentBatch);
}

bool rlCheckRenderBatchLimit(int vCount)          // Check internal buffer overflow for a given number of vertex 
{
	bool overflow = false;

    if ((RLVK.State.vertexCounter + vCount) >=
        (RLVK.currentBatch->vertexBuffer[RLVK.currentBatch->currentBuffer].elementCount*4))
    {
        overflow = true;

        // Store current primitive drawing mode and texture id
        int currentMode = RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].mode;
        VkDescriptorSet currentTextureDescriptorSet = RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].textureDescriptorSet;

        rlDrawRenderBatch(RLVK.currentBatch);    // NOTE: Stereo rendering is checked inside

        // Restore state of last batch so we can continue adding vertices
        RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].mode = currentMode;
        RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].textureDescriptorSet = currentTextureDescriptorSet;
    }

    return overflow;
}

void rlSetTexture(const rlTexture *texture)                // Set current texture for render batch and check buffers limits 
{
    if (texture == NULL)
    {
        // NOTE: If quads batch limit is reached, force a draw call and next batch starts
        if (RLVK.State.vertexCounter >=
            RLVK.currentBatch->vertexBuffer[RLVK.currentBatch->currentBuffer].elementCount*4)
        {
            rlDrawRenderBatch(RLVK.currentBatch);
        }
        RLVK.State.currentTextureDescriptorSet = RLVK.State.defaultTexture.descriptorSet;
    }
    else
    {
        RLVK.State.currentTextureDescriptorSet = texture->descriptorSet;
        if (RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].textureDescriptorSet != texture->descriptorSet)
        {
            if (RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].vertexCount > 0)
            {
                // Make sure current RLVK.currentBatch->draws[i].vertexCount is aligned a multiple of 4,
                // that way, following QUADS drawing will keep aligned with index processing
                // It implies adding some extra alignment vertex at the end of the draw,
                // those vertex are not processed but they are considered as an additional offset
                // for the next set of vertex to be drawn
                if (RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].mode == RL_LINES) RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].vertexAlignment = ((RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].vertexCount < 4)? RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].vertexCount : RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].vertexCount%4);
                else if (RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].mode == RL_TRIANGLES) RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].vertexAlignment = ((RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].vertexCount < 4)? 1 : (4 - (RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].vertexCount%4)));
                else RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].vertexAlignment = 0;

                if (!rlCheckRenderBatchLimit(RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].vertexAlignment))
                {
                    RLVK.State.vertexCounter += RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].vertexAlignment;

                    RLVK.currentBatch->drawCounter++;

                    RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].mode = RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 2].mode;

                }
            }

            if (RLVK.currentBatch->drawCounter >= RL_DEFAULT_BATCH_DRAWCALLS) rlDrawRenderBatch(RLVK.currentBatch);

            RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].textureDescriptorSet = texture->descriptorSet;
            RLVK.currentBatch->draws[RLVK.currentBatch->drawCounter - 1].vertexCount = 0;
        }
    }
}

unsigned int rlLoadVertexArray(void)              // Load vertex array (vao) if supported 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlLoadVertexArray was called.");
	return 0;
}

unsigned int rlLoadVertexBuffer(const void *buffer, int size, bool dynamic)  // Load a vertex buffer object 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlLoadVertexBuffer was called.");
	return 0;
}

unsigned int rlLoadVertexBufferElement(const void *buffer, int size, bool dynamic)  // Load vertex buffer elements object 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlLoadVertexBufferElement was called.");
	return 0;
}

void rlUpdateVertexBuffer(unsigned int bufferId, const void *data, int dataSize, int offset)  // Update vertex buffer object data on GPU buffer 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlUpdateVertexBuffer was called.");
}

void rlUpdateVertexBufferElements(unsigned int id, const void *data, int dataSize, int offset)  // Update vertex buffer elements data on GPU buffer 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlUpdateVertexBufferElements was called.");
}

void rlUnloadVertexArray(unsigned int vaoId)      // Unload vertex array (vao) 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlUnloadVertexArray was called.");
}

void rlUnloadVertexBuffer(unsigned int vboId)     // Unload vertex buffer object 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlUnloadVertexBuffer was called.");
}

void rlSetVertexAttribute(unsigned int index, int compSize, int type, bool normalized, int stride, int offset)  // Set vertex attribute data configuration 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlSetVertexAttribute was called.");
}

void rlSetVertexAttributeDivisor(unsigned int index, int divisor)  // Set vertex attribute data divisor 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlSetVertexAttributeDivisor was called.");
}

void rlSetVertexAttributeDefault(int locIndex, const void *value, int attribType, int count)  // Set vertex attribute default value, when attribute to provided 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlSetVertexAttributeDefault was called.");
}

void rlDrawVertexArray(int offset, int count)     // Draw vertex array (currently active vao) 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlDrawVertexArray was called.");
}

void rlDrawVertexArrayElements(int offset, int count, const void *buffer)  // Draw vertex array elements 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlDrawVertexArrayElements was called.");
}

void rlDrawVertexArrayInstanced(int offset, int count, int instances)  // Draw vertex array (currently active vao) with instancing 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlDrawVertexArrayInstanced was called.");
}

void rlDrawVertexArrayElementsInstanced(int offset, int count, const void *buffer, int instances)  // Draw vertex array elements with instancing 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlDrawVertexArrayElementsInstanced was called.");
}

static uint32_t rlGetPixelSize(rlPixelFormat format)
{
    switch (format)
    {
        default: return 0;
        case RL_PIXELFORMAT_UNCOMPRESSED_GRAYSCALE:
            return 1;
        case RL_PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA:
        case RL_PIXELFORMAT_UNCOMPRESSED_R5G6B5:
        case RL_PIXELFORMAT_UNCOMPRESSED_R5G5B5A1:
        case RL_PIXELFORMAT_UNCOMPRESSED_R4G4B4A4:
        case RL_PIXELFORMAT_UNCOMPRESSED_R16:
            return 2;
        case RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8:
            return 3;
        case RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8:
        case RL_PIXELFORMAT_UNCOMPRESSED_R32:
            return 4;
        case RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32:
            return 4*3;
        case RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32A32:
            return 4*4;
    }
}

// TODO: Implement fully
static VkFormat rlGetVulkanFormat(rlPixelFormat format)
{
    switch (format)
    {
        default: return VK_FORMAT_UNDEFINED;
        case RL_PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA: return VK_FORMAT_R8G8_UNORM;
        case RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8: return VK_FORMAT_R8G8B8_UNORM;
        case RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8: return VK_FORMAT_R8G8B8A8_UNORM;
    }
}

rlTexture rlLoadTexture(const void *data, int width, int height, int format, int mipmapCount)  // Load texture data 
{
    rlTexture texture = { 0 };

    if (format >= RL_PIXELFORMAT_COMPRESSED_DXT1_RGB) {
        TRACELOG(RL_LOG_WARNING, "RLVK: Unsupported texture format");
        return texture;
    }

    VkBuffer stagingBuffer;
    VmaAllocation stagingAllocation;

    VkDeviceSize imageSize = rlGetPixelSize(format)*width*height;
    VkFormat vulkanFormat = rlGetVulkanFormat(format);

    VkBufferCreateInfo stagingBufferCreateInfo =
    {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        .size = imageSize
    };
    
    if (vmaCreateBuffer(RLVK.allocator, &stagingBufferCreateInfo, &sharedWriteAllocationCreateInfo, &stagingBuffer, &stagingAllocation, NULL) != VK_SUCCESS)
    {
        TRACELOG(RL_LOG_WARNING, "Vma: Failed to create staging buffer");
        return texture;
    }
    
    VkImageCreateInfo imageCreateInfo =
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .arrayLayers = 1,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        .format = vulkanFormat,
        .extent = { width, height, 1 },
        .mipLevels = mipmapCount
    };
    
    if (vmaCreateImage(RLVK.allocator, &imageCreateInfo, &deviceAllocationCreateInfo, &texture.image, &texture.allocation, NULL) != VK_SUCCESS)
    {
        TRACELOG(RL_LOG_WARNING, "Vma: Failed to create image");
        return texture;
    }

    // copy image data over
    void* mappedData;
    if (vmaMapMemory(RLVK.allocator, stagingAllocation, &mappedData) != VK_SUCCESS)
    {
        TRACELOG(RL_LOG_WARNING, "Vma: Failed to map buffer");
        return texture;
    }
    memcpy(mappedData, data, imageSize);
    vmaUnmapMemory(RLVK.allocator, stagingAllocation);

    VkCommandBufferBeginInfo beginInfo =
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };

    if (vkBeginCommandBuffer(RLVK.transferCommandBuffer, &beginInfo) != VK_SUCCESS)
    {
        TRACELOG(LOG_WARNING, "Vulkan: Failed to begin recording command buffer");
        return texture;
    }

    VkImageMemoryBarrier barrier =
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .subresourceRange.baseMipLevel = 0,
        .subresourceRange.baseArrayLayer = 0,
        .subresourceRange.layerCount = 1,

        .image = texture.image,
        .subresourceRange.levelCount = mipmapCount,
        .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        .dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT
    };

    vkCmdPipelineBarrier(RLVK.transferCommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &barrier);

    VkBufferImageCopy copy =
    {
        .bufferOffset = 0,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .imageSubresource.mipLevel = 0,
        .imageSubresource.baseArrayLayer = 0,
        .imageSubresource.layerCount = 1,
        .imageOffset = { 0, 0, 0 },
        .imageExtent.depth = 1,
        .imageExtent.width = width,
        .imageExtent.height = height
    };
    
    vkCmdCopyBufferToImage(RLVK.transferCommandBuffer, stagingBuffer, texture.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);

    int32_t mipWidth = (int32_t)width;
    int32_t mipHeight = (int32_t)height;

    barrier.subresourceRange.levelCount = 1;

    VkImageBlit blit =
    {
        .srcOffsets[0] = { 0, 0, 0 },
        .srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .srcSubresource.mipLevel = 0,
        .srcSubresource.baseArrayLayer = 0,
        .srcSubresource.layerCount = 1,
        .dstOffsets[0] = { 0, 0, 0 },
        .dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .dstSubresource.baseArrayLayer = 0,
        .dstSubresource.layerCount = 1
    };

    for (uint32_t i = 1; i < mipmapCount; i++) {
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.subresourceRange.baseMipLevel = i - 1;
        
        vkCmdPipelineBarrier(RLVK.transferCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, NULL, 0, NULL, 1, &barrier);

        blit.srcOffsets[1] = (VkOffset3D){ mipWidth, mipHeight, 1 };
        blit.srcSubresource.mipLevel = i - 1;
        blit.dstOffsets[1] = (VkOffset3D){ mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
        blit.dstSubresource.mipLevel = i;

        vkCmdBlitImage(RLVK.transferCommandBuffer, texture.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, texture.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);
        
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(RLVK.transferCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &barrier);

        if (mipWidth > 1) { mipWidth /= 2; }
        if (mipHeight > 1) { mipHeight /= 2; }
    }

    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    barrier.subresourceRange.baseMipLevel = mipmapCount - 1;

    vkCmdPipelineBarrier(RLVK.transferCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, NULL, 0, NULL, 1, &barrier);

    if (vkEndCommandBuffer(RLVK.transferCommandBuffer) != VK_SUCCESS)
    {
        TRACELOG(LOG_WARNING, "Vulkan: Failed to record command buffer");
        return texture;
    }

    VkSubmitInfo submitInfo =
    {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &RLVK.transferCommandBuffer
    };

    if (vkQueueSubmit(RLVK.graphicsQueue, 1, &submitInfo, RLVK.transferFence) != VK_SUCCESS)
    {
        TRACELOG(LOG_WARNING, "Vulkan: Failed to submit transfer command buffer");
        return texture;
    }

    vkWaitForFences(RLVK.device, 1, &RLVK.transferFence, VK_TRUE, UINT64_MAX);
    vkResetFences(RLVK.device, 1, &RLVK.transferFence);
    vkResetCommandBuffer(RLVK.transferCommandBuffer, 0);
    
    vmaDestroyBuffer(RLVK.allocator, stagingBuffer, stagingAllocation);
    
    // create image view
    VkImageViewCreateInfo viewInfo =
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        // We need to swizzle alpha channel to green channel when using RL_PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA
        .components.r = VK_COMPONENT_SWIZZLE_IDENTITY,
        .components.g = (format == RL_PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA ? VK_COMPONENT_SWIZZLE_ONE : VK_COMPONENT_SWIZZLE_IDENTITY),
        .components.b = (format == RL_PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA ? VK_COMPONENT_SWIZZLE_ONE : VK_COMPONENT_SWIZZLE_IDENTITY),
        .components.a = (format == RL_PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA ? VK_COMPONENT_SWIZZLE_G : VK_COMPONENT_SWIZZLE_IDENTITY),
        // We need to swizzle alpha channel to green channel when using RL_PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA
        .components.a = (format == RL_PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA ? VK_COMPONENT_SWIZZLE_G : VK_COMPONENT_SWIZZLE_IDENTITY),
        .subresourceRange.baseMipLevel = 0,
        .subresourceRange.levelCount = mipmapCount,
        .subresourceRange.baseArrayLayer = 0,
        .subresourceRange.layerCount = 1,
        .subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        .image = texture.image,
        .format = vulkanFormat
    };

    if (vkCreateImageView(RLVK.device, &viewInfo, NULL, &texture.view) != VK_SUCCESS)
    {
        TRACELOG(LOG_WARNING, "Vulkan: Failed to create image view");
        return texture;
    }

    // create sampler
    VkSamplerCreateInfo samplerInfo =
    {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
        .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
        .unnormalizedCoordinates = VK_FALSE,
        .compareEnable = VK_FALSE,
        .compareOp = VK_COMPARE_OP_ALWAYS,
        .mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
        .minLod = 0.0f,
        .maxLod = (float)mipmapCount,
        .mipLodBias = 0.0f,
        .maxAnisotropy = RLVK.physicalDeviceProperties.limits.maxSamplerAnisotropy,
        .minFilter = VK_FILTER_NEAREST,
        .magFilter = VK_FILTER_NEAREST,
        .anisotropyEnable = VK_FALSE
    };

    if (vkCreateSampler(RLVK.device, &samplerInfo, NULL, &texture.sampler) != VK_SUCCESS)
    {
        TRACELOG(LOG_WARNING, "Vulkan: Failed to create sampler");
        return texture;
    }

    // allocate descriptor set
    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo =
    {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = RLVK.descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &RLVK.textureDescriptorSetLayout
    };

    if (vkAllocateDescriptorSets(RLVK.device, &descriptorSetAllocateInfo, &texture.descriptorSet) != VK_SUCCESS)
    {
        TRACELOG(RL_LOG_WARNING, "Vma: Failed to create descriptor set");
        return texture;
    }

    VkDescriptorImageInfo imageDescriptorInfo =
    {
        .sampler = texture.sampler,
        .imageView = texture.view,
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    };

    VkWriteDescriptorSet write =
    {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = texture.descriptorSet,
        .dstBinding = 0,
        .dstArrayElement = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = 1,
        .pImageInfo = &imageDescriptorInfo
    };

    vkUpdateDescriptorSets(RLVK.device, 1, &write, 0, NULL);

    return texture;
}

rlTexture rlLoadTextureDepth(int width, int height, bool useRenderBuffer)  // Load depth texture/renderbuffer (to be attached to fbo) 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlLoadTextureDepth was called.");
	return (rlTexture){ 0 };
}

rlTexture rlLoadTextureCubemap(const void *data, int size, int format, int mipmapCount)  // Load texture cubemap data 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlLoadTextureCubemap was called.");
	return (rlTexture){ 0 };
}

void rlUpdateTexture(const rlTexture* texture, int offsetX, int offsetY, int width, int height, int format, const void *data)  // Update texture with new data on GPU 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlUpdateTexture was called.");
}

void rlGetGlTextureFormats(int format, unsigned int *glInternalFormat, unsigned int *glFormat, unsigned int *glType)  // Get OpenGL internal formats 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlGetGlTextureFormats was called.");
}

const char *rlGetPixelFormatName(unsigned int format)               // Get name string for pixel format 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlGetPixelFormatName was called.");
	
    switch (format)
    {
        case RL_PIXELFORMAT_UNCOMPRESSED_GRAYSCALE: return "GRAYSCALE"; break;         // 8 bit per pixel (no alpha)
        case RL_PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA: return "GRAY_ALPHA"; break;       // 8*2 bpp (2 channels)
        case RL_PIXELFORMAT_UNCOMPRESSED_R5G6B5: return "R5G6B5"; break;               // 16 bpp
        case RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8: return "R8G8B8"; break;               // 24 bpp
        case RL_PIXELFORMAT_UNCOMPRESSED_R5G5B5A1: return "R5G5B5A1"; break;           // 16 bpp (1 bit alpha)
        case RL_PIXELFORMAT_UNCOMPRESSED_R4G4B4A4: return "R4G4B4A4"; break;           // 16 bpp (4 bit alpha)
        case RL_PIXELFORMAT_UNCOMPRESSED_R8G8B8A8: return "R8G8B8A8"; break;           // 32 bpp
        case RL_PIXELFORMAT_UNCOMPRESSED_R32: return "R32"; break;                     // 32 bpp (1 channel - float)
        case RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32: return "R32G32B32"; break;         // 32*3 bpp (3 channels - float)
        case RL_PIXELFORMAT_UNCOMPRESSED_R32G32B32A32: return "R32G32B32A32"; break;   // 32*4 bpp (4 channels - float)
        case RL_PIXELFORMAT_COMPRESSED_DXT1_RGB: return "DXT1_RGB"; break;             // 4 bpp (no alpha)
        case RL_PIXELFORMAT_COMPRESSED_DXT1_RGBA: return "DXT1_RGBA"; break;           // 4 bpp (1 bit alpha)
        case RL_PIXELFORMAT_COMPRESSED_DXT3_RGBA: return "DXT3_RGBA"; break;           // 8 bpp
        case RL_PIXELFORMAT_COMPRESSED_DXT5_RGBA: return "DXT5_RGBA"; break;           // 8 bpp
        case RL_PIXELFORMAT_COMPRESSED_ETC1_RGB: return "ETC1_RGB"; break;             // 4 bpp
        case RL_PIXELFORMAT_COMPRESSED_ETC2_RGB: return "ETC2_RGB"; break;             // 4 bpp
        case RL_PIXELFORMAT_COMPRESSED_ETC2_EAC_RGBA: return "ETC2_RGBA"; break;       // 8 bpp
        case RL_PIXELFORMAT_COMPRESSED_PVRT_RGB: return "PVRT_RGB"; break;             // 4 bpp
        case RL_PIXELFORMAT_COMPRESSED_PVRT_RGBA: return "PVRT_RGBA"; break;           // 4 bpp
        case RL_PIXELFORMAT_COMPRESSED_ASTC_4x4_RGBA: return "ASTC_4x4_RGBA"; break;   // 8 bpp
        case RL_PIXELFORMAT_COMPRESSED_ASTC_8x8_RGBA: return "ASTC_8x8_RGBA"; break;   // 2 bpp
        default: return "UNKNOWN"; break;
    }
}

void rlUnloadTexture(const rlTexture *texture)                               // Unload texture from GPU memory 
{
    vkDestroySampler(RLVK.device, texture->sampler, NULL);
    vkDestroyImageView(RLVK.device, texture->view, NULL);
    vmaDestroyImage(RLVK.allocator, texture->image, texture->allocation);
}

void rlGenTextureMipmaps(const rlTexture* texture, int width, int height, int format, int *mipmaps)  // Generate mipmap data for selected texture 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlGenTextureMipmaps was called.");
}

void *rlReadTexturePixels(const rlTexture* texture, int width, int height, int format)  // Read texture pixel data 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlReadTexturePixels was called.");
	return NULL;
}

unsigned char *rlReadScreenPixels(int width, int height)            // Read screen pixel data (color buffer) 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlReadScreenPixels was called.");
	return NULL;
}

unsigned int rlLoadFramebuffer(void)                                // Load an empty framebuffer 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlLoadFramebuffer was called.");
	return 0;
}

void rlFramebufferAttach(unsigned int id, const rlTexture *texture, int attachType, int texType, int mipLevel)  // Attach texture/renderbuffer to a framebuffer 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlFramebufferAttach was called.");
}

bool rlFramebufferComplete(unsigned int id)                         // Verify framebuffer is complete 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlFramebufferComplete was called.");
	return false;
}

void rlUnloadFramebuffer(unsigned int id)                           // Delete framebuffer from GPU 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlUnloadFramebuffer was called.");
}

void rlCopyFramebuffer(int x, int y, int width, int height, int format, void *pixels)  // Copy framebuffer pixel data to internal buffer 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlCopyFramebuffer was called.");
}

void rlResizeFramebuffer(int width, int height)                     // Resize internal framebuffer 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlResizeFramebuffer was called.");
}

VkShaderModule rlLoadShader(const char *code, int type)                     // Load (compile) shader and return shader id (type: RL_VERTEX_SHADER, RL_FRAGMENT_SHADER, RL_COMPUTE_SHADER)
{
    shaderc_compile_options_t shaderCompileOptions = shaderc_compile_options_initialize();

    shaderc_compilation_result_t compileResult =
        shaderc_compile_into_spv(RLVK.shaderCompiler, code, strlen(code), type, 
        "rlvkShader", "main", shaderCompileOptions);

    if (shaderc_result_get_compilation_status(compileResult) != shaderc_compilation_status_success)
    {
        TRACELOG(LOG_WARNING, "RLVK: Failed to compile shader: %s", shaderc_result_get_error_message(compileResult));
        return NULL;
    }

    size_t vsSpirVLength = shaderc_result_get_length(compileResult);
    uint32_t *vsSpirvVBinary = (uint32_t *)shaderc_result_get_bytes(compileResult);

    VkShaderModuleCreateInfo shaderCreateInfo =
    {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = vsSpirVLength,
        .pCode = vsSpirvVBinary
    };

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(RLVK.device, &shaderCreateInfo, 0, &shaderModule) != VK_SUCCESS)
    {
        TRACELOG(LOG_WARNING, "Vulkan: Failed to create shader module");
        return NULL;
    }

    shaderc_result_release(compileResult);

    shaderc_compile_options_release(shaderCompileOptions);

    return shaderModule;
}

rlShaderProgram rlLoadShaderProgram(const char *vsCode, const char *fsCode)  // Load shader from code strings 
{   
    return rlLoadShaderProgramExA(vsCode, fsCode, 0, NULL);
}

rlShaderProgram rlLoadShaderProgramExA(const char *vsCode, const char *fsCode, uint32_t uniformCount, const rlShaderUniform *uniforms)  // Load shader from code strings 
{   
    rlShaderProgram program = { 0 };

    VkShaderModule vsModule;
    if (vsCode == NULL)
    {
        vsModule = RLVK.State.defaultVShaderModule;
    } else if ((vsModule = rlLoadShader(vsCode, RL_VERTEX_SHADER)) == VK_NULL_HANDLE) {
        return program;
    }


    VkShaderModule fsModule;
    if (fsCode == NULL) {
        fsModule = RLVK.State.defaultFShaderModule;
    } else if ((fsModule = rlLoadShader(fsCode, RL_FRAGMENT_SHADER)) == VK_NULL_HANDLE) {
        return program;
    }
    if (fsModule == VK_NULL_HANDLE) {
        return program;
    }

    program = rlLoadShaderProgramPro(vsModule, fsModule, uniformCount, uniforms);

    if (vsCode != NULL)
    {
        rlUnloadShader(vsModule);
    }
    if (fsCode != NULL)
    {
        rlUnloadShader(fsModule);
    }

    return program;
}

rlShaderProgram rlLoadShaderProgramEx(VkShaderModule vsModule, VkShaderModule fsModule)  // Load shader program, using already loaded shader modules 
{
    return rlLoadShaderProgramPro(vsModule, fsModule, 0, NULL);
}


static uint32_t rlAlign(uint32_t n, uint32_t m) // Returns the smallest multiple of $m$ greater than or equal to $n$.
{
    // $n = mq + r$
    uint32_t q = n/m;
    uint32_t r = n%m;

    if (r == 0)
    {
        return n;
    }
    else
    {
        return m*(q + 1);
    }
}

rlShaderProgram rlLoadShaderProgramPro(VkShaderModule vsModule, VkShaderModule fsModule, uint32_t uniformCount, const rlShaderUniform *uniforms)  // Load shader program, using already loaded shader modules 
{
    rlShaderProgram program = { 0 };
    program.uniformCount = uniformCount;

    // Create shader-specific descriptor sets and layouts
    if (uniformCount > 0)
    {
        // calculate size of uniform buffer
        program.structNames = (char **)RL_CALLOC(uniformCount, sizeof(char *));
        program.structOffsets = (uint32_t *)RL_CALLOC(uniformCount, sizeof(uint32_t *));

        for (uint32_t i = 0; i < uniformCount; i++)
        {
            char *name = (char *)RL_CALLOC(strlen(uniforms[i].name) + 1, sizeof(char));
            strcpy(name, uniforms[i].name);

            program.structNames[i] = name;
        }

        for (uint32_t i = 0; i < uniformCount; i++) {
            program.structOffsets[i] = program.descriptorSize;
            program.descriptorSize += uniforms[i].size;
        }
        program.descriptorSize = rlAlign(program.descriptorSize, 64/* RLVK.physicalDeviceProperties.limits.minUniformBufferOffsetAlignment */);

        program.uniformBufferData = RL_MALLOC(program.descriptorSize);

        // create descriptor set

        VkDescriptorSetLayoutBinding descriptorSetBinding =
        {
            .descriptorCount = 1,
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
            .stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS
        };

        VkDescriptorPoolSize poolSize = {
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
            .descriptorCount = 1
        };

        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .poolSizeCount = 1,
            .pPoolSizes = &poolSize,
            .maxSets = 1
        };

        if (vkCreateDescriptorPool(RLVK.device, &descriptorPoolCreateInfo, NULL, &program.descriptorPool) != VK_SUCCESS)
        {
            TRACELOG(LOG_WARNING, "Vulkan: Failed to create descriptor pool");
            return program;
        }

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = 1,
            .pBindings = &descriptorSetBinding
        };

        if (vkCreateDescriptorSetLayout(RLVK.device, &descriptorSetLayoutCreateInfo, 0, &program.descriptorSetLayout) != VK_SUCCESS)
        {
            TRACELOG(LOG_WARNING, "Vulkan: Failed to create descriptor set layout");
            return program;
        }

        VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = program.descriptorPool,
            .descriptorSetCount = 1,
            .pSetLayouts = &program.descriptorSetLayout
        };

        if (vkAllocateDescriptorSets(RLVK.device, &descriptorSetAllocateInfo, &program.descriptorSet) != VK_SUCCESS) {
            TRACELOG(RL_LOG_WARNING, "Vma: Failed to create descriptor sets");
            return program;
        }

        VkBufferCreateInfo uniformBufferCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            .size = RL_DEFAULT_BATCH_COUNT_PER_FRAME*program.descriptorSize
        };

        if (vmaCreateBuffer(RLVK.allocator, &uniformBufferCreateInfo, &sharedWriteAllocationCreateInfo, &program.uniformBuffer, &program.uniformAllocation, NULL) != VK_SUCCESS)
        {
            TRACELOG(RL_LOG_WARNING, "Vma: Failed to create uniform buffer");
            return program;
        }
        vmaMapMemory(RLVK.allocator, program.uniformAllocation, &program.mappedUniformBuffer);

        VkDescriptorBufferInfo uniformBufferInfo = {
            .buffer = program.uniformBuffer,
            .range = program.descriptorSize
        };

        VkWriteDescriptorSet uniformWrite = {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = program.descriptorSet,
            .dstArrayElement = 0,
            .dstBinding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
            .descriptorCount = 1,
            .pBufferInfo = &uniformBufferInfo
        };

        vkUpdateDescriptorSets(RLVK.device, 1, &uniformWrite, 0, NULL);

        VkDescriptorSetLayout layouts[3] = { RLVK.bufferDescriptorSetLayout, RLVK.textureDescriptorSetLayout, program.descriptorSetLayout };

        VkPipelineLayoutCreateInfo pipelineLayoutInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount = 3,
            .pSetLayouts = layouts
        };

        if (vkCreatePipelineLayout(RLVK.device, &pipelineLayoutInfo, 0, &program.pipelineLayout) != VK_SUCCESS)
        {
            TRACELOG(LOG_WARNING, "Vulkan: Failed to create pipeline layout");
            return program;
        }
    } else
    {
        VkDescriptorSetLayout layouts[2] = { RLVK.bufferDescriptorSetLayout, RLVK.textureDescriptorSetLayout };

        VkPipelineLayoutCreateInfo pipelineLayoutInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount = 2,
            .pSetLayouts = layouts
        };

        if (vkCreatePipelineLayout(RLVK.device, &pipelineLayoutInfo, 0, &program.pipelineLayout) != VK_SUCCESS)
        {
            TRACELOG(LOG_WARNING, "Vulkan: Failed to create pipeline layout");
            return program;
        }
    }

    // Create graphics pipeline
    VkPipelineShaderStageCreateInfo vertexShaderStageInfo =
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = vsModule,
        .pName = "main"
    };

    VkPipelineShaderStageCreateInfo fragmentShaderStageInfo =
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = fsModule,
        .pName = "main"
    };

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertexShaderStageInfo, fragmentShaderStageInfo };

    VkDynamicState dynamicStates[] =
    {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
        VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY,
        VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE,
        VK_DYNAMIC_STATE_DEPTH_WRITE_ENABLE,
        VK_DYNAMIC_STATE_CULL_MODE,
        VK_DYNAMIC_STATE_FRONT_FACE
    };

    VkPipelineDynamicStateCreateInfo dynamicState =
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = sizeof(dynamicStates) / sizeof(dynamicStates[0]),
        .pDynamicStates = dynamicStates
    };

    VkVertexInputBindingDescription vertexBindings[4] = {
        {
            .binding = 0,
            .stride = 3*sizeof(float),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
        },
        {
            .binding = 1,
            .stride = 2*sizeof(float),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
        },
        {
            .binding = 2,
            .stride = 3*sizeof(float),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
        },
        {
            .binding = 3,
            .stride = 4*sizeof(uint8_t),
            .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
        },
    };
    
    VkVertexInputAttributeDescription vertexAttributes[4] = {
        {
            .binding = 0,
            .location = 0,
            .format = VK_FORMAT_R32G32B32_SFLOAT
        },
        {
            .binding = 1,
            .location = 1,
            .format = VK_FORMAT_R32G32_SFLOAT
        },
        {
            .binding = 2,
            .location = 2,
            .format = VK_FORMAT_R32G32B32_SFLOAT
        },
        {
            .binding = 3,
            .location = 3,
            .format = VK_FORMAT_R8G8B8A8_UNORM,
            .offset = 0
        }
    };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo =
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = 4,
        .pVertexBindingDescriptions = vertexBindings,
        .vertexAttributeDescriptionCount = 4,
        .pVertexAttributeDescriptions = vertexAttributes
    };

    VkPipelineInputAssemblyStateCreateInfo inputAssembly =
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE
    };

    VkPipelineViewportStateCreateInfo viewportState =
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .pViewports = &RLVK.State.viewport,
        .scissorCount = 1,
        .pScissors = &RLVK.State.scissor
    };

    VkPipelineRasterizationStateCreateInfo rasterizer =
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .lineWidth = 1.0f,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.0f, //Optional
        .depthBiasClamp = 0.0f, //Optional
        .depthBiasSlopeFactor = 0.0f //Optional
    };

    VkPipelineMultisampleStateCreateInfo multisampling =
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .sampleShadingEnable = VK_FALSE,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .minSampleShading = 1.0f, //Optional
        .pSampleMask = 0, //Optional
        .alphaToCoverageEnable = VK_FALSE, //Optional
        .alphaToOneEnable = VK_FALSE //Optional
    };
    
    VkPipelineColorBlendAttachmentState colorBlendAttachment =
    {
        .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        .blendEnable = VK_TRUE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD
    };

    VkPipelineColorBlendStateCreateInfo colorBlending =
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY, //Optional
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachment,
        .blendConstants[0] = 0.0f, //Optional
        .blendConstants[1] = 0.0f, //Optional
        .blendConstants[2] = 0.0f, //Optional
        .blendConstants[3] = 0.0f, //Optional
    };

    VkPipelineDepthStencilStateCreateInfo depthStencil =
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = VK_FALSE,
        .depthWriteEnable = VK_FALSE,
        .depthCompareOp = VK_COMPARE_OP_LESS,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable = VK_FALSE
    };

    VkGraphicsPipelineCreateInfo pipelineInfo =
    {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = 2,
        .pStages = shaderStages,
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &inputAssembly,
        .pViewportState = &viewportState,
        .pRasterizationState = &rasterizer,
        .pMultisampleState = &multisampling,
        .pDepthStencilState = &depthStencil, //Optional
        .pColorBlendState = &colorBlending,
        .pDynamicState = &dynamicState,
        .layout = program.pipelineLayout,
        .renderPass = RLVK.renderPass,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE, //Optional
        .basePipelineIndex = -1 //Optional
    };

    if (vkCreateGraphicsPipelines(RLVK.device, VK_NULL_HANDLE, 1, &pipelineInfo, 0, &program.pipeline) != VK_SUCCESS)
    {
        TRACELOG(LOG_WARNING, "Vulkan: Failed to create graphics pipeline");
        return program;
    }

    return program;
}

unsigned int rlLoadShaderProgramCompute(unsigned int csId)                // Load compute shader program 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlLoadShaderProgramCompute was called.");
	return 0;
}

void rlUnloadShader(VkShaderModule shaderModule)                                      // Unload shader, loaded with rlLoadShader() 
{
    vkDestroyShaderModule(RLVK.device, shaderModule, NULL);
}

void rlUnloadShaderProgram(const rlShaderProgram *program)                               // Unload shader program 
{
    for (uint32_t i = 0; i < program->uniformCount; i++)
    {
        RL_FREE(program->structNames[i]);
    }
    if (program->uniformCount > 0)
    {
        vmaUnmapMemory(RLVK.allocator, program->uniformAllocation);
        vmaDestroyBuffer(RLVK.allocator, program->uniformBuffer, program->uniformAllocation);
    }

    RL_FREE(program->uniformBufferData);
    RL_FREE(program->structOffsets);
    RL_FREE(program->structNames);
    
    vkDestroyPipeline(RLVK.device, program->pipeline, NULL);
    vkDestroyPipelineLayout(RLVK.device, program->pipelineLayout, 0);
    vkDestroyDescriptorPool(RLVK.device, program->descriptorPool, NULL);
    vkDestroyDescriptorSetLayout(RLVK.device, program->descriptorSetLayout, NULL);
}

int rlGetLocationUniform(const rlShaderProgram *program, const char *uniformName)        // Get shader location uniform, requires shader program id 
{
    for (uint32_t i = 0; i < program->uniformCount; i++)
    {
        if (strcmp(uniformName, program->structNames[i]) == 0)
        {
            return i;
        }
    }
    return -1;
}

int rlGetLocationAttrib(const rlShaderProgram *program, const char *attribName)          // Get shader location attribute, requires shader program id 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlGetLocationAttrib was called.");
	return 0;
}

static uint32_t rlGetUniformValueSize(int uniformType)
{
    switch (uniformType)
    {
        case RL_SHADER_UNIFORM_FLOAT: return sizeof(float);
        case RL_SHADER_UNIFORM_VEC2: return 2*sizeof(float);
        case RL_SHADER_UNIFORM_VEC3: return 3*sizeof(float);
        case RL_SHADER_UNIFORM_VEC4: return 4*sizeof(float);
        case RL_SHADER_UNIFORM_INT: return sizeof(int32_t);
        case RL_SHADER_UNIFORM_IVEC2: return 2*sizeof(int32_t);
        case RL_SHADER_UNIFORM_IVEC3: return 3*sizeof(int32_t);
        case RL_SHADER_UNIFORM_IVEC4: return 4*sizeof(int32_t);
        case RL_SHADER_UNIFORM_UINT: return sizeof(int32_t);
        case RL_SHADER_UNIFORM_UIVEC2: return 2*sizeof(int32_t);
        case RL_SHADER_UNIFORM_UIVEC3: return 3*sizeof(int32_t);
        case RL_SHADER_UNIFORM_UIVEC4: return 4*sizeof(int32_t);
        default: return 0;
    }
}

void rlSetUniform(int locIndex, const void *value, int uniformType, int count)  // Set shader value uniform 
{
    // Essentially just does a write into a struct. TODO: Eventually just offer a struct to write into
    char *buf = RLVK.State.descriptorManagingCurrentBoundShaderProgram->uniformBufferData;
    buf += RLVK.State.descriptorManagingCurrentBoundShaderProgram->structOffsets[locIndex];
    memcpy(buf, value, rlGetUniformValueSize(uniformType)*count);
}

void rlSetUniformMatrix(int locIndex, Matrix mat)                         // Set shader value matrix 
{
    rlSetUniform(locIndex, rlMatrixToFloat(mat), RL_SHADER_UNIFORM_VEC4, 4);
}

void rlSetUniformMatrices(int locIndex, const Matrix *matrices, int count)     // Set shader value matrices 
{
    // TODO: Make sure this syntax is actually right.

    float (*mat)[4*4] = (float (*)[4*4])RL_CALLOC(count, 4*4*sizeof(float));

    for (int i = 0; i < count; i++)
    {
        memcpy(mat[i], rlMatrixToFloat(matrices[i]), 4*4*sizeof(float));
    }

    rlSetUniform(locIndex, mat, RL_SHADER_UNIFORM_VEC4, 4*count);

    RL_FREE(mat);
}

void rlSetUniformSampler(int locIndex, const rlTexture *texture)            // Set shader value sampler 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlSetUniformSampler was called.");
}

void rlSetShader(const rlShaderProgram *program)                              // Set shader currently active (id and locations) 
{
    if (program == NULL)
    {
        program = &RLVK.State.defaultShaderProgram;
    }
    if (RLVK.State.currentShaderProgram->pipeline != program->pipeline)
    {
        rlDrawRenderBatch(RLVK.currentBatch);

        RLVK.State.currentShaderProgram = program;
    }
}

void rlComputeShaderDispatch(unsigned int groupX, unsigned int groupY, unsigned int groupZ)  // Dispatch compute shader (equivalent to *draw* for graphics pipeline) 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlComputeShaderDispatch was called.");
}

unsigned int rlLoadShaderBuffer(unsigned int size, const void *data, int usageHint)  // Load shader storage buffer object (SSBO) 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlLoadShaderBuffer was called.");
	return 0;
}

void rlUnloadShaderBuffer(unsigned int ssboId)                            // Unload shader storage buffer object (SSBO) 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlUnloadShaderBuffer was called.");
}

void rlUpdateShaderBuffer(unsigned int id, const void *data, unsigned int dataSize, unsigned int offset)  // Update SSBO buffer data 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlUpdateShaderBuffer was called.");
}

void rlBindShaderBuffer(unsigned int id, unsigned int index)              // Bind SSBO buffer 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlBindShaderBuffer was called.");
}

void rlReadShaderBuffer(unsigned int id, void *dest, unsigned int count, unsigned int offset)  // Read SSBO buffer data (GPU->CPU) 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlReadShaderBuffer was called.");
}

void rlCopyShaderBuffer(unsigned int destId, unsigned int srcId, unsigned int destOffset, unsigned int srcOffset, unsigned int count)  // Copy SSBO data between buffers 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlCopyShaderBuffer was called.");
}

unsigned int rlGetShaderBufferSize(unsigned int id)                       // Get SSBO buffer size 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlGetShaderBufferSize was called.");
	return 0;
}

void rlBindImageTexture(const rlTexture* texture, unsigned int index, int format, bool readonly)   // Bind image texture 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlBindImageTexture was called.");
}

// Matrix state management
//-----------------------------------------------------------------------------------------
// Get internal modelview matrix
Matrix rlGetMatrixModelview(void)
{
    Matrix matrix = rlMatrixIdentity();
    matrix = RLVK.State.modelview;
    return matrix;
}

// Get internal projection matrix
Matrix rlGetMatrixProjection(void)
{
    return RLVK.State.projection;
}

// Get internal accumulated transform matrix
Matrix rlGetMatrixTransform(void)
{
    Matrix mat = rlMatrixIdentity();
    // TODO: Consider possible transform matrices in the RLVK.State.stack
    //Matrix matStackTransform = rlMatrixIdentity();
    //for (int i = RLVK.State.stackCounter; i > 0; i--) matStackTransform = rlMatrixMultiply(RLVK.State.stack[i], matStackTransform);

    mat = RLVK.State.transform;
    return mat;
}

// Get internal projection matrix for stereo render (selected eye)
Matrix rlGetMatrixProjectionStereo(int eye)
{
    Matrix mat = rlMatrixIdentity();
    mat = RLVK.State.projectionStereo[eye];
    return mat;
}

// Get internal view offset matrix for stereo render (selected eye)
Matrix rlGetMatrixViewOffsetStereo(int eye)
{
    Matrix mat = rlMatrixIdentity();
    mat = RLVK.State.viewOffsetStereo[eye];
    return mat;
}

// Set a custom modelview matrix (replaces internal modelview matrix)
void rlSetMatrixModelview(Matrix view)
{
    RLVK.State.modelview = view;
}

// Set a custom projection matrix (replaces internal projection matrix)
void rlSetMatrixProjection(Matrix projection)
{
    RLVK.State.projection = projection;
}

// Set eyes projection matrices for stereo rendering
void rlSetMatrixProjectionStereo(Matrix right, Matrix left)
{
    RLVK.State.projectionStereo[0] = right;
    RLVK.State.projectionStereo[1] = left;
}

// Set eyes view offsets matrices for stereo rendering
void rlSetMatrixViewOffsetStereo(Matrix right, Matrix left)
{
    RLVK.State.viewOffsetStereo[0] = right;
    RLVK.State.viewOffsetStereo[1] = left;
}

void rlLoadDrawCube(void)      // Load and draw a cube 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlLoadDrawCube was called.");
}

void rlLoadDrawQuad(void)      // Load and draw a qua 
{
    TRACELOG(RL_LOG_TRACE, "rlvk function rlLoadDrawQuad was called.");
}

void rlWaitIdle(void) {
    vkDeviceWaitIdle(RLVK.device);
}

#endif // RLVK_IMPLEMENTATION
