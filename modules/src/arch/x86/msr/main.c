#include <arch/x86/cpuid/cpuid.h>
#include <arch/x86/msr/msr.h>

void init()
{

    struct cpuid_data data;

    cpuid_get(CPUID_INSTRUCTION_FEATURES0, &data);

    if (!(data.edx & CPUID_FEATURES0_EDX_FLAG_MSR))
        return;

}

void destroy()
{

}

