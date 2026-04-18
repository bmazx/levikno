
#include <levikno/levikno.h>
#include <levikno/lvn_graphics.h>
#include <stdio.h>
#include <stdlib.h>

#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WAYLAND
#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3native.h>

typedef struct WindowData
{
    bool framebufferResized;
    int width, height;
} WindowData;

static float s_Vertices[] = {
    0.0f,-0.5f, 1.0f, 0.0f, 0.0f,
    0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
   -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
};

static uint32_t s_Indices[] = {
    0, 1, 2,
};

void myPrint(const char* msg)
{
    printf("%s", msg);
}

char* myLogPattern(const LvnLogMessage* logmsg)
{
    return lvnLogCreateOneShotStrMsg(">>>");
}

static void GLFWerrorCallback(int error, const char* descripion)
{
    printf("[glfw]: (%d): %s\n", error, descripion);
}

static size_t s_MemAllocCount = 0;

void* customMalloc(size_t size, void* userData)
{
    if (!size) { return NULL; }
    void* ptr = malloc(size);
    if (!ptr) { printf("alloc fail\n"); exit(-1); }
    s_MemAllocCount++;
    return ptr;
}

void customFree(void* ptr, void* userData)
{
    if (!ptr) { return; }
    s_MemAllocCount--;
    free(ptr);
}

void* customRealloc(void* ptr, size_t size, void* userData)
{
    if (!ptr) { return customMalloc(size, userData); }
    void* newptr = realloc(ptr, size);
    if (!newptr) { printf("realloc fail\n"); exit(-1); }
    return newptr;
}

void resizeFramebuffers(const LvnGraphicsContext* graphicsctx, LvnSwapchain* swapchain, LvnRenderPass* renderPass, LvnFramebuffer*** swapchainFramebuffers, uint32_t frameBufferCount, uint32_t width, uint32_t height)
{
    for (uint32_t i = 0; i < frameBufferCount; i++)
    {
        lvnDestroyFramebuffer((*swapchainFramebuffers)[i]);
    }
    free(*swapchainFramebuffers);

    uint32_t imageCount = lvnSwapchainGetImageCount(swapchain);

    *swapchainFramebuffers = (LvnFramebuffer**) malloc(imageCount * sizeof(LvnFramebuffer*));

    for (uint32_t i = 0; i < imageCount; i++)
    {
        LvnTexture* swapchainImage = lvnSwapchainGetImage(swapchain, i);
        LvnFramebufferCreateInfo framebufferCreateInfo = {
            .renderPass = renderPass,
            .pColorAttachments = &swapchainImage,
            .colorAttachmentCount = 1,
            .width = width,
            .height = height,
        };

        lvnCreateFramebuffer(graphicsctx, &(*swapchainFramebuffers)[i], &framebufferCreateInfo);
    }
}

static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    WindowData* winData = (WindowData*) glfwGetWindowUserPointer(window);
    winData->framebufferResized = true;
    winData->width = width;
    winData->height = height;
}

int main(int argc, char** argv)
{
    lvnSetMemAllocCallbacks(customMalloc, customFree, customRealloc, NULL);

    LvnContextCreateInfo ctxCreateInfo =
    {
        .logging.enableLogging = true,
    };

    LvnContext* ctx;
    lvnCreateContext(&ctx, &ctxCreateInfo);

    LvnLogger* logger = lvnCtxGetCoreLogger(ctx);

    float b = 3.1415f;
    const char* c = "world";

    lvnLogMessageTrace(logger, "hello %d | %s | %.5f | %p", 12, c, b, &b);
    lvnLogMessageDebug(logger, "hello %d | %s | %.5f | %p", 12, c, b, &b);
    lvnLogMessageInfo(logger, "hello %d | %s | %.5f | %p", 12, c, b, &b);
    lvnLogMessageWarn(logger, "hello %d | %s | %.5f | %p", 12, c, b, &b);
    lvnLogMessageError(logger, "hello %d | %s | %.5f | %p", 12, c, b, &b);
    lvnLogMessageFatal(logger, "hello %d | %s | %.5f | %p", 12, c, b, &b);

    uint32_t len = lvnLogFormatMessageArgs(logger, NULL, 0, Lvn_LogLevel_Warn, "hello world %s , %d", c, 42);
    char* str = (char*) malloc(len * sizeof(char));
    lvnLogFormatMessageArgs(logger, str, len, Lvn_LogLevel_Warn, "hello world %s , %d", c, 42);

    printf("%.*s", len, str);
    free(str);

    LvnSink sink =
    {
        .logFunc = myPrint,
    };

    LvnLoggerCreateInfo logCreateInfo =
    {
        .name = "myLog",
        .level = Lvn_LogLevel_None,
        .format = "[%Y-%m-%d] [%T] [%#%l%^] %> %n: %v%$",
        .pSinks = &sink,
        .sinkCount = 1,
    };

    LvnLogger* mylog;
    lvnCreateLogger(ctx, &mylog, &logCreateInfo);

    LvnLogPattern logPattern =
    {
        .symbol = '>',
        .func = myLogPattern,
    };

    lvnCtxAddLogPatterns(ctx, &logPattern, 1);

    lvnLogParseLogPatternFormat(mylog, "[%Y-%m-%d] [%#%l%^] %> %n: %v%$");

    lvnLogMessageDebug(mylog, "hello there %f", 3.1415);
    lvnLogMessageError(mylog, "hello there %f", 3.1415);

    lvnDestroyLogger(mylog);



    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwSetErrorCallback(GLFWerrorCallback);

    /* Create a windowed mode window and its OpenGL context */
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(800, 600, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    WindowData winData = {0};
    glfwSetWindowUserPointer(window, &winData);

    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

    // struct wl_display* nativeDisplay = glfwGetWaylandDisplay();
    // struct wl_surface* nativeWindow = glfwGetWaylandWindow(window);

    Display* nativeDisplay = glfwGetX11Display();
    Window nativeWindow = glfwGetX11Window(window);

    LvnPlatformData pd = {0};
    pd.ndh = nativeDisplay;
    pd.nwh = &nativeWindow;

    LvnGraphicsContextCreateInfo graphicsCreateInfo = {0};
    graphicsCreateInfo.graphicsapi = Lvn_GraphicsApi_Opengl;
    graphicsCreateInfo.presentationModeFlags = Lvn_PresentationModeFlag_Headless | Lvn_PresentationModeFlag_Surface;
    graphicsCreateInfo.platformData = &pd;
    graphicsCreateInfo.enableGraphicsApiDebugLogging = true;

    LvnGraphicsContext* graphicsctx;
    lvnCreateGraphicsContext(ctx, &graphicsctx, &graphicsCreateInfo);

    lvnDestroyGraphicsContext(graphicsctx);

    lvnDestroyContext(ctx);

    glfwDestroyWindow(window);
    glfwTerminate();

    printf("s_MemAllocCount: %zu\n", s_MemAllocCount);
}
