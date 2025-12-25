#ifndef HG_LVN_GRAPHICS_H
#define HG_LVN_GRAPHICS_H

#include "lvn_config.h"


typedef enum LvnGraphicsApi
{
    Lvn_GraphicsApi_None = 0,
    Lvn_GraphicsApi_Opengl,
    Lvn_GraphicsApi_Vulkan,
} LvnGraphicsApi;

typedef enum LvnPresentationModeFlagBits
{
    Lvn_PresentationModeFlag_Headless = 0x00000001,
    Lvn_PresentationModeFlag_Surface  = 0x00000002,
} LvnPresentationModeFlagBits;
typedef LvnFlags LvnPresentationModeFlags;

typedef enum LvnTopologyType
{
    Lvn_TopologyType_None = 0,
    Lvn_TopologyType_Point,
    Lvn_TopologyType_Line,
    Lvn_TopologyType_LineStrip,
    Lvn_TopologyType_Triangle,
    Lvn_TopologyType_TriangleStrip,
} LvnTopologyType;

typedef enum LvnCullFaceMode
{
    Lvn_CullFaceMode_Front,
    Lvn_CullFaceMode_Back,
    Lvn_CullFaceMode_Both,
    Lvn_CullFaceMode_Disable,
} LvnCullFaceMode;

typedef enum LvnCullFrontFace
{
    Lvn_CullFrontFace_Clockwise,
    Lvn_CullFrontFace_CounterClockwise,

    Lvn_CullFrontFace_CW = Lvn_CullFrontFace_Clockwise,
    Lvn_CullFrontFace_CCW = Lvn_CullFrontFace_CounterClockwise,
} LvnCullFrontFace;

typedef enum LvnSampleCountFlagBits
{
    Lvn_SampleCountFlag_1_Bit  = 0x00000001,
    Lvn_SampleCountFlag_2_Bit  = 0x00000002,
    Lvn_SampleCountFlag_4_Bit  = 0x00000004,
    Lvn_SampleCountFlag_8_Bit  = 0x00000008,
    Lvn_SampleCountFlag_16_Bit = 0x00000010,
    Lvn_SampleCountFlag_32_Bit = 0x00000020,
    Lvn_SampleCountFlag_64_Bit = 0x00000040,
} LvnSampleCountFlagBits;
typedef LvnFlags LvnSampleCountFlags;

typedef enum LvnColorBlendFactor
{
    Lvn_ColorBlendFactor_Zero,
    Lvn_ColorBlendFactor_One,
    Lvn_ColorBlendFactor_Src,
    Lvn_ColorBlendFactor_OneMinusSrcColor,
    Lvn_ColorBlendFactor_DstColor,
    Lvn_ColorBlendFactor_OneMinus,
    Lvn_ColorBlendFactor_SrcAlpha,
    Lvn_ColorBlendFactor_OneMinusSrcAlpha,
    Lvn_ColorBlendFactor_DstAlpha,
    Lvn_ColorBlendFactor_OneMinusDstAlpha,
    Lvn_ColorBlendFactor_ConstantColor,
    Lvn_ColorBlendFactor_OneMinusConstantColor,
    Lvn_ColorBlendFactor_ConstantAlpha,
    Lvn_ColorBlendFactor_OneMinusConstantAlpha,
    Lvn_ColorBlendFactor_SrcAlphaSaturate,
    Lvn_ColorBlendFactor_Src1Color,
    Lvn_ColorBlendFactor_OneMinusSrc1Color,
    Lvn_ColorBlendFactor_Src1_Alpha,
    Lvn_ColorBlendFactor_OneMinusSrc1Alpha,
} LvnColorBlendFactor;

typedef enum LvnColorBlendOperation
{
    Lvn_ColorBlendOp_Add,
    Lvn_ColorBlendOp_Subtract,
    Lvn_ColorBlendOp_ReverseSubtract,
    Lvn_ColorBlendOp_Min,
    Lvn_ColorBlendOp_Max,
} LvnColorBlendOperation;

typedef enum LvnStencilOperation
{
    Lvn_StencilOp_Keep,
    Lvn_StencilOp_Zero,
    Lvn_StencilOp_Replace,
    Lvn_StencilOp_IncrementAndClamp,
    Lvn_StencilOp_DecrementAndClamp,
    Lvn_StencilOp_Invert,
    Lvn_StencilOp_IncrementAndWrap,
    Lvn_StencilOp_DecrementAndWrap,
} LvnStencilOperation;

typedef enum LvnCompareOperation
{
    Lvn_CompareOp_Never,
    Lvn_CompareOp_Less,
    Lvn_CompareOp_Equal,
    Lvn_CompareOp_LessOrEqual,
    Lvn_CompareOp_Greater,
    Lvn_CompareOp_NotEqual,
    Lvn_CompareOp_GreaterOrEqual,
    Lvn_CompareOp_Always,
} LvnCompareOperation;

typedef enum LvnAttributeFormat
{
    Lvn_AttributeFormat_Undefined = 0,
    Lvn_AttributeFormat_Scalar_f32,
    Lvn_AttributeFormat_Scalar_f64,
    Lvn_AttributeFormat_Scalar_i32,
    Lvn_AttributeFormat_Scalar_ui32,
    Lvn_AttributeFormat_Scalar_i8,
    Lvn_AttributeFormat_Scalar_ui8,
    Lvn_AttributeFormat_Vec2_f32,
    Lvn_AttributeFormat_Vec3_f32,
    Lvn_AttributeFormat_Vec4_f32,
    Lvn_AttributeFormat_Vec2_f64,
    Lvn_AttributeFormat_Vec3_f64,
    Lvn_AttributeFormat_Vec4_f64,
    Lvn_AttributeFormat_Vec2_i32,
    Lvn_AttributeFormat_Vec3_i32,
    Lvn_AttributeFormat_Vec4_i32,
    Lvn_AttributeFormat_Vec2_ui32,
    Lvn_AttributeFormat_Vec3_ui32,
    Lvn_AttributeFormat_Vec4_ui32,
    Lvn_AttributeFormat_Vec2_i8,
    Lvn_AttributeFormat_Vec3_i8,
    Lvn_AttributeFormat_Vec4_i8,
    Lvn_AttributeFormat_Vec2_ui8,
    Lvn_AttributeFormat_Vec3_ui8,
    Lvn_AttributeFormat_Vec4_ui8,
    Lvn_AttributeFormat_Vec2_n8,
    Lvn_AttributeFormat_Vec3_n8,
    Lvn_AttributeFormat_Vec4_n8,
    Lvn_AttributeFormat_Vec2_un8,
    Lvn_AttributeFormat_Vec3_un8,
    Lvn_AttributeFormat_Vec4_un8,
    Lvn_AttributeFormat_2_10_10_10_ile,
    Lvn_AttributeFormat_2_10_10_10_uile,
    Lvn_AttributeFormat_2_10_10_10_nle,
    Lvn_AttributeFormat_2_10_10_10_unle,
} LvnAttributeFormat;


typedef struct LvnGraphicsContext LvnGraphicsContext;
typedef struct LvnSurface LvnSurface;
typedef struct LvnDescriptorLayout LvnDescriptorLayout;
typedef struct LvnShader LvnShader;
typedef struct LvnPipeline LvnPipeline;

struct LvnContext;


typedef struct LvnPlatformData
{
    void* nativeDisplayHandle;
    void* nativeWindowHandle;
} LvnPlatformData;

typedef struct LvnSurfaceCreateInfo
{
    void* nativeDisplayHandle;
    void* nativeWindowHandle;
    uint32_t width;
    uint32_t height;
} LvnSurfaceCreateInfo;

typedef struct LvnShaderCreateInfo
{
    const uint8_t* pCode;
    size_t codeSize;
} LvnShaderCreateInfo;

typedef struct LvnPipelineInputAssembly
{
    LvnTopologyType topology;
    bool primitiveRestartEnable;
} LvnPipelineInputAssembly;

typedef struct LvnPipelineViewport
{
    float x, y;
    float width, height;
    float minDepth, maxDepth;
} LvnPipelineViewport;

typedef struct LvnPipelineScissor
{
    struct { uint32_t x, y; } offset;
    struct { uint32_t width, height; } extent;
} LvnPipelineScissor;

typedef struct LvnPipelineRasterizer
{
    LvnCullFaceMode cullMode;
    LvnCullFrontFace frontFace;

    float lineWidth;
    float depthBiasConstantFactor;
    float depthBiasClamp;
    float depthBiasSlopeFactor;

    bool depthClampEnable;
    bool rasterizerDiscardEnable;
    bool depthBiasEnable;
} LvnPipelineRasterizer;

typedef struct LvnPipelineColorWriteMask
{
    bool colorComponentR;
    bool colorComponentG;
    bool colorComponentB;
    bool colorComponentA;
} LvnPipelineColorWriteMask;

typedef struct LvnPipelineMultiSampling
{
    LvnSampleCountFlags rasterizationSamples;
    float minSampleShading;
    uint32_t* sampleMask;
    bool sampleShadingEnable;
    bool alphaToCoverageEnable;
    bool alphaToOneEnable;
} LvnPipelineMultiSampling;

typedef struct LvnPipelineColorBlendAttachment
{
    LvnPipelineColorWriteMask colorWriteMask;
    LvnColorBlendFactor srcColorBlendFactor;
    LvnColorBlendFactor dstColorBlendFactor;
    LvnColorBlendOperation colorBlendOp;
    LvnColorBlendFactor srcAlphaBlendFactor;
    LvnColorBlendFactor dstAlphaBlendFactor;
    LvnColorBlendOperation alphaBlendOp;
    bool blendEnable;
} LvnPipelineColorBlendAttachment;

typedef struct LvnPipelineColorBlend
{
    LvnPipelineColorBlendAttachment* pColorBlendAttachments;
    uint32_t colorBlendAttachmentCount;
    float blendConstants[4];
    bool logicOpEnable;
} LvnPipelineColorBlend;

typedef struct LvnPipelineStencilAttachment
{
    LvnStencilOperation failOp;
    LvnStencilOperation passOp;
    LvnStencilOperation depthFailOp;
    LvnCompareOperation compareOp;
    uint32_t compareMask;
    uint32_t writeMask;
    uint32_t reference;
} LvnPipelineStencilAttachment;

typedef struct LvnPipelineDepthStencil
{
    LvnCompareOperation depthOpCompare;
    LvnPipelineStencilAttachment stencil;
    bool enableDepth, enableStencil;
} LvnPipelineDepthStencil;

typedef struct LvnPipelineFixedFunctions
{
    LvnPipelineInputAssembly inputAssembly;
    LvnPipelineViewport viewport;
    LvnPipelineScissor scissor;
    LvnPipelineRasterizer rasterizer;
    LvnPipelineMultiSampling multisampling;
    LvnPipelineColorBlend colorBlend;
    LvnPipelineDepthStencil depthstencil;
} LvnPipelineFixedFunctions;

typedef struct LvnVertexBindingDescription
{
    uint32_t binding, stride;
} LvnVertexBindingDescription;

typedef struct LvnVertexAttribute
{
    uint32_t binding;
    uint32_t layout;
    LvnAttributeFormat format;
    uint64_t offset;
} LvnVertexAttribute;


typedef struct LvnPipelineCreateInfo
{
    const LvnPipelineFixedFunctions* pipelineFixedFunctions;
    const LvnVertexBindingDescription* pVertexBindingDescriptions;
    uint32_t vertexBindingDescriptionCount;
    const LvnVertexAttribute* pVertexAttributes;
    uint32_t vertexAttributeCount;
    const LvnDescriptorLayout* const* pDescriptorLayouts;
    uint32_t descriptorLayoutCount;
    const LvnShader* shader;
} LvnPipelineCreateInfo;

typedef struct LvnGraphicsContextCreateInfo
{
    LvnGraphicsApi graphicsapi;                          // graphics api backend
    LvnPresentationModeFlags presentationModeFlags;      // type of output the graphics api will render to
    const LvnPlatformData* platformData;                 // native platform data for surface creation
    bool enableGraphicsApiDebugLogging;                  // enable logging for graphics api layer debug logs
} LvnGraphicsContextCreateInfo;


#ifdef __cplusplus
extern "C" {
#endif

LVN_API LvnResult               lvnCreateGraphicsContext(struct LvnContext* ctx, LvnGraphicsContext** graphicsctx, const LvnGraphicsContextCreateInfo* createInfo); // create the graphics context
LVN_API void                    lvnDestroyGraphicsContext(LvnGraphicsContext* graphicsctx);                                                                         // destroy the graphics context

LVN_API LvnResult               lvnCreateSurface(const LvnGraphicsContext* graphicsctx, LvnSurface** surface, const LvnSurfaceCreateInfo* createInfo);
LVN_API void                    lvnDestroySurface(LvnSurface* surface);
LVN_API LvnResult               lvnCreateShader(const LvnGraphicsContext* graphicsctx, LvnShader** shader, const LvnShaderCreateInfo* createInfo);
LVN_API void                    lvnDestroyShader(LvnShader* shader);
LVN_API LvnResult               lvnCreatePipeline(const LvnGraphicsContext* graphicsctx, LvnPipeline** pipeline, const LvnPipelineCreateInfo* createInfo);
LVN_API void                    lvnDestroyPipeline(LvnPipeline* pipeline);

#ifdef __cplusplus
}
#endif


#endif // !HG_LVN_GRAPHICS_H
