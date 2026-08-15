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

typedef enum LvnAttachmentUsage
{
    Lvn_AttachmentUsage_ColorAttachment,
    Lvn_AttachmentUsage_ShaderReadOnly,
    Lvn_AttachmentUsage_PresentSrc,
} LvnAttachmentUsage;

typedef enum LvnTopologyType
{
    Lvn_TopologyType_Point,
    Lvn_TopologyType_Line,
    Lvn_TopologyType_LineStrip,
    Lvn_TopologyType_Triangle,
    Lvn_TopologyType_TriangleStrip,
} LvnTopologyType;

typedef enum LvnPolygonMode
{
    Lvn_PolygonMode_Fill,
    Lvn_PolygonMode_Line,
    Lvn_PolygonMode_Point,
} LvnPolygonMode;

typedef enum LvnCullFaceMode
{
    Lvn_CullFaceMode_Disable = 0,
    Lvn_CullFaceMode_Front,
    Lvn_CullFaceMode_Back,
    Lvn_CullFaceMode_Both,
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

typedef enum LvnLogicOperation
{
    Lvn_LogicOp_Clear,
    Lvn_LogicOp_And,
    Lvn_LogicOp_AndReverse,
    Lvn_LogicOp_Copy,
    Lvn_LogicOp_AndInverted,
    Lvn_LogicOp_NoOp,
    Lvn_LogicOp_Xor,
    Lvn_LogicOp_Or,
    Lvn_LogicOp_Nor,
    Lvn_LogicOp_Equivalent,
    Lvn_LogicOp_Invert,
    Lvn_LogicOp_OrReverse,
    Lvn_LogicOp_CopyInverted,
    Lvn_LogicOp_OrInverted,
    Lvn_LogicOp_Nand,
    Lvn_LogicOp_Set,
} LvnLogicOperation;

typedef enum LvnShaderStageFlagBits
{
    Lvn_ShaderStageFlag_Vertex   = 0x00000001,
    Lvn_ShaderStageFlag_Fragment = 0x00000002,
} LvnShaderStageFlagBits;
typedef LvnFlags LvnShaderStageFlags;

typedef enum LvnFormat
{
    Lvn_Format_Undefined = 0,

    Lvn_Format_R8_UNORM,
    Lvn_Format_R8_SNORM,
    Lvn_Format_R8_UINT,
    Lvn_Format_R8_SINT,

    Lvn_Format_R16_UNORM,
    Lvn_Format_R16_SNORM,
    Lvn_Format_R16_UINT,
    Lvn_Format_R16_SINT,
    Lvn_Format_R16_FLOAT,

    Lvn_Format_R32_UINT,
    Lvn_Format_R32_SINT,
    Lvn_Format_R32_FLOAT,

    Lvn_Format_R8G8_UNORM,
    Lvn_Format_R8G8_SNORM,
    Lvn_Format_R8G8_UINT,
    Lvn_Format_R8G8_SINT,

    Lvn_Format_R16G16_FLOAT,
    Lvn_Format_R32G32_FLOAT,
    Lvn_Format_R32G32_UINT,
    Lvn_Format_R32G32_SINT,

    Lvn_Format_R32G32B32_FLOAT,
    Lvn_Format_R32G32B32_UINT,
    Lvn_Format_R32G32B32_SINT,

    Lvn_Format_R8G8B8A8_UNORM,
    Lvn_Format_R8G8B8A8_SNORM,
    Lvn_Format_R8G8B8A8_UINT,
    Lvn_Format_R8G8B8A8_SINT,
    Lvn_Format_R8G8B8A8_SRGB,

    Lvn_Format_R16G16B16A16_FLOAT,

    Lvn_Format_R32G32B32A32_FLOAT,
    Lvn_Format_R32G32B32A32_UINT,
    Lvn_Format_R32G32B32A32_SINT,

    Lvn_Format_B8G8R8A8_UNORM,
    Lvn_Format_B8G8R8A8_SRGB,

    Lvn_Format_A2B10G10R10_UNORM,
    Lvn_Format_A2B10G10R10_UINT,

    Lvn_Format_D16_UNORM,
    Lvn_Format_D24_UNORM_S8_UINT,
    Lvn_Format_D32_FLOAT,
} LvnFormat;

typedef enum LvnPresentMode
{
    Lvn_PresentMode_FIFO,
    Lvn_PresentMode_Mailbox,
    Lvn_PresentMode_Immediate,
} LvnPresentMode;

typedef enum LvnBufferTypeFlagBits
{
    Lvn_BufferTypeFlag_Unknown = 0x00000000,
    Lvn_BufferTypeFlag_Vertex  = 0x00000001,
    Lvn_BufferTypeFlag_Index   = 0x00000002,
    Lvn_BufferTypeFlag_Uniform = 0x00000004,
    Lvn_BufferTypeFlag_Storage = 0x00000008,
} LvnBufferTypeFlagBits;
typedef uint32_t LvnBufferTypeFlags;

typedef enum LvnBufferMemoryUsage
{
    Lvn_BufferMemoryUsage_GpuOnly,
    Lvn_BufferMemoryUsage_CpuToGpu,
    Lvn_BufferMemoryUsage_GpuToCpu,
} LvnBufferMemoryUsage;

typedef enum LvnTextureFilter
{
    Lvn_TextureFilter_Nearest,
    Lvn_TextureFilter_Linear,
} LvnTextureFilter;

typedef enum LvnMipmapMode
{
    Lvn_MipmapMode_Disabled = 0,
    Lvn_MipmapMode_Nearest,
    Lvn_MipmapMode_Linear,
} LvnMipmapMode;

typedef enum LvnTextureMode
{
    Lvn_TextureMode_Repeat,
    Lvn_TextureMode_MirrorRepeat,
    Lvn_TextureMode_ClampToEdge,
    Lvn_TextureMode_ClampToBorder,
} LvnTextureMode;

typedef enum LvnDescriptorType
{
    Lvn_DescriptorType_Sampler,
    Lvn_DescriptorType_CombinedImageSampler,
    Lvn_DescriptorType_SampledImage,
    Lvn_DescriptorType_UniformBuffer,
    Lvn_DescriptorType_StorageBuffer,
    Lvn_DescriptorType_UniformBufferDynamic,
    Lvn_DescriptorType_StorageBufferDynamic,
} LvnDescriptorType;

typedef struct LvnGraphicsContext LvnGraphicsContext;
typedef struct LvnBuffer LvnBuffer;
typedef struct LvnSampler LvnSampler;
typedef struct LvnTexture LvnTexture;
typedef struct LvnSurface LvnSurface;
typedef struct LvnSwapchain LvnSwapchain;
typedef struct LvnRenderPass LvnRenderPass;
typedef struct LvnFramebuffer LvnFramebuffer;
typedef struct LvnDescriptorLayout LvnDescriptorLayout;
typedef struct LvnDescriptorPool LvnDescriptorPool;
typedef struct LvnDescriptorSet LvnDescriptorSet;
typedef struct LvnShader LvnShader;
typedef struct LvnPipeline LvnPipeline;
typedef struct LvnCommandBuffer LvnCommandBuffer;
typedef struct LvnFence LvnFence;
typedef struct LvnSemaphore LvnSemaphore;
typedef struct LvnResolveAttachment LvnResolveAttachment;
typedef struct LvnColorAttachment LvnColorAttachment;
typedef struct LvnDepthStencilAttachment LvnDepthStencilAttachment;
typedef struct LvnDescriptorBinding LvnDescriptorBinding;
typedef struct LvnDescriptorPoolSize LvnDescriptorPoolSize;
typedef struct LvnDescriptorBufferInfo LvnDescriptorBufferInfo;
typedef struct LvnDescriptorImageInfo LvnDescriptorImageInfo;
typedef struct LvnPipelineInputAssembly LvnPipelineInputAssembly;
typedef struct LvnPipelineRasterizer LvnPipelineRasterizer;
typedef struct LvnPipelineMultiSampling LvnPipelineMultiSampling;
typedef struct LvnPipelineColorBlendAttachment LvnPipelineColorBlendAttachment;
typedef struct LvnPipelineColorBlend LvnPipelineColorBlend;
typedef struct LvnPipelineStencilAttachment LvnPipelineStencilAttachment;
typedef struct LvnPipelineDepthStencil LvnPipelineDepthStencil;
typedef struct LvnPipelineFixedFunctions LvnPipelineFixedFunctions;
typedef struct LvnVertexBindingDescription LvnVertexBindingDescription;
typedef struct LvnVertexAttribute LvnVertexAttribute;
typedef struct LvnClearDepthStencilValue LvnClearDepthStencilValue;
typedef struct LvnExtent2D LvnExtent2D;
typedef struct LvnOffset2D LvnOffset2D;
typedef struct LvnRenderArea LvnRenderArea;
typedef struct LvnViewport LvnViewport;
typedef struct LvnSurfaceCreateInfo LvnSurfaceCreateInfo;
typedef struct LvnSwapchainCreateInfo LvnSwapchainCreateInfo;
typedef struct LvnRenderPassCreateInfo LvnRenderPassCreateInfo;
typedef struct LvnFramebufferCreateInfo LvnFramebufferCreateInfo;
typedef struct LvnShaderCreateInfo LvnShaderCreateInfo;
typedef struct LvnDescriptorLayoutCreateInfo LvnDescriptorLayoutCreateInfo;
typedef struct LvnDescriptorPoolCreateInfo LvnDescriptorPoolCreateInfo;
typedef struct LvnPipelineCreateInfo LvnPipelineCreateInfo;
typedef struct LvnBufferCreateInfo LvnBufferCreateInfo;
typedef struct LvnSamplerCreateInfo LvnSamplerCreateInfo;
typedef struct LvnTextureCreateInfo LvnTextureCreateInfo;
typedef struct LvnDescriptorSetAllocateInfo LvnDescriptorSetAllocateInfo;
typedef struct LvnDescriptorSetWriteInfo LvnDescriptorSetWriteInfo;
typedef struct LvnDescriptorSetCopyInfo LvnDescriptorSetCopyInfo;
typedef struct LvnRenderPassBeginInfo LvnRenderPassBeginInfo;
typedef struct LvnSubmitInfo LvnSubmitInfo;
typedef struct LvnPresentInfo LvnPresentInfo;
typedef struct LvnGraphicsContextFunctions LvnGraphicsContextFunctions;
typedef struct LvnGraphicsContextCreateInfo LvnGraphicsContextCreateInfo;

typedef union LvnClearColorValue LvnClearColorValue;

typedef LvnSurfaceCreateInfo LvnPlatformData;

struct LvnContext;


typedef const LvnSurface* (*PFN_lvnGetSurface)(const LvnGraphicsContext*);
typedef LvnResult (*PFN_lvnCreateSurface)(const LvnGraphicsContext*, LvnSurface*, const LvnSurfaceCreateInfo*);
typedef void      (*PFN_lvnDestroySurface)(LvnSurface*);
typedef LvnResult (*PFN_lvnCreateSwapchain)(const LvnGraphicsContext*, LvnSwapchain*, const LvnSwapchainCreateInfo*);
typedef void      (*PFN_lvnDestroySwapchain)(LvnSwapchain*);
typedef LvnResult (*PFN_lvnCreateRenderPass)(const LvnGraphicsContext*, LvnRenderPass*, const LvnRenderPassCreateInfo*);
typedef void      (*PFN_lvnDestroyRenderPass)(LvnRenderPass*);
typedef LvnResult (*PFN_lvnCreateFramebuffer)(const LvnGraphicsContext*, LvnFramebuffer*, const LvnFramebufferCreateInfo*);
typedef void      (*PFN_lvnDestroyFramebuffer)(LvnFramebuffer*);
typedef LvnResult (*PFN_lvnCreateShader)(const LvnGraphicsContext*, LvnShader*, const LvnShaderCreateInfo*);
typedef void      (*PFN_lvnDestroyShader)(LvnShader*);
typedef LvnResult (*PFN_lvnCreateDescriptorLayout)(const LvnGraphicsContext*, LvnDescriptorLayout*, const LvnDescriptorLayoutCreateInfo*);
typedef void      (*PFN_lvnDestroyDescriptorLayout)(LvnDescriptorLayout*);
typedef LvnResult (*PFN_lvnCreateDescriptorPool)(const LvnGraphicsContext*, LvnDescriptorPool*, const LvnDescriptorPoolCreateInfo*);
typedef void      (*PFN_lvnDestroyDescriptorPool)(LvnDescriptorPool*);
typedef LvnResult (*PFN_lvnCreatePipeline)(const LvnGraphicsContext*, LvnPipeline*, const LvnPipelineCreateInfo*);
typedef void      (*PFN_lvnDestroyPipeline)(LvnPipeline*);
typedef LvnResult (*PFN_lvnCreateFence)(const LvnGraphicsContext*, LvnFence*, bool);
typedef void      (*PFN_lvnDestroyFence)(LvnFence*);
typedef LvnResult (*PFN_lvnCreateSemaphore)(const LvnGraphicsContext*, LvnSemaphore*);
typedef void      (*PFN_lvnDestroySemaphore)(LvnSemaphore*);
typedef LvnResult (*PFN_lvnCreateBuffer)(const LvnGraphicsContext*, LvnBuffer*, const LvnBufferCreateInfo*);
typedef void      (*PFN_lvnDestroyBuffer)(LvnBuffer*);
typedef LvnResult (*PFN_lvnCreateSampler)(const LvnGraphicsContext*, LvnSampler*, const LvnSamplerCreateInfo*);
typedef void      (*PFN_lvnDestroySampler)(LvnSampler*);
typedef LvnResult (*PFN_lvnCreateTexture)(const LvnGraphicsContext*, LvnTexture*, const LvnTextureCreateInfo*);
typedef void      (*PFN_lvnDestroyTexture)(LvnTexture*);
typedef LvnResult (*PFN_lvnCreateCommandBuffer)(const LvnGraphicsContext*, LvnCommandBuffer*);
typedef void      (*PFN_lvnDestroyCommandBuffer)(LvnCommandBuffer*);
typedef LvnResult (*PFN_lvnAllocateDescriptorSets)(const LvnGraphicsContext*, LvnDescriptorSet**, LvnDescriptorSetAllocateInfo*);
typedef LvnResult (*PFN_lvnResetDescriptorPool)(const LvnGraphicsContext*, LvnDescriptorPool*);
typedef LvnResult (*PFN_lvnUpdateDescriptorSets)(const LvnGraphicsContext*, uint32_t, const LvnDescriptorSetWriteInfo*, uint32_t, const LvnDescriptorSetCopyInfo*);
typedef void      (*PFN_lvnSurfaceGetSupportedFormats)(const LvnSurface*, uint32_t*, LvnFormat*);
typedef void      (*PFN_lvnSurfaceGetSupportedPresentModes)(const LvnSurface*, uint32_t*, LvnPresentMode*);
typedef LvnResult (*PFN_lvnSwapchainResize)(LvnSwapchain*, uint32_t, uint32_t);
typedef LvnResult (*PFN_lvnSwapchainAcquireNextImage)(LvnSwapchain*, LvnSemaphore*, LvnFence*, uint32_t*);
typedef LvnResult (*PFN_lvnFenceWait)(LvnFence*, uint64_t);
typedef LvnResult (*PFN_lvnFenceReset)(LvnFence*);
typedef void      (*PFN_lvnBufferUpdate)(LvnBuffer*, void*, uint64_t, uint64_t);
typedef void      (*PFN_lvnBeginCommandBuffer)(LvnCommandBuffer*);
typedef void      (*PFN_lvnEndCommandBuffer)(LvnCommandBuffer*);
typedef void      (*PFN_lvnCmdBeginRenderPass)(LvnCommandBuffer*, LvnRenderPassBeginInfo*);
typedef void      (*PFN_lvnCmdEndRenderPass)(LvnCommandBuffer*);
typedef void      (*PFN_lvnCmdBindPipeline)(LvnCommandBuffer*, LvnPipeline*);
typedef void      (*PFN_lvnCmdBindVertexBuffer)(LvnCommandBuffer*, uint32_t, uint32_t, LvnBuffer**, uint64_t*);
typedef void      (*PFN_lvnCmdBindIndexBuffer)(LvnCommandBuffer*, LvnBuffer*, uint64_t);
typedef void      (*PFN_lvnCmdBindDescriptorSets)(LvnCommandBuffer*, LvnPipeline*, uint32_t, uint32_t, LvnDescriptorSet* const*, uint32_t, const uint32_t*);
typedef void      (*PFN_lvnCmdSetViewport)(LvnCommandBuffer*, const LvnViewport*);
typedef void      (*PFN_lvnCmdSetScissor)(LvnCommandBuffer*, const LvnRenderArea*);
typedef void      (*PFN_lvnCmdDraw)(LvnCommandBuffer*, uint32_t, uint32_t, uint32_t, uint32_t);
typedef void      (*PFN_lvnCmdDrawIndexed)(LvnCommandBuffer*, uint32_t, uint32_t, uint32_t, int32_t, uint32_t);
typedef LvnResult (*PFN_lvnRenderSubmit)(const LvnGraphicsContext*, const LvnSubmitInfo*, uint32_t, LvnFence*);
typedef LvnResult (*PFN_lvnRenderPresent)(const LvnGraphicsContext*, const LvnPresentInfo*);


struct LvnSurfaceCreateInfo
{
    void*    ndh;
    void*    nwh;
};

struct LvnSwapchainCreateInfo
{
    const LvnSurface*    surface;
    LvnFormat            surfaceFormat;
    LvnPresentMode       presentMode;
    uint32_t             minImageCount;
    uint32_t             width;
    uint32_t             height;
};

struct LvnResolveAttachment
{
    LvnFormat               format;
    LvnAttachmentUsage      usage;
    LvnAttachmentLoadOp     loadOp;
    LvnAttachmentStoreOp    storeOp;
};

struct LvnColorAttachment
{
    LvnFormat                format;
    LvnAttachmentUsage       usage;
    LvnSampleCountFlags      samples;
    LvnAttachmentLoadOp      loadOp;
    LvnAttachmentStoreOp     storeOp;
    LvnResolveAttachment*    resolveAttachment;
};

struct LvnDepthStencilAttachment
{
    LvnFormat               format;
    LvnSampleCountFlags     samples;
    LvnAttachmentLoadOp     loadOp;
    LvnAttachmentStoreOp    storeOp;
    LvnAttachmentLoadOp     stencilLoadOp;
    LvnAttachmentStoreOp    stencilStoreOp;
};

struct LvnRenderPassCreateInfo
{
    LvnColorAttachment*           pColorAttachments;
    uint32_t                      colorAttachmentCount;
    LvnDepthStencilAttachment*    depthStencilAttachment;
};

struct LvnFramebufferCreateInfo
{
    LvnRenderPass*        renderPass;
    uint32_t              colorAttachmentCount;
    LvnTexture* const*    pColorAttachments;
    LvnTexture* const*    pResolveAttachments;
    LvnTexture*           depthStencilAttachment;
    uint32_t              width;
    uint32_t              height;
};

struct LvnShaderCreateInfo
{
    const uint8_t*            pCode;
    size_t                    codeSize;
    LvnShaderStageFlagBits    stage;
    const char*               entryPoint;
};

struct LvnDescriptorBinding
{
    uint32_t               binding;
    LvnDescriptorType      descriptorType;
    LvnShaderStageFlags    stageFlags;
};

struct LvnDescriptorLayoutCreateInfo
{
    uint32_t                 descriptorBindingCount;
    LvnDescriptorBinding*    pDescriptorBindings;
};

struct LvnDescriptorPoolSize
{
    LvnDescriptorType    type;
    uint32_t             descriptorCount;
};

struct LvnDescriptorPoolCreateInfo
{
    uint32_t                  maxSets;
    uint32_t                  poolSizeCount;
    LvnDescriptorPoolSize*    pPoolSizes;
};

struct LvnDescriptorSetAllocateInfo
{
    LvnDescriptorPool*      descriptorPool;
    uint32_t                descriptorSetCount;
    LvnDescriptorLayout*    pDescriptorLayouts;
};

struct LvnDescriptorBufferInfo
{
    LvnBuffer*    buffer;
    uint64_t      offset;
    uint64_t      range;
};

struct LvnDescriptorImageInfo
{
    LvnSampler*    sampler;
    LvnTexture*    texture;
};

struct LvnDescriptorSetWriteInfo
{
    LvnDescriptorType                 descriptorType;
    uint32_t                          binding;
    LvnDescriptorSet*                 descriptorSet;
    const LvnDescriptorBufferInfo*    bufferInfo;
    const LvnDescriptorImageInfo*     imageInfo;
};

struct LvnDescriptorSetCopyInfo
{
    LvnDescriptorSet*                 srcDescriptorSet;
    uint32_t                          srcBinding;
    uint32_t                          srcFirstIndex;
    LvnDescriptorSet*                 dstDescriptorSet;
    uint32_t                          dstBinding;
    uint32_t                          dstFirstIndex;
    uint32_t                          descriptorCount;
};

struct LvnPipelineInputAssembly
{
    LvnTopologyType    topology;
    bool               primitiveRestartEnable;
};

struct LvnPipelineRasterizer
{
    LvnCullFaceMode     cullMode;
    LvnCullFrontFace    frontFace;
    LvnPolygonMode      polygonMode;
    float               lineWidth;
    float               depthBiasConstantFactor;
    float               depthBiasClamp;
    float               depthBiasSlopeFactor;
    bool                depthBiasEnable;
    bool                depthClampEnable;
    bool                rasterizerDiscardEnable;
};

struct LvnPipelineMultiSampling
{
    LvnSampleCountFlagBits    rasterizationSamples;
    float                     minSampleShading;
    uint32_t*                 sampleMask;
    bool                      sampleShadingEnable;
    bool                      alphaToCoverageEnable;
    bool                      alphaToOneEnable;
};

struct LvnPipelineColorBlendAttachment
{
    LvnColorComponentFlags    colorWriteMask;
    LvnColorBlendFactor       srcColorBlendFactor;
    LvnColorBlendFactor       dstColorBlendFactor;
    LvnColorBlendOperation    colorBlendOp;
    LvnColorBlendFactor       srcAlphaBlendFactor;
    LvnColorBlendFactor       dstAlphaBlendFactor;
    LvnColorBlendOperation    alphaBlendOp;
    bool                      blendEnable;
};

struct LvnPipelineColorBlend
{
    LvnPipelineColorBlendAttachment*    pColorBlendAttachments;
    uint32_t                            colorBlendAttachmentCount;
    float                               blendConstants[4];
    LvnLogicOperation                   logicOp;
    bool                                logicOpEnable;
};

struct LvnPipelineStencilAttachment
{
    LvnStencilOperation    failOp;
    LvnStencilOperation    passOp;
    LvnStencilOperation    depthFailOp;
    LvnCompareOperation    compareOp;
    uint32_t               compareMask;
    uint32_t               writeMask;
    uint32_t               reference;
};

struct LvnPipelineDepthStencil
{
    LvnCompareOperation             depthOpCompare;
    LvnPipelineStencilAttachment    stencil;
    bool                            depthTestEnable;
    bool                            depthWriteEnable;
    bool                            stencilTestEnable;
};

struct LvnPipelineFixedFunctions
{
    LvnPipelineInputAssembly    inputAssembly;
    LvnPipelineRasterizer       rasterizer;
    LvnPipelineMultiSampling    multisampling;
    LvnPipelineColorBlend       colorBlend;
    LvnPipelineDepthStencil     depthstencil;
};

struct LvnVertexBindingDescription
{
    uint32_t binding, stride;
};

struct LvnVertexAttribute
{
    uint32_t     binding;
    uint32_t     layout;
    LvnFormat    format;
    uint64_t     offset;
};

struct LvnPipelineCreateInfo
{
    const LvnPipelineFixedFunctions*           pipelineFixedFunctions;
    uint32_t                                   vertexBindingDescriptionCount;
    const LvnVertexBindingDescription*         pVertexBindingDescriptions;
    uint32_t                                   vertexAttributeCount;
    const LvnVertexAttribute*                  pVertexAttributes;
    uint32_t                                   descriptorLayoutCount;
    LvnDescriptorLayout* const*                pDescriptorLayouts;
    uint32_t                                   stageCount;
    LvnShader* const*                          pShaderStages;
    LvnRenderPass*                             renderPass;
};

struct LvnBufferCreateInfo
{
    LvnBufferTypeFlags      type;
    LvnBufferMemoryUsage    usage;
    uint64_t                size;
    const void*             data;
};

struct LvnSamplerCreateInfo
{
    LvnTextureFilter    minFilter;
    LvnTextureFilter    magFilter;
    LvnMipmapMode       mipmapMode;
    LvnTextureMode      wrapS;
    LvnTextureMode      wrapT;
    LvnTextureMode      wrapR;
    float               mipLodBias;
    float               minLod;
    float               maxLod;
};

struct LvnTextureCreateInfo
{
    const LvnSampler*      sampler;
    LvnFormat              format;
    LvnSampleCountFlags    samples;
    const uint8_t*         image;
    uint32_t               width;
    uint32_t               height;
};

union LvnClearColorValue
{
    float       float32[4];
    int32_t     int32[4];
    uint32_t    uint32[4];
};

struct LvnClearDepthStencilValue
{
    float       depth;
    uint32_t    stencil;
};

struct LvnExtent2D
{
    uint32_t width, height;
};

struct LvnOffset2D
{
    int32_t x, y;
};

struct LvnRenderArea
{
    LvnExtent2D    extent;
    LvnOffset2D    offset;
};

struct LvnViewport
{
    float x, y, width, height, minDepth, maxDepth;
};

struct LvnRenderPassBeginInfo
{
    LvnRenderPass*                     renderPass;
    LvnFramebuffer*                    framebuffer;
    LvnRenderArea                      renderArea;
    uint32_t                           clearColorValueCount;
    const LvnClearColorValue*          pClearColorValues;
    LvnClearDepthStencilValue          clearDepthStencilValue;
};

struct LvnSubmitInfo
{
    uint32_t                    waitSemaphoreCount;
    LvnSemaphore* const*        pWaitSemaphores;
    uint32_t                    signalSemaphoreCount;
    LvnSemaphore* const*        pSignalSemaphores;
    uint32_t                    commandBufferCount;
    LvnCommandBuffer* const*    pCommandBuffers;
};

struct LvnPresentInfo
{
    uint32_t                waitSemaphoreCount;
    LvnSemaphore* const*    pWaitSemaphores;
    uint32_t                swapchainCount;
    LvnSwapchain* const*    pSwapchains;
    const uint32_t*         pImageIndices;
};

struct LvnGraphicsContextFunctions
{
    PFN_lvnGetSurface                         getSurface;
    PFN_lvnCreateSurface                      createSurface;
    PFN_lvnDestroySurface                     destroySurface;
    PFN_lvnCreateSwapchain                    createSwapchain;
    PFN_lvnDestroySwapchain                   destroySwapchain;
    PFN_lvnCreateRenderPass                   createRenderPass;
    PFN_lvnDestroyRenderPass                  destroyRenderPass;
    PFN_lvnCreateFramebuffer                  createFramebuffer;
    PFN_lvnDestroyFramebuffer                 destroyFramebuffer;
    PFN_lvnCreateShader                       createShader;
    PFN_lvnDestroyShader                      destroyShader;
    PFN_lvnCreateDescriptorLayout             createDescriptorLayout;
    PFN_lvnDestroyDescriptorLayout            destroyDescriptorLayout;
    PFN_lvnCreateDescriptorPool               createDescriptorPool;
    PFN_lvnDestroyDescriptorPool              destroyDescriptorPool;
    PFN_lvnCreatePipeline                     createPipeline;
    PFN_lvnDestroyPipeline                    destroyPipeline;
    PFN_lvnCreateFence                        createFence;
    PFN_lvnDestroyFence                       destroyFence;
    PFN_lvnCreateSemaphore                    createSemaphore;
    PFN_lvnDestroySemaphore                   destroySemaphore;
    PFN_lvnCreateBuffer                       createBuffer;
    PFN_lvnDestroyBuffer                      destroyBuffer;
    PFN_lvnCreateSampler                      createSampler;
    PFN_lvnDestroySampler                     destroySampler;
    PFN_lvnCreateTexture                      createTexture;
    PFN_lvnDestroyTexture                     destroyTexture;
    PFN_lvnCreateCommandBuffer                createCommandBuffer;
    PFN_lvnDestroyCommandBuffer               destroyCommandBuffer;
    PFN_lvnAllocateDescriptorSets             allocateDescriptorSets;
    PFN_lvnResetDescriptorPool                resetDescriptorPool;
    PFN_lvnUpdateDescriptorSets               updateDescriptorSets;
    PFN_lvnSurfaceGetSupportedFormats         surfaceGetSupportedFormats;
    PFN_lvnSurfaceGetSupportedPresentModes    surfaceGetSupportedPresentModes;
    PFN_lvnSwapchainResize                    swapchainResize;
    PFN_lvnSwapchainAcquireNextImage          swapchainAcquireNextImage;
    PFN_lvnFenceWait                          fenceWait;
    PFN_lvnFenceReset                         fenceReset;
    PFN_lvnBufferUpdate                       bufferUpdate;
    PFN_lvnBeginCommandBuffer                 beginCommandBuffer;
    PFN_lvnEndCommandBuffer                   endCommandBuffer;
    PFN_lvnCmdBeginRenderPass                 cmdBeginRenderPass;
    PFN_lvnCmdEndRenderPass                   cmdEndRenderPass;
    PFN_lvnCmdBindPipeline                    cmdBindPipeline;
    PFN_lvnCmdBindVertexBuffer                cmdBindVertexBuffer;
    PFN_lvnCmdBindIndexBuffer                 cmdBindIndexBuffer;
    PFN_lvnCmdBindDescriptorSets              cmdBindDescriptorSets;
    PFN_lvnCmdSetViewport                     cmdSetViewport;
    PFN_lvnCmdSetScissor                      cmdSetScissor;
    PFN_lvnCmdDraw                            cmdDraw;
    PFN_lvnCmdDrawIndexed                     cmdDrawIndexed;
    PFN_lvnRenderSubmit                       renderSubmit;
    PFN_lvnRenderPresent                      renderPresent;
};

struct LvnGraphicsContextCreateInfo
{
    LvnGraphicsApi                        graphicsapi;                      // graphics api backend
    LvnPresentationModeFlags              presentationModeFlags;            // type of output the graphics api will render to
    const LvnPlatformData*                platformData;                     // native platform data for surface creation
    bool                                  enableGraphicsApiDebugLogging;    // enable logging for graphics api layer debug logs
    const LvnGraphicsContextFunctions*    gctxFuncs;                        // use custom graphics functions instead of the supported graphics api functions, will only use if graphicsapi is set to none and gctxFuncs is not null
    struct
    {
        size_t                            baseFrameArenaSize;               // base size of frame arena in bytes
        size_t                            baseCmdBuffFrameArenaSize;        // base size of command buffer frame arena
        size_t                            baseCmdBuffByteStreamSize;        // base size of command buffer byte stream
    } memory;
};


#ifdef __cplusplus
extern "C" {
#endif

LVN_API LvnResult                   lvnCreateGraphicsContext(struct LvnContext* ctx, LvnGraphicsContext** graphicsctx, const LvnGraphicsContextCreateInfo* createInfo); // create the graphics context
LVN_API void                        lvnDestroyGraphicsContext(LvnGraphicsContext* graphicsctx);                                                                         // destroy the graphics context
LVN_API const LvnSurface*           lvnGetSurface(const LvnGraphicsContext* graphicsctx);

LVN_API LvnResult                   lvnCreateSurface(const LvnGraphicsContext* graphicsctx, LvnSurface** surface, const LvnSurfaceCreateInfo* createInfo);
LVN_API void                        lvnDestroySurface(LvnSurface* surface);
LVN_API LvnResult                   lvnCreateSwapchain(const LvnGraphicsContext* graphicsctx, LvnSwapchain** swapchain, const LvnSwapchainCreateInfo* createInfo);
LVN_API void                        lvnDestroySwapchain(LvnSwapchain* swapchain);
LVN_API LvnResult                   lvnCreateRenderPass(const LvnGraphicsContext* graphicsctx, LvnRenderPass** renderpass, const LvnRenderPassCreateInfo* createInfo);
LVN_API void                        lvnDestroyRenderPass(LvnRenderPass* renderpass);
LVN_API LvnResult                   lvnCreateFramebuffer(const LvnGraphicsContext* graphicsctx, LvnFramebuffer** framebuffer, const LvnFramebufferCreateInfo* createInfo);
LVN_API void                        lvnDestroyFramebuffer(LvnFramebuffer* framebuffer);
LVN_API LvnResult                   lvnCreateShader(const LvnGraphicsContext* graphicsctx, LvnShader** shader, const LvnShaderCreateInfo* createInfo);
LVN_API void                        lvnDestroyShader(LvnShader* shader);
LVN_API LvnResult                   lvnCreateDescriptorLayout(const LvnGraphicsContext* graphicsctx, LvnDescriptorLayout** descriptorLayout, const LvnDescriptorLayoutCreateInfo* createInfo);
LVN_API void                        lvnDestroyDescriptorLayout(LvnDescriptorLayout* descriptorLayout);
LVN_API LvnResult                   lvnCreateDescriptorPool(const LvnGraphicsContext* graphicsctx, LvnDescriptorPool** descriptorPool, const LvnDescriptorPoolCreateInfo* createInfo);
LVN_API void                        lvnDestroyDescriptorPool(LvnDescriptorPool* descriptorPool);
LVN_API LvnResult                   lvnCreatePipeline(const LvnGraphicsContext* graphicsctx, LvnPipeline** pipeline, const LvnPipelineCreateInfo* createInfo);
LVN_API void                        lvnDestroyPipeline(LvnPipeline* pipeline);
LVN_API LvnResult                   lvnCreateFence(const LvnGraphicsContext* graphicsctx, LvnFence** fence, bool signaled);
LVN_API void                        lvnDestroyFence(LvnFence* fence);
LVN_API LvnResult                   lvnCreateSemaphore(const LvnGraphicsContext* graphicsctx, LvnSemaphore** semaphore);
LVN_API void                        lvnDestroySemaphore(LvnSemaphore* semaphore);
LVN_API LvnResult                   lvnCreateBuffer(const LvnGraphicsContext* graphicsctx, LvnBuffer** buffer, const LvnBufferCreateInfo* createInfo);
LVN_API void                        lvnDestroyBuffer(LvnBuffer* buffer);
LVN_API LvnResult                   lvnCreateSampler(const LvnGraphicsContext* graphicsctx, LvnSampler** sampler, const LvnSamplerCreateInfo* createInfo);
LVN_API void                        lvnDestroySampler(LvnSampler* sampler);
LVN_API LvnResult                   lvnCreateTexture(const LvnGraphicsContext* graphicsctx, LvnTexture** texture, const LvnTextureCreateInfo* createInfo);
LVN_API void                        lvnDestroyTexture(LvnTexture* texture);
LVN_API LvnResult                   lvnCreateCommandBuffer(const LvnGraphicsContext* graphicsctx, LvnCommandBuffer** commandBuffer);
LVN_API void                        lvnDestroyCommandBuffer(LvnCommandBuffer* commandBuffer);

LVN_API LvnResult                   lvnAllocateDescriptorSets(const LvnGraphicsContext* graphicsctx, LvnDescriptorSet** pDescriptorSets, LvnDescriptorSetAllocateInfo* allocInfo);
LVN_API LvnResult                   lvnResetDescriptorPool(const LvnGraphicsContext* graphicsctx, LvnDescriptorPool* descriptorPool);
LVN_API LvnResult                   lvnUpdateDescriptorSets(const LvnGraphicsContext* graphicsctx, uint32_t descriptorWriteCount, const LvnDescriptorSetWriteInfo* pDescriptorWrites, uint32_t descriptorCopyCount, const LvnDescriptorSetCopyInfo* pDescriptorCopies);

LVN_API void                        lvnSurfaceGetSupportedFormats(const LvnSurface* surface, uint32_t* formatCount, LvnFormat* pSurfaceFormats);
LVN_API void                        lvnSurfaceGetSupportedPresentModes(const LvnSurface* surface, uint32_t* presentModeCount, LvnPresentMode* pPresentModes);

LVN_API LvnFormat                   lvnSwapchainGetFormat(const LvnSwapchain* swapchain);
LVN_API LvnTexture*                 lvnSwapchainGetImage(LvnSwapchain* swapchain, uint32_t imageIndex);
LVN_API uint32_t                    lvnSwapchainGetImageCount(const LvnSwapchain* swapchain);
LVN_API LvnExtent2D                 lvnSwapchainGetExtent(const LvnSwapchain* swapchain);
LVN_API LvnResult                   lvnSwapchainResize(LvnSwapchain* swapchain, uint32_t width, uint32_t height);
LVN_API LvnResult                   lvnSwapchainAcquireNextImage(LvnSwapchain* swapchain, LvnSemaphore* semaphore, LvnFence* fence, uint32_t* imageIndex);
LVN_API LvnPipelineFixedFunctions   lvnConfigPipelineFixedFunctionsInit(void);
LVN_API LvnResult                   lvnFenceWait(LvnFence* fence, uint64_t timeout);
LVN_API LvnResult                   lvnFenceReset(LvnFence* fence);

LVN_API void                        lvnBufferUpdate(LvnBuffer* buffer, void* data, uint64_t size, uint64_t offset);

LVN_API void                        lvnBeginCommandBuffer(LvnCommandBuffer* commandBuffer);
LVN_API void                        lvnEndCommandBuffer(LvnCommandBuffer* commandBuffer);
LVN_API void                        lvnCmdBeginRenderPass(LvnCommandBuffer* commandBuffer, LvnRenderPassBeginInfo* beginInfo);
LVN_API void                        lvnCmdEndRenderPass(LvnCommandBuffer* commandBuffer);
LVN_API void                        lvnCmdBindPipeline(LvnCommandBuffer* commandBuffer, LvnPipeline* pipeline);
LVN_API void                        lvnCmdBindVertexBuffer(LvnCommandBuffer* commandBuffer, uint32_t firstBinding, uint32_t bindingCount, LvnBuffer** pBuffers, uint64_t* pOffsets);
LVN_API void                        lvnCmdBindIndexBuffer(LvnCommandBuffer* commandBuffer, LvnBuffer* buffer, uint64_t offset);
LVN_API void                        lvnCmdBindDescriptorSets(LvnCommandBuffer* commandBuffer, LvnPipeline* pipeline, uint32_t firstSet, uint32_t descriptorSetCount, LvnDescriptorSet* const* pDescriptorSets, uint32_t dynamicOffsetCount, const uint32_t* pDynamicOffsets);
LVN_API void                        lvnCmdSetViewport(LvnCommandBuffer* commandBuffer, const LvnViewport* viewport);
LVN_API void                        lvnCmdSetScissor(LvnCommandBuffer* commandBuffer, const LvnRenderArea* scissor);
LVN_API void                        lvnCmdDraw(LvnCommandBuffer* commandBuffer, uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);
LVN_API void                        lvnCmdDrawIndexed(LvnCommandBuffer* commandBuffer, uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance);
LVN_API LvnResult                   lvnRenderSubmit(const LvnGraphicsContext* graphicsctx, const LvnSubmitInfo* pSubmits, uint32_t submitCount, LvnFence* fence);
LVN_API LvnResult                   lvnRenderPresent(const LvnGraphicsContext* graphicsctx, const LvnPresentInfo* presentInfo);

#ifdef __cplusplus
}
#endif


#endif // !HG_LVN_GRAPHICS_H
