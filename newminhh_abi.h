#ifndef NEWMINHH_ABI_H
#define NEWMINHH_ABI_H

/* Byte offsets into newminhh_cpu_t (shared between C and GAS assembly) */
#define NM_CPU_OFF_GPR          0
#define NM_CPU_OFF_EAX          0
#define NM_CPU_OFF_ECX          4
#define NM_CPU_OFF_EDX          8
#define NM_CPU_OFF_EBX          12
#define NM_CPU_OFF_ESP          16
#define NM_CPU_OFF_EBP          20
#define NM_CPU_OFF_ESI          24
#define NM_CPU_OFF_EDI          28
#define NM_CPU_OFF_EIP          32
#define NM_CPU_OFF_EFLAGS       36
#define NM_CPU_OFF_GS_BASE      40
#define NM_CPU_OFF_FS_BASE      44
#define NM_CPU_OFF_TLS_BASES    48
#define NM_CPU_OFF_EXIT_REASON  60
#define NM_CPU_OFF_EXIT_ARG     64
#define NM_CPU_OFF_FXSAVE       80
#define NM_CPU_SIZE             592

/* Exit reason constants */
#define NM_EXIT_NONE            0
#define NM_EXIT_SYSCALL         1
#define NM_EXIT_COMPAT_CALL     2
#define NM_EXIT_HLT             3
#define NM_EXIT_UNSUPPORTED     4
#define NM_EXIT_RET_TO_HOST     5

/* Argument / return signature kind constants */
#define NM_ARG_NONE             0
#define NM_ARG_I32              1
#define NM_ARG_U32              2
#define NM_ARG_PTR              3
#define NM_ARG_I64              4
#define NM_ARG_U64              5
#define NM_ARG_F32              6
#define NM_ARG_F64              7

#define NM_RET_VOID             0
#define NM_RET_I32              1
#define NM_RET_U32              2
#define NM_RET_PTR              3
#define NM_RET_I64              4
#define NM_RET_U64              5
#define NM_RET_F32              6
#define NM_RET_F64              7

#define NM_MAX_SIG_ARGS         16

/* Callback / symbol flags */
#define NM_CB_MODE_NATIVE_CDECL 0
#define NM_CB_MODE_DBT          1
#define NM_CB_FLAG_UNTYPED      (1u << 0)

#define NM_SYM_FLAG_DATA        (1u << 0)
#define NM_SYM_FLAG_VARIADIC    (1u << 1)
#define NM_SYM_FLAG_CDECL_DIRECT (1u << 2)
#define NM_SYM_FLAG_IFUNC        (1u << 3)
#define NM_SYM_FLAG_NATIVE       (1u << 4) /* wrapper_fn is a native x32 symbol */
#define NM_SYM_FLAG_PATCHED      (1u << 5) /* cdecl entry of a patched DSO */
#define NM_SYM_FLAG_STATIC_ONLY  (1u << 6)
#define NM_SYM_FLAG_RUNTIME_ONLY (1u << 7)
#define NM_SYM_FLAG_WEAK         (1u << 8)
#define NM_SYM_FLAG_PURE         (1u << 9)

#define NM_RTLD_CDECL           0x10000

/* nm_fn_sig_t ABI extension: abi_ext is 0 for legacy behaviour.
 * NM_SIG_EXT_SRET: hidden sret pointer passed by the caller.
 * NM_SIG_EXT_CALLEE_POP4: callee pops 4 bytes after returning (i386 SysV).
 * NM_SIG_EXT_SRET_SIZE: number of extra i386 stack slots the callee pops, bits 3..7. */
#define NM_SIG_EXT_SRET         (1u << 0)
#define NM_SIG_EXT_CALLEE_POP4  (1u << 1)
#ifdef __ASSEMBLER__
#  define NM_SIG_EXT_SRET_SIZE(e)  ((e) >> 3)
#  define NM_SIG_EXT_MAKE_SRET(n)  (NM_SIG_EXT_SRET | NM_SIG_EXT_CALLEE_POP4 | ((n) << 3))
#else
#  define NM_SIG_EXT_SRET_SIZE(e)  ((uint8_t)((e) >> 3))
#  define NM_SIG_EXT_MAKE_SRET(n)  ((uint8_t)(NM_SIG_EXT_SRET | NM_SIG_EXT_CALLEE_POP4 | ((n) << 3)))
#endif

/* .note.newminhh v2 constants (loader/patcher; GAS-safe without C casts) */
#define NM_NOTE_NAME                  "newminhh"
#define NM_NOTE_TYPE_PATCHED          0x4E4D4848u
#define NM_NOTE_V2_VERSION            2u
#define NM_NOTE_V2_FLAG_CDECL_ALIASES (1u << 0)
#define NM_NOTE_V2_FLAG_LEVEL_SHIFT   4
#define NM_NOTE_V2_FLAG_LEVEL_MASK    (0xFu << NM_NOTE_V2_FLAG_LEVEL_SHIFT)
#ifdef __ASSEMBLER__
#  define NM_NOTE_V2_FLAG_LEVEL(n)    ((n) << NM_NOTE_V2_FLAG_LEVEL_SHIFT)
#else
#  define NM_NOTE_V2_FLAG_LEVEL(n)    (((uint32_t)(n) & 0xFu) << NM_NOTE_V2_FLAG_LEVEL_SHIFT)
#endif

#ifndef __ASSEMBLER__

#include "newminhh_cstd.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

NM_ASSUME_NONNULL_BEGIN

typedef enum : int32_t {
    NM_OK          =  0,
    NM_ERR_ELF     = -1,
    NM_ERR_INSN    = -2,
    NM_ERR_SYSCALL = -3,
    NM_ERR_DLSYM   = -4,
    NM_ERR_ABI     = -5
} nm_err_t;

typedef enum : uint32_t {
    NM_REG_EAX = 0,
    NM_REG_ECX = 1,
    NM_REG_EDX = 2,
    NM_REG_EBX = 3,
    NM_REG_ESP = 4,
    NM_REG_EBP = 5,
    NM_REG_ESI = 6,
    NM_REG_EDI = 7
} nm_reg_idx_t;

typedef struct NM_ALIGNED(16) {
    alignas(16) uint32_t gpr[8];            /* EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI */
    uint32_t eip;
    uint32_t eflags;
    uint32_t gs_base;
    uint32_t fs_base;
    uint32_t tls_bases[3];      /* GDT entries 6, 7, 8 */
    uint32_t exit_reason;
    uint32_t exit_arg;
    uint32_t _pad[3];
    alignas(16) uint8_t  fxsave_area[512];
} newminhh_cpu_t;

static_assert(sizeof(newminhh_cpu_t) == NM_CPU_SIZE, "newminhh_cpu_t size mismatch");
static_assert(alignof(newminhh_cpu_t) == 16, "newminhh_cpu_t align mismatch");
static_assert(offsetof(newminhh_cpu_t, eip) == NM_CPU_OFF_EIP, "cpu.eip offset mismatch");
static_assert(offsetof(newminhh_cpu_t, eflags) == NM_CPU_OFF_EFLAGS, "cpu.eflags offset mismatch");
static_assert(offsetof(newminhh_cpu_t, gs_base) == NM_CPU_OFF_GS_BASE, "cpu.gs_base offset mismatch");
static_assert(offsetof(newminhh_cpu_t, fs_base) == NM_CPU_OFF_FS_BASE, "cpu.fs_base offset mismatch");
static_assert(offsetof(newminhh_cpu_t, tls_bases) == NM_CPU_OFF_TLS_BASES, "cpu.tls_bases offset mismatch");
static_assert(offsetof(newminhh_cpu_t, exit_reason) == NM_CPU_OFF_EXIT_REASON, "cpu.exit_reason offset mismatch");
static_assert(offsetof(newminhh_cpu_t, exit_arg) == NM_CPU_OFF_EXIT_ARG, "cpu.exit_arg offset mismatch");
static_assert(offsetof(newminhh_cpu_t, fxsave_area) == NM_CPU_OFF_FXSAVE, "cpu.fxsave_area offset mismatch");

/* --- i386 ABI struct layouts --- */

typedef struct {
    int32_t tv_sec;
    int32_t tv_nsec;
} newminhh_timespec32_t;

static_assert(sizeof(newminhh_timespec32_t) == 8, "newminhh_timespec32_t must be 8 bytes");
static_assert(alignof(newminhh_timespec32_t) == 4, "newminhh_timespec32_t align must be 4");

typedef struct NM_PACKED NM_ALIGNED(4) {
    int64_t tv_sec;
    int32_t tv_nsec;
    int32_t _pad;
} newminhh_timespec64_t32_t;

static_assert(sizeof(newminhh_timespec64_t32_t) == 16, "newminhh_timespec64_t32_t must be 16 bytes");
static_assert(alignof(newminhh_timespec64_t32_t) == 4, "newminhh_timespec64_t32_t align must be 4");

typedef struct {
    uint32_t rlim_cur;
    uint32_t rlim_max;
} newminhh_rlimit32_t;

static_assert(sizeof(newminhh_rlimit32_t) == 8, "newminhh_rlimit32_t must be 8 bytes");
static_assert(alignof(newminhh_rlimit32_t) == 4, "newminhh_rlimit32_t align must be 4");

typedef struct NM_PACKED NM_ALIGNED(4) {
    uint64_t rlim_cur;
    uint64_t rlim_max;
} newminhh_rlimit64_t32_t;

static_assert(sizeof(newminhh_rlimit64_t32_t) == 16, "newminhh_rlimit64_t32_t must be 16 bytes");
static_assert(alignof(newminhh_rlimit64_t32_t) == 4, "newminhh_rlimit64_t32_t align must be 4");

/* Kernel i386 old_stat (syscalls 106/107/108) — 64 bytes */
typedef struct {
    uint32_t st_dev;
    uint32_t st_ino;
    uint16_t st_mode;
    uint16_t st_nlink;
    uint16_t st_uid;
    uint16_t st_gid;
    uint32_t st_rdev;
    uint32_t st_size;
    uint32_t st_blksize;
    uint32_t st_blocks;
    uint32_t st_atime_sec;
    uint32_t st_atime_nsec;
    uint32_t st_mtime_sec;
    uint32_t st_mtime_nsec;
    uint32_t st_ctime_sec;
    uint32_t st_ctime_nsec;
    uint32_t __unused4;
    uint32_t __unused5;
} newminhh_kstat32_t;

static_assert(sizeof(newminhh_kstat32_t) == 64, "newminhh_kstat32_t must be 64 bytes");

/* glibc i386 default struct stat (32-bit off_t/ino_t/time_t) — 88 bytes */
typedef struct {
    uint64_t st_dev;
    uint16_t __pad1;
    uint16_t __pad1_hi;
    uint32_t st_ino;
    uint32_t st_mode;
    uint32_t st_nlink;
    uint32_t st_uid;
    uint32_t st_gid;
    uint64_t st_rdev;
    uint16_t __pad2;
    uint16_t __pad2_hi;
    int32_t  st_size;
    int32_t  st_blksize;
    int32_t  st_blocks;
    newminhh_timespec32_t st_atim;
    newminhh_timespec32_t st_mtim;
    newminhh_timespec32_t st_ctim;
    uint32_t __glibc_reserved[2];
} newminhh_stat32_t;

static_assert(sizeof(newminhh_stat32_t) == 88, "newminhh_stat32_t must be 88 bytes");
static_assert(alignof(newminhh_stat32_t) <= 8, "newminhh_stat32_t align");

/* glibc i386 struct stat64 / kernel stat64 (_FILE_OFFSET_BITS=64, 32-bit time) — 96 bytes */
typedef struct NM_PACKED NM_ALIGNED(4) {
    uint64_t st_dev;
    uint8_t  __pad1[4];
    uint32_t __st_ino;
    uint32_t st_mode;
    uint32_t st_nlink;
    uint32_t st_uid;
    uint32_t st_gid;
    uint64_t st_rdev;
    uint8_t  __pad2[4];
    int64_t  st_size;
    int32_t  st_blksize;
    int64_t  st_blocks;
    newminhh_timespec32_t st_atim;
    newminhh_timespec32_t st_mtim;
    newminhh_timespec32_t st_ctim;
    uint64_t st_ino;
} newminhh_stat64_t;

static_assert(sizeof(newminhh_stat64_t) == 96, "newminhh_stat64_t must be 96 bytes");
static_assert(alignof(newminhh_stat64_t) == 4, "newminhh_stat64_t align must be 4");

/* glibc i386 _FILE_OFFSET_BITS=64 + _TIME_BITS=64 struct stat — 108 bytes */
typedef struct NM_PACKED NM_ALIGNED(4) {
    uint64_t st_dev;
    uint64_t st_ino;
    uint32_t st_mode;
    uint32_t st_nlink;
    uint32_t st_uid;
    uint32_t st_gid;
    uint64_t st_rdev;
    int64_t  st_size;
    int32_t  st_blksize;
    int64_t  st_blocks;
    newminhh_timespec64_t32_t st_atim;
    newminhh_timespec64_t32_t st_mtim;
    newminhh_timespec64_t32_t st_ctim;
} newminhh_stat64_t64_t;

static_assert(sizeof(newminhh_stat64_t64_t) == 108, "newminhh_stat64_t64_t must be 108 bytes");
static_assert(alignof(newminhh_stat64_t64_t) == 4, "newminhh_stat64_t64_t align must be 4");

/* i386 set_thread_area user_desc — 16 bytes */
typedef struct {
    uint32_t entry_number;
    uint32_t base_addr;
    uint32_t limit;
    uint32_t flags;
} newminhh_user_desc32_t;

static_assert(sizeof(newminhh_user_desc32_t) == 16, "newminhh_user_desc32_t must be 16 bytes");

/* i386 old_mmap (syscall 90) arg struct — 24 bytes */
typedef struct {
    uint32_t addr;
    uint32_t len;
    uint32_t prot;
    uint32_t flags;
    int32_t  fd;
    uint32_t offset;
} newminhh_mmap_arg_struct32_t;

static_assert(sizeof(newminhh_mmap_arg_struct32_t) == 24, "newminhh_mmap_arg_struct32_t must be 24 bytes");

/* i386 Linux struct sigaction userspace layout (kernel arch/x86 i386 ABI).
 * compat_callbacks.c repacks between host glibc/musl sigactions and this form. */
typedef struct {
    uint32_t sa_handler_va;
    uint32_t sa_flags;
    uint32_t sa_restorer_va;
    uint32_t sa_mask[2];
} newminhh_sigaction32_t;

static_assert(sizeof(newminhh_sigaction32_t) == 20, "newminhh_sigaction32_t must be 20 bytes");

/* --- Signature descriptors & symbol registry --- */

typedef struct {
    uint8_t ret_kind;
    uint8_t nargs;
    uint8_t is_variadic;
    uint8_t abi_ext;                /* NM_SIG_EXT_*; 0 = legacy behaviour */
    uint8_t arg_kinds[NM_MAX_SIG_ARGS];
} nm_fn_sig_t;

static_assert(sizeof(nm_fn_sig_t) == 20, "nm_fn_sig_t layout is embedded in patched binaries");

typedef struct {
    const char *NM_NONNULL_PTR name;
    void       *NM_NULLABLE_PTR wrapper_fn;
    nm_fn_sig_t sig;
    uint32_t    flags;
} nm_sym_entry_t;

/* --- Part 3 / Part 4 Integration Callback Interfaces --- */

typedef struct {
    void *NM_NULLABLE_PTR (* NM_NULLABLE_PTR dlopen_fn)(const char *path, int flags);
    void *NM_NULLABLE_PTR (* NM_NULLABLE_PTR dlsym_fn)(void *handle, const char *symbol);
    int   (* NM_NULLABLE_PTR dlclose_fn)(void *handle);
    const char *NM_NULLABLE_PTR (* NM_NULLABLE_PTR dlerror_fn)(void);
    int   (* NM_NULLABLE_PTR is_guest_elf_fn)(const char *path);
} nm_dl_backend_t;

typedef int (*nm_guest_invoker_t)(uint32_t guest_fn_va,
                                  const nm_fn_sig_t *sig,
                                  const uint32_t *cdecl_words,
                                  int nwords,
                                  uint64_t *out_gpr,
                                  double *out_fp);

typedef uint32_t (*nm_brk_hook_t)(uint32_t requested_brk);

void newminhh_register_dl_backend(const nm_dl_backend_t *NM_NULLABLE_PTR backend);
void newminhh_register_guest_invoker(nm_guest_invoker_t NM_NULLABLE_PTR invoker);
[[nodiscard]] nm_guest_invoker_t NM_NULLABLE_PTR newminhh_get_guest_invoker(void);
void newminhh_register_brk_hook(nm_brk_hook_t NM_NULLABLE_PTR brk_hook);
void newminhh_register_native_cdecl_range(uint32_t start_va, uint32_t end_va);
void newminhh_unregister_native_cdecl_range(uint32_t start_va, uint32_t end_va);
[[nodiscard]] int  newminhh_is_native_cdecl_va(uint32_t va);
[[nodiscard]] int  newminhh_mode_for_va(uint32_t guest_fn_va);

/* --- Calling convention bridges (compat_asm.S) --- */

uint64_t newminhh_call_x32_sig(void *target_x32_fn,
                               const nm_fn_sig_t *sig,
                               const uint32_t *i386_cdecl_args,
                               uint64_t *NM_NULLABLE_PTR out_fp_bits);

uint64_t newminhh_call_x32_cdecl(void *target_x32_fn,
                                 const nm_fn_sig_t *sig,
                                 const uint32_t *NM_NULLABLE_PTR i386_cdecl_args);

uint64_t newminhh_in_bridge(uint32_t guest_fn_va,
                            const nm_fn_sig_t *NM_NULLABLE_PTR sig,
                            const uint64_t *NM_NULLABLE_PTR x32_gprs,
                            const uint64_t *NM_NULLABLE_PTR x32_xmms,
                            const uint64_t *NM_NULLABLE_PTR x32_stack,
                            int mode,
                            double *NM_NULLABLE_PTR out_fp_ret);

/* --- Syscall translation (compat_syscall.c) --- */

int32_t newminhh_compat_syscall(uint32_t nr, uint32_t a1, uint32_t a2,
                                uint32_t a3, uint32_t a4, uint32_t a5,
                                uint32_t a6);
int32_t newminhh_compat_syscall_cpu(newminhh_cpu_t *cpu);
[[nodiscard]] uint32_t newminhh_compat_get_tls_base(uint32_t selector_or_idx);
void newminhh_compat_set_gs_sel(uint32_t selector);

/* --- Struct repacking (compat_structs.c) --- */

int newminhh_repack_stat_to_stat32(NM_NOESCAPE const struct stat *src, newminhh_stat32_t *dst);
int newminhh_repack_stat_to_stat64(NM_NOESCAPE const struct stat *src, newminhh_stat64_t *dst);
int newminhh_repack_stat_to_stat64_t64(NM_NOESCAPE const struct stat *src, newminhh_stat64_t64_t *dst);
int newminhh_repack_stat_to_kstat32(NM_NOESCAPE const struct stat *src, newminhh_kstat32_t *dst);

int newminhh_repack_timespec_to_32(NM_NOESCAPE const struct timespec *src, newminhh_timespec32_t *dst);
int newminhh_repack_timespec_from_32(NM_NOESCAPE const newminhh_timespec32_t *src, struct timespec *dst);
int newminhh_repack_timespec_to_64t32(NM_NOESCAPE const struct timespec *src, newminhh_timespec64_t32_t *dst);
int newminhh_repack_timespec_from_64t32(NM_NOESCAPE const newminhh_timespec64_t32_t *src, struct timespec *dst);

int newminhh_repack_rlimit_to_32(NM_NOESCAPE const struct rlimit *src, newminhh_rlimit32_t *dst);
int newminhh_repack_rlimit_from_32(NM_NOESCAPE const newminhh_rlimit32_t *src, struct rlimit *dst);
int newminhh_repack_rlimit_to_64t32(NM_NOESCAPE const struct rlimit *src, newminhh_rlimit64_t32_t *dst);
int newminhh_repack_rlimit_from_64t32(NM_NOESCAPE const newminhh_rlimit64_t32_t *src, struct rlimit *dst);

/* Fix up ET_EXEC R_386_COPY slots for environ/optarg/optind/opterr/optopt after ctor init. */
void newminhh_compat_fixup_copy_data(void *NM_NULLABLE_PTR environ_slot,
                                     void *NM_NULLABLE_PTR optarg_slot,
                                     void *NM_NULLABLE_PTR optind_slot,
                                     void *NM_NULLABLE_PTR opterr_slot,
                                     void *NM_NULLABLE_PTR optopt_slot);

/* --- Callback pool & lifecycle wrappers (compat_callbacks.c) --- */

void *NM_NULLABLE_PTR newminhh_alloc_cb_trampoline(uint32_t guest_fn_va,
                                                   const nm_fn_sig_t *NM_NULLABLE_PTR sig,
                                                   int mode,
                                                   uint32_t flags);
void *NM_NULLABLE_PTR newminhh_alloc_out_trampoline(void *target_x32_fn, const nm_fn_sig_t *sig);
void *NM_NULLABLE_PTR newminhh_alloc_out_trampoline_ex(void *target_x32_fn, const nm_fn_sig_t *sig, uint32_t flags);
void *NM_NULLABLE_PTR newminhh_wrap_init_array_fn(uint32_t guest_fn_va, int mode);
void *NM_NULLABLE_PTR newminhh_wrap_fini_array_fn(uint32_t guest_fn_va, int mode);
void *NM_NULLABLE_PTR newminhh_wrap_so_export(uint32_t guest_fn_va, const nm_fn_sig_t *sig, int mode);

void compat_qsort(void *base, uint32_t nmemb, uint32_t size, uint32_t guest_compar_va);
void *NM_NULLABLE_PTR compat_bsearch(const void *key, const void *base, uint32_t nmemb,
                                     uint32_t size, uint32_t guest_compar_va);
int compat_pthread_create(uint32_t *thread_out, const void *NM_NULLABLE_PTR attr,
                          uint32_t start_routine_va, uint32_t arg);
int compat_pthread_join(uint32_t thread, void *NM_NULLABLE_PTR *NM_NULLABLE_PTR retval);
int compat_pthread_detach(uint32_t thread);
uint32_t compat_pthread_self(void);
[[noreturn]] void compat_pthread_exit(void *NM_NULLABLE_PTR retval);
int compat_pthread_mutex_init(void *guest_mutex, const void *NM_NULLABLE_PTR attr);
int compat_pthread_mutex_destroy(void *guest_mutex);
int compat_pthread_mutex_lock(void *guest_mutex);
int compat_pthread_mutex_trylock(void *guest_mutex);
int compat_pthread_mutex_unlock(void *guest_mutex);
int compat_atexit(uint32_t fn_va);
int compat___cxa_atexit(uint32_t fn_va, uint32_t arg, uint32_t dso_handle);
void compat___cxa_finalize(uint32_t dso_handle);
void newminhh_run_guest_atexits(void);
uint32_t compat_signal(int signum, uint32_t handler_va);
int compat_sigaction(int signum, const void *NM_NULLABLE_PTR act, void *NM_NULLABLE_PTR oldact);

int compat_printf_cdecl(const uint32_t *cdecl_args);
int compat_fprintf_cdecl(const uint32_t *cdecl_args);
int compat_sprintf_cdecl(const uint32_t *cdecl_args);
int compat_snprintf_cdecl(const uint32_t *cdecl_args);
int compat_vprintf(const char *fmt, va_list ap);
int compat_vfprintf(FILE *fp, const char *fmt, va_list ap);
int compat_vsprintf(char *str, const char *fmt, va_list ap);
int compat_vsnprintf(char *str, uint32_t size, const char *fmt, va_list ap);

/* --- Dynamic linking & wrapper lookup (compat_dl.c / compat_wrappers.c) --- */

void *NM_NULLABLE_PTR compat_dlopen(const char *filename, int flags);
void *NM_NULLABLE_PTR compat_dlsym(void *handle, const char *symbol);
void *NM_NULLABLE_PTR compat_dlsym_cdecl(void *handle, const char *symbol);
const char *NM_NULLABLE_PTR compat_dlerror(void);
int compat_dlclose(void *handle);

int newminhh_compat_lookup_wrapper(const char *name, nm_sym_entry_t *out);
int newminhh_lookup_resolve(const char *NM_NULLABLE_PTR soname,
                            const char *NM_NONNULL_PTR sym,
                            const char *NM_NULLABLE_PTR ver,
                            nm_sym_entry_t *NM_NONNULL_PTR out,
                            void *NM_NULLABLE_PTR *NM_NULLABLE_PTR native_handle);
int newminhh_lookup_classify_lib(const char *NM_NONNULL_PTR soname,
                                 char *NM_NULLABLE_PTR tgt_out,
                                 size_t tgt_sz);
void newminhh_get_stdio_streams(FILE *NM_NULLABLE_PTR *NM_NULLABLE_PTR out_stdin,
                                FILE *NM_NULLABLE_PTR *NM_NULLABLE_PTR out_stdout,
                                FILE *NM_NULLABLE_PTR *NM_NULLABLE_PTR out_stderr);

/* --- C11 / C23 Guest Libc Wrappers (compat_wrappers.c) --- */

void *NM_NULLABLE_PTR compat_aligned_alloc(uint32_t alignment, uint32_t size);
int compat_timespec_get(newminhh_timespec32_t *NM_NULLABLE_PTR ts32, int base);
[[noreturn]] void compat_quick_exit(int status);
char *NM_NULLABLE_PTR compat_strndup(const char *NM_NONNULL_PTR s, uint32_t n);
void *NM_NULLABLE_PTR compat_memchr(const void *NM_NONNULL_PTR s, int c, uint32_t n);
void *NM_NULLABLE_PTR compat_memccpy(void *NM_NONNULL_PTR dst, const void *NM_NONNULL_PTR src, int c, uint32_t n);
void *NM_NULLABLE_PTR compat_memset_explicit(void *NM_NONNULL_PTR s, int c, uint32_t n);
void compat_free_sized(void *NM_NULLABLE_PTR p, uint32_t sz);
void compat_free_aligned_sized(void *NM_NULLABLE_PTR p, uint32_t align, uint32_t sz);

/* --- Numeric conversion wrappers (compat_wrappers.c) --- */
int32_t compat_strtol(const char *NM_NONNULL_PTR nptr, char *NM_NULLABLE_PTR *NM_NULLABLE_PTR endptr, int base);
uint32_t compat_strtoul(const char *NM_NONNULL_PTR nptr, char *NM_NULLABLE_PTR *NM_NULLABLE_PTR endptr, int base);
int64_t compat_strtoll(const char *NM_NONNULL_PTR nptr, char *NM_NULLABLE_PTR *NM_NULLABLE_PTR endptr, int base);
uint64_t compat_strtoull(const char *NM_NONNULL_PTR nptr, char *NM_NULLABLE_PTR *NM_NULLABLE_PTR endptr, int base);
int32_t compat___isoc23_strtol(const char *NM_NONNULL_PTR nptr, char *NM_NULLABLE_PTR *NM_NULLABLE_PTR endptr, int base);
uint32_t compat___isoc23_strtoul(const char *NM_NONNULL_PTR nptr, char *NM_NULLABLE_PTR *NM_NULLABLE_PTR endptr, int base);
int64_t compat___isoc23_strtoll(const char *NM_NONNULL_PTR nptr, char *NM_NULLABLE_PTR *NM_NULLABLE_PTR endptr, int base);
uint64_t compat___isoc23_strtoull(const char *NM_NONNULL_PTR nptr, char *NM_NULLABLE_PTR *NM_NULLABLE_PTR endptr, int base);

NM_ASSUME_NONNULL_END

#ifdef __cplusplus
}
#endif

#endif /* !__ASSEMBLER__ */

#endif /* NEWMINHH_ABI_H */
