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

typedef enum LvnAttachmentLoadOp
{
    Lvn_AttachmentLoadOp_Load,
    Lvn_AttachmentLoadOp_Clear,
    Lvn_AttachmentLoadOp_DontCare,
} LvnAttachmentLoadOp;

typedef enum LvnAttachmentStoreOp
{
    Lvn_AttachmentStoreOp_Store,
    Lvn_AttachmentStoreOp_DontCare,
} LvnAttachmentStoreOp;

typedef enum LvnTopologyType
{
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

typedef enum LvnColorComponentFlagBits
{
    Lvn_ColorComponentFlag_R = 0x00000001,
    Lvn_ColorComponentFlag_G = 0x00000002,
    Lvn_ColorComponentFlag_B = 0x00000004,
    Lvn_ColorComponentFlag_A = 0x00000008,
} LvnColorComponentFlagBits;
typedef LvnFlags LvnColorComponentFlags;

typedef enum LvnColorBlendFactor
{
    Lvn_ColorBlendFactor_Zero,
    Lvn_ColorBlendFactor_One,
    Lvn_ColorBlendFactor_SrcColor,
    Lvn_ColorBlendFactor_OneMinusSrcColor,
    Lvn_ColorBlendFactor_DstColor,
    Lvn_ColorBlendFactor_OneMinusDstColor,
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

typedef enum LvnShaderStage
{
    Lvn_ShaderStage_Vertex,
    Lvn_ShaderStage_Fragment,
} LvnShaderStage;

typedef enum LvnFormat
{
    Lvn_Format_None = 0,
    Lvn_Format_R8G8B8_UNORM,
    Lvn_Format_R8G8B8_SRGB,
    Lvn_Format_R8G8B8A8_UNORM,
    Lvn_Format_R8G8B8A8_SRGB,

    Lvn_Format_B8G8R8_SRGB,
    Lvn_Format_B8G8R8A8_SRGB,
} LvnFormat;

typedef enum LvnPresentMode
{
    Lvn_PresentMode_FIFO,
    Lvn_PresentMode_Mailbox,
    Lvn_PresentMode_Immediate,
} LvnPresentMode;

typedef struct LvnGraphicsContext LvnGraphicsContext;
typedef struct LvnSurface LvnSurface;
typedef struct LvnDescriptorLayout LvnDescriptorLayout;
typedef struct LvnShader LvnShader;
typedef struct LvnPipeline LvnPipeline;
typedef struct LvnImageView LvnImageView;
typedef struct LvnCommandBuffer LvnCommandBuffer;
typedef struct LvnFence LvnFence;
typedef struct LvnSemaphore LvnSemaphore;

struct LvnContext;


typedef struct LvnPlatformData
{
    void*    nativeDisplayHandle;
    void*    nativeWindowHandle;
} LvnPlatformData;

typedef struct LvnSurfaceCreateInfo
{
    void*             nativeDisplayHandle;
    void*             nativeWindowHandle;
    uint32_t          width;
    uint32_t          height;
    uint32_t          minImageCount;
    LvnFormat         surfaceFormat;
    LvnPresentMode    presentMode;
} LvnSurfaceCreateInfo;

typedef struct LvnShaderCreateInfo
{
    const uint8_t*    pCode;
    size_t            codeSize;
} LvnShaderCreateInfo;

typedef struct LvnPipelineInputAssembly
{
    LvnTopologyType    topology;
    bool               primitiveRestartEnable;
} LvnPipelineInputAssembly;

typedef struct LvnPipelineViewport
{
    float x, y;
    float width, height;
    float minDepth, maxDepth;
} LvnPipelineViewport;

typedef struct LvnPipelineScissor
{
    struct { uint32_t x, y; }             offset;
    struct { uint32_t width, height; }    extent;
} LvnPipelineScissor;

typedef struct LvnPipelineRasterizer
{
    LvnCullFaceMode     cullMode;
    LvnCullFrontFace    frontFace;
    float               lineWidth;
    float               depthBiasConstantFactor;
    float               depthBiasClamp;
    float               depthBiasSlopeFactor;
    bool                depthClampEnable;
    bool                rasterizerDiscardEnable;
    bool                depthBiasEnable;
} LvnPipelineRasterizer;

typedef struct LvnPipelineMultiSampling
{
    LvnSampleCountFlagBits    rasterizationSamples;
    float                     minSampleShading;
    uint32_t*                 sampleMask;
    bool                      sampleShadingEnable;
    bool                      alphaToCoverageEnable;
    bool                      alphaToOneEnable;
} LvnPipelineMultiSampling;

typedef struct LvnPipelineColorBlendAttachment
{
    LvnColorComponentFlags    colorWriteMask;
    LvnColorBlendFactor       srcColorBlendFactor;
    LvnColorBlendFactor       dstColorBlendFactor;
    LvnColorBlendOperation    colorBlendOp;
    LvnColorBlendFactor       srcAlphaBlendFactor;
    LvnColorBlendFactor       dstAlphaBlendFactor;
    LvnColorBlendOperation    alphaBlendOp;
    bool                      blendEnable;
} LvnPipelineColorBlendAttachment;

typedef struct LvnPipelineColorBlend
{
    LvnPipelineColorBlendAttachment*    pColorBlendAttachments;
    uint32_t                            colorBlendAttachmentCount;
    float                               blendConstants[4];
    bool                                logicOpEnable;
} LvnPipelineColorBlend;

typedef struct LvnPipelineStencilAttachment
{
    LvnStencilOperation    failOp;
    LvnStencilOperation    passOp;
    LvnStencilOperation    depthFailOp;
    LvnCompareOperation    compareOp;
    uint32_t               compareMask;
    uint32_t               writeMask;
    uint32_t               reference;
} LvnPipelineStencilAttachment;

typedef struct LvnPipelineDepthStencil
{
    LvnCompareOperation             depthOpCompare;
    LvnPipelineStencilAttachment    stencil;
    bool                            enableDepth, enableStencil;
} LvnPipelineDepthStencil;

typedef struct LvnPipelineFixedFunctions
{
    LvnPipelineInputAssembly    inputAssembly;
    LvnPipelineViewport         viewport;
    LvnPipelineScissor          scissor;
    LvnPipelineRasterizer       rasterizer;
    LvnPipelineMultiSampling    multisampling;
    LvnPipelineColorBlend       colorBlend;
    LvnPipelineDepthStencil     depthstencil;
} LvnPipelineFixedFunctions;

typedef struct LvnVertexBindingDescription
{
    uint32_t binding, stride;
} LvnVertexBindingDescription;

typedef struct LvnVertexAttribute
{
    uint32_t              binding;
    uint32_t              layout;
    LvnAttributeFormat    format;
    uint64_t              offset;
} LvnVertexAttribute;

typedef struct LvnPipelineShaderStageCreateInfo
{
    LvnShaderStage      stage;
    const LvnShader*    shader;
    const char*         entryPoint;
} LvnPipelineShaderStageCreateInfo;

typedef struct LvnPipelineCreateInfo
{
    const LvnPipelineFixedFunctions*           pipelineFixedFunctions;
    const LvnVertexBindingDescription*         pVertexBindingDescriptions;
    uint32_t                                   vertexBindingDescriptionCount;
    const LvnVertexAttribute*                  pVertexAttributes;
    uint32_t                                   vertexAttributeCount;
    const LvnDescriptorLayout* const*          pDescriptorLayouts;
    uint32_t                                   descriptorLayoutCount;
    const LvnPipelineShaderStageCreateInfo*    pStages;
    uint32_t                                   stageCount;
    const LvnFormat*                           pColorAttachmentFormats;
    uint32_t                                   colorAttachmentCount;
    LvnFormat                                  depthAttachmentFormat;
    LvnFormat                                  stencilAttachmentFormat;
} LvnPipelineCreateInfo;

typedef struct LvnCommandBufferCreateInfo
{
    int temp; // TODO: remove this
} LvnCommandBufferCreateInfo;

typedef union LvnClearColorValue
{
    float       float32[4];
    int32_t     int32[4];
    uint32_t    uint32[4];
} LvnClearColorValue;

typedef struct LvnClearDepthStencilValue
{
    float       depth;
    uint32_t    stencil;
} LvnClearDepthStencilValue;

typedef union LvnClearValue
{
    LvnClearColorValue           color;
    LvnClearDepthStencilValue    depthStencil;
} LvnClearValue;

typedef struct LvnRenderingAttachmentInfo
{
    LvnAttachmentLoadOp     loadOp;
    LvnAttachmentStoreOp    storeOp;
    LvnImageView*           imageView;
    LvnClearValue           clearValue;
} LvnRenderingAttachmentInfo;

typedef struct LvnExtent2D
{
    uint32_t width, height;
} LvnExtent2D;

typedef struct LvnOffset2D
{
    int32_t x, y;
} LvnOffset2D;

typedef struct LvnRenderArea
{
    LvnExtent2D    extent;
    LvnOffset2D    offset;
} LvnRenderArea;

typedef struct LvnViewport
{
    float x, y, width, height, minDepth, maxDepth;
} LvnViewport;

typedef struct LvnRenderingInfo
{
    LvnRenderArea                        renderArea;
    uint32_t                             colorAttachmentCount;
    const LvnRenderingAttachmentInfo*    pColorAttachments;
    const LvnRenderingAttachmentInfo*    depthAttachment;
} LvnRenderingInfo;

typedef struct LvnSubmitInfo
{
    uint32_t                    waitSemaphoreCount;
    LvnSemaphore* const*        pWaitSemaphores;
    uint32_t                    signalSemaphoreCount;
    LvnSemaphore* const*        pSignalSemaphores;
    uint32_t                    commandBufferCount;
    LvnCommandBuffer* const*    pCommandBuffers;
} LvnSubmitInfo;

typedef struct LvnPresentInfo
{
    uint32_t                waitSemaphoreCount;
    LvnSemaphore* const*    pWaitSemaphores;
    uint32_t                surfaceCount;
    LvnSurface* const*      pSurfaces;
    const uint32_t*         pImageIndices;
} LvnPresentInfo;

typedef struct LvnGraphicsContextCreateInfo
{
    LvnGraphicsApi              graphicsapi;                                    // graphics api backend
    LvnPresentationModeFlags    presentationModeFlags;                          // type of output the graphics api will render to
    const LvnPlatformData*      platformData;                                   // native platform data for surface creation
    bool                        enableGraphicsApiDebugLogging;                  // enable logging for graphics api layer debug logs
} LvnGraphicsContextCreateInfo;


#ifdef __cplusplus
extern "C" {
#endif

LVN_API LvnResult                   lvnCreateGraphicsContext(struct LvnContext* ctx, LvnGraphicsContext** graphicsctx, const LvnGraphicsContextCreateInfo* createInfo); // create the graphics context
LVN_API void                        lvnDestroyGraphicsContext(LvnGraphicsContext* graphicsctx);                                                                         // destroy the graphics context

LVN_API LvnResult                   lvnCreateSurface(const LvnGraphicsContext* graphicsctx, LvnSurface** surface, const LvnSurfaceCreateInfo* createInfo);
LVN_API void                        lvnDestroySurface(LvnSurface* surface);
LVN_API LvnResult                   lvnCreateShader(const LvnGraphicsContext* graphicsctx, LvnShader** shader, const LvnShaderCreateInfo* createInfo);
LVN_API void                        lvnDestroyShader(LvnShader* shader);
LVN_API LvnResult                   lvnCreatePipeline(const LvnGraphicsContext* graphicsctx, LvnPipeline** pipeline, const LvnPipelineCreateInfo* createInfo);
LVN_API void                        lvnDestroyPipeline(LvnPipeline* pipeline);
LVN_API LvnResult                   lvnCreateCommandBuffer(const LvnGraphicsContext* graphicsctx, LvnCommandBuffer** commandBuffer, const LvnCommandBufferCreateInfo* createInfo);
LVN_API void                        lvnDestroyCommandBuffer(LvnCommandBuffer* commandBuffer);
LVN_API LvnResult                   lvnCreateFence(const LvnGraphicsContext* graphicsctx, LvnFence** fence);
LVN_API void                        lvnDestroyFence(LvnFence* fence);
LVN_API LvnResult                   lvnCreateSemaphore(const LvnGraphicsContext* graphicsctx, LvnSemaphore** semaphore);
LVN_API void                        lvnDestroySemaphore(LvnSemaphore* semaphore);

LVN_API LvnFormat                   lvnSurfaceGetSwapchainFormat(const LvnSurface* surface);
LVN_API LvnImageView*               lvnSurfaceGetSwapchainImageView(LvnSurface* surface, uint32_t imageIndex);
LVN_API LvnExtent2D                 lvnSurfaceGetExtent(const LvnSurface* surface);
LVN_API LvnResult                   lvnSurfaceResize(LvnSurface* surface, uint32_t width, uint32_t height);
LVN_API LvnPipelineFixedFunctions   lvnConfigPipelineFixedFunctionsInit(void);
LVN_API LvnResult                   lvnFenceWait(LvnFence* fence, uint64_t timeout);
LVN_API LvnResult                   lvnFenceReset(LvnFence* fence);

LVN_API void                        lvnBeginCommandBuffer(LvnCommandBuffer* commandBuffer);
LVN_API void                        lvnEndCommandBuffer(LvnCommandBuffer* commandBuffer);
LVN_API void                        lvnCmdBeginRendering(LvnCommandBuffer* commandBuffer, const LvnRenderingInfo* renderInfo);
LVN_API void                        lvnCmdEndRendering(LvnCommandBuffer* commandBuffer);
LVN_API void                        lvnCmdBindPipeline(LvnCommandBuffer* commandBuffer, LvnPipeline* pipeline);
LVN_API void                        lvnCmdSetViewport(LvnCommandBuffer* commandBuffer, const LvnViewport* viewport);
LVN_API void                        lvnCmdSetScissor(LvnCommandBuffer* commandBuffer, const LvnRenderArea* scissor);
LVN_API void                        lvnCmdDraw(LvnCommandBuffer* commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);
LVN_API void                        lvnCmdDrawIndexed(LvnCommandBuffer* commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance);
LVN_API LvnResult                   lvnSurfaceAcquireNextImage(LvnSurface* surface, LvnSemaphore* semaphore, LvnFence* fence, uint32_t* imageIndex);
LVN_API LvnResult                   lvnRenderSubmit(const LvnGraphicsContext* graphicsctx, const LvnSubmitInfo* pSubmits, uint32_t submitCount, LvnFence* fence);
LVN_API LvnResult                   lvnRenderPresent(const LvnGraphicsContext* graphicsctx, const LvnPresentInfo* presentInfo);

#ifdef __cplusplus
}
#endif


#endif // !HG_LVN_GRAPHICS_H
