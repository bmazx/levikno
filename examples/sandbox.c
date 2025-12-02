#include <levikno/levikno.h>
#include <stdio.h>

int main(int argc, char** argv)
{
    LvnContext* ctx;
    lvnCreateContext(&ctx, NULL);


    lvnDestroyContext(ctx);
}
