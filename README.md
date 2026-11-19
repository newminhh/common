# newminhh/common

Shared C and GNU Assembler (`__ASSEMBLER__`) ABI header contract for the [`newminhh`](https://github.com/newminhh/newminhh) i386-on-x32 compatibility system.

Consumed by [`newminhh/compat`](https://github.com/newminhh/compat) (`libnewminhh_compat.so`) and [`newminhh/dbt`](https://github.com/newminhh/dbt) (`libnewminhh_dbt.so` / `newminhh-run`), and mirrored in [`newminhh/patcher`](https://github.com/newminhh/patcher).

---

## Files

| File | Description |
| :--- | :--- |
| `newminhh_abi.h` | Virtual CPU state (`newminhh_cpu_t`), exact byte offset macros for GAS assembly, exit reasons, i386/x32 ABI struct definitions with `_Static_assert` checks, signature descriptors, integration callback interfaces, and public `compat` API declarations |
| `newminhh_log.h` | Structured `stderr` logging macros (`NM_LOG_ERROR`, `NM_LOG_WARN`, `NM_LOG_INFO` gated on `NEWMINHH_DEBUG`) |

---

## Key Definitions

### 1. Virtual CPU State (`newminhh_cpu_t`, 592 bytes, 16-byte aligned)

Shared directly between C (`src/dbt/`, `src/compat/compat_syscall.c`) and GAS assembly (`src/dbt/dbt_trampolines.S`):

| Constant | Offset | Field | Description |
| :--- | :--- | :--- | :--- |
| `NM_CPU_OFF_GPR` / `EAX..EDI` | `0..28` | `uint32_t gpr[8]` | `EAX(0)=0`, `ECX(1)=4`, `EDX(2)=8`, `EBX(3)=12`, `ESP(4)=16`, `EBP(5)=20`, `ESI(6)=24`, `EDI(7)=28` |
| `NM_CPU_OFF_EIP` | `32` | `uint32_t eip` | Next guest instruction pointer |
| `NM_CPU_OFF_EFLAGS` | `36` | `uint32_t eflags` | Guest flags register |
| `NM_CPU_OFF_GS_BASE` | `40` | `uint32_t gs_base` | Active guest TLS `GS` base address |
| `NM_CPU_OFF_FS_BASE` | `44` | `uint32_t fs_base` | Guest `FS` base |
| `NM_CPU_OFF_TLS_BASES` | `48..59` | `uint32_t tls_bases[3]` | Virtualized GDT slots 6, 7, 8 (`set_thread_area`) |
| `NM_CPU_OFF_EXIT_REASON` | `60` | `uint32_t exit_reason` | `NM_EXIT_NONE(0)`, `SYSCALL(1)`, `COMPAT_CALL(2)`, `HLT(3)`, `UNSUPPORTED(4)`, `RET_TO_HOST(5)` |
| `NM_CPU_OFF_EXIT_ARG` | `64` | `uint32_t exit_arg` | Exit metadata (`stub_id` or branch patch site) |
| `NM_CPU_OFF_FXSAVE` | `80..591` | `uint8_t fxsave_area[512]` | 16-byte aligned x87/MMX/SSE `fxsave` state |

### 2. i386 ABI Struct Layouts

Compile-time verified with `_Static_assert` for exact size and 4-byte i386 alignment:

- `newminhh_timespec32_t` (8B, align 4) / `newminhh_timespec64_t32_t` (16B, packed, align 4)
- `newminhh_rlimit32_t` (8B, align 4) / `newminhh_rlimit64_t32_t` (16B, packed, align 4)
- `newminhh_kstat32_t` (64B) — legacy kernel `old_stat`
- `newminhh_stat32_t` (88B) — glibc i386 default `struct stat`
- `newminhh_stat64_t` (96B, packed, align 4) — `_FILE_OFFSET_BITS=64` with 32-bit `timespec`
- `newminhh_stat64_t64_t` (108B, packed, align 4) — `_FILE_OFFSET_BITS=64` + `_TIME_BITS=64`
- `newminhh_user_desc32_t` (16B) — `set_thread_area` descriptor
- `newminhh_mmap_arg_struct32_t` (24B) — `old_mmap` (syscall 90) argument block

### 3. Signature Descriptors, Flags & Integration Hooks

- `nm_fn_sig_t` (`NM_MAX_SIG_ARGS = 16`): encodes `ret_kind`, `nargs`, `is_variadic`, and `arg_kinds[]` using `NM_ARG_*` / `NM_RET_*` (`VOID/NONE=0`, `I32=1`, `U32=2`, `PTR=3`, `I64=4`, `U64=5`, `F32=6`, `F64=7`).
- `nm_sym_entry_t`: pairs symbol name and wrapper pointer with `nm_fn_sig_t` and flags (`NM_SYM_FLAG_DATA`, `NM_SYM_FLAG_VARIADIC`, `NM_SYM_FLAG_CDECL_DIRECT`).
- Runtime hooks registered by `libnewminhh_dbt.so` or statically patched modules:
  - `newminhh_register_dl_backend(const nm_dl_backend_t *)`
  - `newminhh_register_guest_invoker(nm_guest_invoker_t)` / `newminhh_get_guest_invoker(void)`
  - `newminhh_register_brk_hook(nm_brk_hook_t)`
  - `newminhh_register_native_cdecl_range` / `unregister` / `newminhh_is_native_cdecl_va` / `newminhh_mode_for_va`

### 4. Structured Logging (`newminhh_log.h`)

- `NM_LOG_ERROR(mod, fmt, ...)` — Always prints `[newminhh:<mod>] ERROR: ...` to `stderr`
- `NM_LOG_WARN(mod, fmt, ...)` — Always prints `[newminhh:<mod>] WARN: ...` to `stderr`
- `NM_LOG_INFO(mod, fmt, ...)` — Emits `[newminhh:<mod>] INFO: ...` to `stderr` only when `NEWMINHH_DEBUG` is set and non-zero

---

## Usage

`common` is header-only; compile dependents on an x32 toolchain with `-I<path-to-common>`.

See [`newminhh/docs`](https://github.com/newminhh/docs/blob/main/ARCHITECTURE.md) for the complete system architecture.
