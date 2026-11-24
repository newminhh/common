#ifndef NEWMINHH_LOOKUP_H
#define NEWMINHH_LOOKUP_H

#ifndef __ASSEMBLER__

#include "newminhh_cstd.h"
#include "newminhh_abi.h"

#ifdef __cplusplus
extern "C" {
#endif

NM_ASSUME_NONNULL_BEGIN

/* --- Lookup table / rules API (compat: future lookup.c) --- */

#define NM_LOOKUP_DEFAULT_CACHE "/var/cache/newminhh/lookup_table.txt"
#define NM_LOOKUP_DEFAULT_RULES "/usr/share/newminhh/rules.d"
#define NM_LOOKUP_HEADER_MAGIC  "#!newminhh-lookup v1"

typedef enum : uint8_t {
    NM_LK_NONE = 0,
    NM_LK_COMPAT = 1,
    NM_LK_NATIVE_TYPED = 2,
    NM_LK_NATIVE_DATA = 3,
    NM_LK_PATCHED_CDECL = 4,
    NM_LK_ALIAS = 5,
    NM_LK_UNTYPED = 6,
    NM_LK_FORBIDDEN = 7,
    NM_LK_NATIVE_LIB = 8,
} nm_lk_class_t;

typedef struct {
    nm_lk_class_t cls;
    uint32_t      flags;
    nm_fn_sig_t   sig;
    uint32_t      size;
    const char   *NM_NULLABLE_PTR tgt_soname;
    const char   *NM_NULLABLE_PTR tgt_sym;
    const char   *NM_NULLABLE_PTR tgt_ver;
    const char   *NM_NULLABLE_PTR reason;
} nm_lk_entry_t;

typedef struct nm_lookup nm_lookup_t;

[[nodiscard]] int nm_lookup_open(const char *NM_NONNULL_PTR path,
                                 nm_lookup_t *NM_NULLABLE_PTR *NM_NONNULL_PTR out);
[[nodiscard]] int nm_lookup_open_mem(const char *NM_NONNULL_PTR data, size_t len,
                                      nm_lookup_t *NM_NULLABLE_PTR *NM_NONNULL_PTR out);
[[nodiscard]] int nm_lookup_find(const nm_lookup_t *NM_NONNULL_PTR lk,
                                 const char *NM_NULLABLE_PTR soname,
                                 const char *NM_NONNULL_PTR sym,
                                 const char *NM_NULLABLE_PTR ver,
                                 uint32_t mode_mask,
                                 nm_lk_entry_t *NM_NONNULL_PTR out);
[[nodiscard]] int nm_lookup_find_lib(const nm_lookup_t *NM_NONNULL_PTR lk,
                                     const char *NM_NONNULL_PTR soname,
                                     nm_lk_entry_t *NM_NONNULL_PTR out,
                                     const char *NM_NULLABLE_PTR *NM_NULLABLE_PTR path_out);
const char *NM_NULLABLE_PTR nm_lookup_interp(const nm_lookup_t *NM_NONNULL_PTR lk);
const char *NM_NULLABLE_PTR nm_lookup_libc(const nm_lookup_t *NM_NONNULL_PTR lk);
int nm_lookup_is_stale(const nm_lookup_t *NM_NONNULL_PTR lk);
void nm_lookup_close(nm_lookup_t *NM_NULLABLE_PTR lk);

/* --- Signature helpers (parsing/formatting for lookup rules) --- */

[[nodiscard]] int nm_sig_parse(const char *NM_NONNULL_PTR s, nm_fn_sig_t *NM_NONNULL_PTR out,
                               uint32_t *NM_NULLABLE_PTR out_flags);
int nm_sig_format(const nm_fn_sig_t *NM_NONNULL_PTR sig, uint32_t flags,
                  char *NM_NONNULL_PTR buf, size_t buf_sz);

NM_ASSUME_NONNULL_END

#ifdef __cplusplus
}
#endif

#endif /* !__ASSEMBLER__ */

#endif /* NEWMINHH_LOOKUP_H */
