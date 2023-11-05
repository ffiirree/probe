#ifndef PROBE_CPU_H
#define PROBE_CPU_H

#include "probe/dllport.h"
#include "probe/types.h"

#include <cstdint>
#include <string>
#include <vector>

namespace probe::cpu
{
    enum class endianness_t
    {
        little,
        big
    };

    enum class architecture_t
    {
        unknown,
        x86,
        i386 = x86,
        ia64,
        itanium = ia64,
        x64,
        amd64  = x64,
        x86_64 = x64,
        arm,
        arm64
    };

    struct quantities_t
    {
        // Hyperthreads / Logical processors.
        uint32_t logical{};
        // Physical "Cores".
        uint32_t physical{};
        // Physical CPU units/packages/sockets.
        uint32_t packages{};
    };

    struct cpu_info_t
    {
        std::string    name{};
        vendor_t       vendor{};
        architecture_t arch{};

        endianness_t endianness{};
        // base speed
        uint64_t     frequency{};
        quantities_t quantities{};
    };

    enum class cache_type_t
    {
        unified,
        instruction,
        data,
        trace,
    };

    struct cache_t
    {
        int32_t      level{};
        int32_t      associativity{};
        uint64_t     line_size{};
        uint64_t     size{};
        cache_type_t type{};
    };

    PROBE_API architecture_t architecture();

    PROBE_API endianness_t endianness();

    PROBE_API quantities_t quantities();

    PROBE_API std::vector<cache_t> caches();

    PROBE_API std::vector<cache_t> cache(int, cache_type_t);

    // convert string to cache_type_t
    PROBE_API cache_type_t to_cache_type(const std::string&);

    PROBE_API uint64_t frequency();

    // vendor type
    PROBE_API vendor_t vendor();

    // cpu name
    PROBE_API std::string name();

    PROBE_API cpu_info_t info();

    // cpuid
    PROBE_API void cpuid(int32_t (&)[4], int32_t, int32_t);

    // clang-format off
    enum class feature_t : uint64_t
    {
        // cpuid(EAX=0x0000'0001): ECX
        sse3                = 0x0000'0001'00'02'0000, // Prescott New Instructions-SSE3 (PNI)
        pclmulqdq           = 0x0000'0001'00'02'0001,
        monitor             = 0x0000'0001'00'02'0003, // MONITOR and MWAIT instructions (SSE3)
        vmx                 = 0x0000'0001'00'02'0005, // Virtual Machine eXtensions
        ssse3               = 0x0000'0001'00'02'0009, // Supplemental SSE3 instructions
        fma                 = 0x0000'0001'00'02'000C, // Fused multiply-add (FMA3)
        cx16                = 0x0000'0001'00'02'000D, // CMPXCHG16B instruction
        pcid                = 0x0000'0001'00'02'0011, // Process context identifiers (CR4 bit 17)
        dca                 = 0x0000'0001'00'02'0012, // Direct cache access for DMA writes
        sse4_1              = 0x0000'0001'00'02'0013,
        sse4_2              = 0x0000'0001'00'02'0014,
        x2apic              = 0x0000'0001'00'02'0015, // Intel's Advanced Programmable Interrupt Controller
        movbe               = 0x0000'0001'00'02'0016,
        popcnt              = 0x0000'0001'00'02'0017,
        aes                 = 0x0000'0001'00'02'0019,
        xsave               = 0x0000'0001'00'02'001A, // XSAVE, XRESTOR, XSETBV, XGETBV
        osxsave             = 0x0000'0001'00'02'001B, // XSAVE enabled by OS
        avx                 = 0x0000'0001'00'02'001C,
        f16c                = 0x0000'0001'00'02'001D,
        rdrnd               = 0x0000'0001'00'02'001E,
        hypervisor          = 0x0000'0001'00'02'001F, // Hypervisor present (always zero on physical CPUs)

        // cpuid(EAX=0x0000'0001): EDX
        fpu                 = 0x0000'0001'00'03'0000, // Onboard x87 FPU
        vme                 = 0x0000'0001'00'03'0001, // Virtual 8086 mode extensions
        de                  = 0x0000'0001'00'03'0002,
        pse                 = 0x0000'0001'00'03'0003, // Page Size Extension
        tsc                 = 0x0000'0001'00'03'0004, // Time Stamp Counter
        msr                 = 0x0000'0001'00'03'0005, // Model-specific registers
        pae                 = 0x0000'0001'00'03'0006, // Physical Address Extension
        mce                 = 0x0000'0001'00'03'0007, // Machine Check Exception
        cx8                 = 0x0000'0001'00'03'0008, // CMPXCHG8 (compare-and-swap) instruction
        apic                = 0x0000'0001'00'03'0009, // Onboard Advanced Programmable Interrupt Controller
        sep                 = 0x0000'0001'00'03'000B, // SYSENTER and SYSEXIT instructions
        mtrr                = 0x0000'0001'00'03'000C,
        pge                 = 0x0000'0001'00'03'000D, // Page Global Enable bit in CR4
        mca                 = 0x0000'0001'00'03'000E, // Machine check architecture
        cmov                = 0x0000'0001'00'03'000F, // Conditional move and FCMOV instructions
        pat                 = 0x0000'0001'00'03'0010, // Page Attribute Table
        pse36               = 0x0000'0001'00'03'0011, // 36-bit page size extension
        psn                 = 0x0000'0001'00'03'0012, // Processor Serial Number
        clfsh               = 0x0000'0001'00'03'0013, // CLFLUSH instruction
        nx                  = 0x0000'0001'00'03'0014, // NX bit (Itanium only)[
        acpi                = 0x0000'0001'00'03'0016, // Onboard thermal control MSRs for ACPI
        mmx                 = 0x0000'0001'00'03'0017, // MMX instructions
        fxsr                = 0x0000'0001'00'03'0018, // FXSAVE, FXRESTOR instructions, CR4 bit 9
        sse                 = 0x0000'0001'00'03'0019,
        sse2                = 0x0000'0001'00'03'001A,
        ss                  = 0x0000'0001'00'03'001B, // CPU cache implements self-snoop
        htt                 = 0x0000'0001'00'03'001C, // Hyper-threading
        tm                  = 0x0000'0001'00'03'001D, // Thermal monitor automatically limits temperature
        ia64                = 0x0000'0001'00'03'001E, // IA64 processor emulating x86
        pbe                 = 0x0000'0001'00'03'001F, // Pending Break Enable (PBE# pin) wakeup capability

        // Extended Features
        // cpuid(EAX=0x0000'0007, ECX=0): EBX
        fsgsbase            = 0x0000'0007'00'01'0000, // Access to base of %fs and %gs
        sgx                 = 0x0000'0007'00'01'0002, // Software Guard Extensions, Intel
        bmi1                = 0x0000'0007'00'01'0003, // Bit Manipulation Instruction Set 1
        hle                 = 0x0000'0007'00'01'0004, // TSX Hardware Lock Elision
        avx2                = 0x0000'0007'00'01'0005, // Advanced Vector Extensions 2
        smep                = 0x0000'0007'00'01'0007, // Supervisor Mode Execution Prevention
        bmi2                = 0x0000'0007'00'01'0008, // Bit Manipulation Instruction Set 2
        erms                = 0x0000'0007'00'01'0009, // Enhanced REP MOVSB/STOSB
        invpcid             = 0x0000'0007'00'01'000A, // INVPCID instruction
        rtm                 = 0x0000'0007'00'01'000B, // TSX Restricted Transactional Memory
        mpx                 = 0x0000'0007'00'01'000E, // Intel MPX (Memory Protection Extensions)
        avx512_f            = 0x0000'0007'00'01'0010,
        avx512_dq           = 0x0000'0007'00'01'0011,
        rdseed              = 0x0000'0007'00'01'0012,
        adx                 = 0x0000'0007'00'01'0013, // Intel ADX
        smap                = 0x0000'0007'00'01'0014,
        avx512_ifma         = 0x0000'0007'00'01'0015,
        clflushopt          = 0x0000'0007'00'01'0017,
        clwb                = 0x0000'0007'00'01'0018,
        pt                  = 0x0000'0007'00'01'0019, // Intel Processor Trace
        avx512_pf           = 0x0000'0007'00'01'001A,
        avx512_er           = 0x0000'0007'00'01'001B,
        avx512_cd           = 0x0000'0007'00'01'001C,
        sha                 = 0x0000'0007'00'01'001D,
        avx512_bw           = 0x0000'0007'00'01'001E,
        avx512_vl           = 0x0000'0007'00'01'001F,

        // cpuid(EAX=0x0000'0007, ECX=0): ECX
        prefetchwt1         = 0x0000'0007'00'02'0000,
        avx512_vbmi         = 0x0000'0007'00'02'0001,
        umip                = 0x0000'0007'00'02'0002,
        pku                 = 0x0000'0007'00'02'0003, // Memory Protection Keys for User-mode pages
        ospke               = 0x0000'0007'00'02'0004, // PKU enabled by OS
        waitpkg             = 0x0000'0007'00'02'0005, // Timed pause and user-level monitor/wait instructions
        avx512_vbmi2        = 0x0000'0007'00'02'0006,
        cet_ss              = 0x0000'0007'00'02'0007, // Control flow enforcement (CET): shadow stack
        gfni                = 0x0000'0007'00'02'0008, // Galois Field instructions
        vaes                = 0x0000'0007'00'02'0009, // Vector AES instruction set (VEX-256/EVEX)
        vpclmulqdq          = 0x0000'0007'00'02'000A, // CLMUL instruction set (VEX-256/EVEX)
        avx512_vnni         = 0x0000'0007'00'02'000B,
        avx512_bitalg       = 0x0000'0007'00'02'000C,
        tme                 = 0x0000'0007'00'02'000D, // Total Memory Encryption MSRs available
        avx512_vpopcntdq    = 0x0000'0007'00'02'000E, // AVX-512 Vector Population Count Double and Quad-word
        la57                = 0x0000'0007'00'02'0010,
        mawau               = 0x0000'0007'00'02'0011, // 0x11-0x15
        rdpid               = 0x0000'0007'00'02'0016,
        kl                  = 0x0000'0007'00'02'0017, // AES Key Locker
        cldemote            = 0x0000'0007'00'02'0019,
        movdiri             = 0x0000'0007'00'02'001B,
        movdir64b           = 0x0000'0007'00'02'001C,
        enqcmd              = 0x0000'0007'00'02'001D,
        sgx_lc              = 0x0000'0007'00'02'001E,
        pks                 = 0x0000'0007'00'02'001F, // Protection keys for supervisor-mode pages

        // cpuid(EAX=0x0000'0007, ECX=0): EDX
        sgx_keys            = 0x0000'0007'00'03'0001, // Attestation Services for Intel SGX
        avx512_4vnniw       = 0x0000'0007'00'03'0002,
        avx512_4fmaps       = 0x0000'0007'00'03'0003,
        fsrm                = 0x0000'0007'00'03'0004, // Fast Short REP MOVSB
        uintr               = 0x0000'0007'00'03'0005, // User Inter-processor Interrupts
        avx512_vp2intersect = 0x0000'0007'00'03'0008,
        srdbs_ctrl          = 0x0000'0007'00'03'0009, // Special Register Buffer Data Sampling Mitigations
        mc_clear            = 0x0000'0007'00'03'000A, // VERW instruction clears CPU buffers
        rtm_always_abort    = 0x0000'0007'00'03'000B,
        serialize           = 0x0000'0007'00'03'000E,
        hybrid              = 0x0000'0007'00'03'000F, // Mixture of CPU types in processor topology
        tsxldtrk            = 0x0000'0007'00'03'0010,
        pconfig             = 0x0000'0007'00'03'0012, // 	Platform configuration
        lbr                 = 0x0000'0007'00'03'0013, // Architectural Last Branch Records
        cet_ibt             = 0x0000'0007'00'03'0014,
        amx_bf16            = 0x0000'0007'00'03'0016,
        avx512_fp16         = 0x0000'0007'00'03'0017,
        amx_tile            = 0x0000'0007'00'03'0018,
        amx_int8            = 0x0000'0007'00'03'0019,
        stibp               = 0x0000'0007'00'03'001B, // Single Thread Indirect Branch Predictor, part of IBC
        ssbd                = 0x0000'0007'00'03'001F, // Speculative Store Bypass Disable

        // cpuid(EAX=0x0000'0007, ECX=1): EAX
        rao_int             = 0x0000'0007'01'00'0003,
        avx_vnni            = 0x0000'0007'01'00'0004,
        avx512_bf16         = 0x0000'0007'01'00'0005,
        lass                = 0x0000'0007'01'00'0006,
        cmpccxadd           = 0x0000'0007'01'00'0007,
        archperfmonext      = 0x0000'0007'01'00'0008,
        fred                = 0x0000'0007'01'00'0011,
        lkgs                = 0x0000'0007'01'00'0012,
        wrmsrns             = 0x0000'0007'01'00'0013,
        amx_fp16            = 0x0000'0007'01'00'0015,
        hreset              = 0x0000'0007'01'00'0016,
        avx_ifma            = 0x0000'0007'01'00'0017,
        lam                 = 0x0000'0007'01'00'001A,
        msrlist             = 0x0000'0007'01'00'001B,

        // cpuid(EAX=0x0000'0007, ECX=1): EDX
        avx_vnn_int8        = 0x0000'0007'01'03'0004,
        avx_ne_convert      = 0x0000'0007'01'03'0005,
        amx_complex         = 0x0000'0007'01'03'0008,
        prefetchiti         = 0x0000'0007'01'03'000E,
        cet_sss             = 0x0000'0007'01'03'0012,

        // cpuid(EAX=0x0000'000D, ECX=1): EAX
        xsaveopt            = 0x0000'000D'01'00'0000,
        xsavec              = 0x0000'000D'01'00'0001,
        xgetbv_ecx1         = 0x0000'000D'01'00'0002,
        xss                 = 0x0000'000D'01'00'0003,
        xfd                 = 0x0000'000D'01'00'0004,

        // cpuid(EAX=0x0000'0019): EBX
        aes_kle             = 0x0000'0019'00'01'0000,
        aes_wide_kl         = 0x0000'0019'00'01'0002,
        kl_msrs             = 0x0000'0019'00'01'0004,

        // cpuid(EAX=0x8000'0001): ECX
        lahf_lm             = 0x8000'0001'00'02'0000, // LAHF/SAHF in long mode
        cmp_legacy          = 0x8000'0001'00'02'0001,
        svm                 = 0x8000'0001'00'02'0002, // Secure Virtual Machine
        extapic             = 0x8000'0001'00'02'0003, // Extended APIC space
        cr8_legacy          = 0x8000'0001'00'02'0004,
        lzcnt               = 0x8000'0001'00'02'0005, // Intel
        abm                 = 0x8000'0001'00'02'0005, // AMD
        sse4a               = 0x8000'0001'00'02'0006,
        misalignsse         = 0x8000'0001'00'02'0007,
        amd_3dnowprefetch   = 0x8000'0001'00'02'0008,
        osvw                = 0x8000'0001'00'02'0009, // OS Visible Workaround
        ibs                 = 0x8000'0001'00'02'000A, // Instruction Based Sampling
        xop                 = 0x8000'0001'00'02'000B, // XOP instruction set
        skinit              = 0x8000'0001'00'02'000C,
        wdt                 = 0x8000'0001'00'02'000D, // 	Watchdog timer
        lwp                 = 0x8000'0001'00'02'000F, // Light Weight Profiling
        fma4                = 0x8000'0001'00'02'0010, // 4-operand fused multiply-add instructions
        tce                 = 0x8000'0001'00'02'0011, // Translation Cache Extension
        nodeid_msr          = 0x8000'0001'00'02'0013, // NodeID MSR
        tbm                 = 0x8000'0001'00'02'0015, // Trailing Bit Manipulation
        perfctr_core        = 0x8000'0001'00'02'0017,
        perfctr_nb          = 0x8000'0001'00'02'0018,
        dbx                 = 0x8000'0001'00'02'001A, // Data breakpoint extensions
        perftsc             = 0x8000'0001'00'02'001B, // Performance timestamp counter (PTSC)
        pcx_l2i             = 0x8000'0001'00'02'001C, // L2I perf counter extensions
        monitorx            = 0x8000'0001'00'02'001D,
        addr_mask_ext       = 0x8000'0001'00'02'001E,

        // cpuid(EAX=0x8000'0001): EDX

        syscall             = 0x8000'0001'00'03'000B,
        ecc                 = 0x8000'0001'00'03'0013,
        mmxext              = 0x8000'0001'00'03'0016,
        fxsr_opt            = 0x8000'0001'00'03'0019,
        pdpe1gb             = 0x8000'0001'00'03'001A, // Gigabyte pages
        rdtscp              = 0x8000'0001'00'03'001B, // RDTSCP instruction
        lm                  = 0x8000'0001'00'03'001D,
        amd_3dnowext        = 0x8000'0001'00'03'001E,
        amd_3dnow           = 0x8000'0001'00'03'001F,

        // masks
        leaf_mask           = 0xffff'ffff'00'00'0000,
        subleaf_mask        = 0x0000'0000'ff'00'0000,
        register_mask       = 0x0000'0000'00'ff'0000,
        bit_mask            = 0x0000'0000'00'00'00ff,

        // unknown
        unknown             = 0xffff'ffff'ffff'ffff,
    };
    // clang-format on

    PROBE_API std::tuple<int32_t, int32_t, int32_t, int32_t> unpack(feature_t);

    PROBE_API std::vector<feature_t> instruction_sets();

    PROBE_API bool is_supported(feature_t);
} // namespace probe::cpu

namespace probe
{
    PROBE_API std::string to_string(cpu::architecture_t);
    PROBE_API std::string to_string(cpu::endianness_t);
    PROBE_API std::string to_string(cpu::feature_t);
    PROBE_API std::string to_string(cpu::cache_type_t);
} // namespace probe

#endif //! PROBE_CPU_H