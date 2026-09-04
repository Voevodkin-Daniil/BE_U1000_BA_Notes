/****************************************************************************
 *
 *  RISC-V "P" extension proposal intrinsics and emulation
 *
 *  Copyright (C) 2021  Mark Fedorov <mark.fedorov@cloudbear.ru>
 *
 *  Permission to use, copy, modify, and/or distribute this software for any
 *  purpose with or without fee is hereby granted, provided that the above
 *  copyright notice and this permission notice appear in all copies.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 ****************************************************************************/

#ifndef __RVZPSFOPERANDINTRIN_H__
#define __RVZPSFOPERANDINTRIN_H__

#ifndef __RVINTRIN_H__
#error Never use <rvzpsfoperandintrin.h> directly; include <rvintrin.h> instead.
#endif

#ifndef __RVZPNINTRIN_H__
#error <rvzpsfoperandintrin.h> requires <rvzpnintrin.h>.
#endif

#ifdef __riscv_zpsfoperand
#if __riscv_xlen == 32
#define DECLARE_INTRIN_RR(_insn_, insn, rd_t, rs1_t, rs2_t) static inline rd_t __rv_ ## _insn_ (rs1_t rs1, rs2_t rs2) { rd_t rd; __asm__(#insn " %0, %1, %2" : "=Z0,Z0,Z0,Z0,Z1,Z1,Z1,Z1"(rd) : "Z0,Z0,Z1,Z1,Z0,Z0,Z1,Z1"(rs1), "Z0,Z1,Z0,Z1,Z0,Z1,Z0,Z1"(rs2)); return rd; }
#define DECLARE_INTRIN_VOLATILE_RR(_insn_, insn, rd_t, rs1_t, rs2_t) static inline rd_t __rv_ ## _insn_ (rs1_t rs1, rs2_t rs2) { rd_t rd; __asm__ volatile(#insn " %0, %1, %2" : "=Z0,Z0,Z0,Z0,Z1,Z1,Z1,Z1"(rd) : "Z0,Z0,Z1,Z1,Z0,Z0,Z1,Z1"(rs1), "Z0,Z1,Z0,Z1,Z0,Z1,Z0,Z1"(rs2)); return rd; }
#define DECLARE_INTRIN_RI_(_insn_, insn, tail, imm_mask, rd_t, rs1_t, rs2_t) static inline rd_t __rv_ ## _insn_ (rs1_t rs1, rs2_t rs2) { rd_t rd; if (__builtin_constant_p(rs2)) __asm__(#insn "i" #tail " %0, %1, %2" : "=r,r"(rd) : "Z0,Z1"(rs1), "i,i"(rs2 & (imm_mask))); else __asm__(#insn #tail " %0, %1, %2" : "=r,r"(rd) : "Z0,Z1"(rs1), "r,r"(rs2)); return rd; }
#define DECLARE_INTRIN_RI(_insn_, insn, imm_mask, rd_t, rs1_t, rs2_t) DECLARE_INTRIN_RI_(_insn_, insn,, imm_mask, rd_t, rs1_t, rs2_t)
#define DECLARE_INTRIN_RR_DESTRUCTIVE(_insn_, insn, rd_t, rs1_t, rs2_t) static inline rd_t __rv_ ## _insn_ (rd_t rd, rs1_t rs1, rs2_t rs2) { __asm__(#insn " %0, %1, %2" : "+Z0,Z1"(rd) : "r,r"(rs1), "r,r"(rs2)); return rd; }
#define DECLARE_INTRIN_VOLATILE_RR_DESTRUCTIVE(_insn_, insn, rd_t, rs1_t, rs2_t) static inline rd_t __rv_ ## _insn_ (rd_t rd, rs1_t rs1, rs2_t rs2) { __asm__ volatile(#insn " %0, %1, %2" : "+Z0,Z1"(rd) : "r,r"(rs1), "r,r"(rs2)); return rd; }
#define DECLARE_INTRIN_(_insn_, insn, MACRO, types...) DECLARE_INTRIN_##MACRO(_insn_, insn, types)
#define DECLARE_INTRIN(insn, MACRO, types...) DECLARE_INTRIN_(insn, insn, MACRO, types)
#else
#define DECLARE_INTRIN_RR(_insn_, insn, rd_t, rs1_t, rs2_t) static inline rd_t __rv_ ## _insn_ (rs1_t rs1, rs2_t rs2) { rd_t rd; __asm__(#insn " %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
#define DECLARE_INTRIN_VOLATILE_RR(_insn_, insn, rd_t, rs1_t, rs2_t) static inline rd_t __rv_ ## _insn_ (rs1_t rs1, rs2_t rs2) { rd_t rd; __asm__ volatile(#insn " %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
#define DECLARE_INTRIN_RI_(_insn_, insn, tail, imm_mask, rd_t, rs1_t, rs2_t) static inline rd_t __rv_ ## _insn_ (rs1_t rs1, rs2_t rs2) { rd_t rd; if (__builtin_constant_p(rs2)) __asm__(#insn "i" #tail " %0, %1, %2" : "=r"(rd) : "r"(rs1), "i"(rs2 & (imm_mask))); else __asm__(#insn #tail " %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
#define DECLARE_INTRIN_RI(_insn_, insn, imm_mask, rd_t, rs1_t, rs2_t) DECLARE_INTRIN_RI_(_insn_, insn,, imm_mask, rd_t, rs1_t, rs2_t)
#define DECLARE_INTRIN_RR_DESTRUCTIVE(_insn_, insn, rd_t, rs1_t, rs2_t) static inline rd_t __rv_ ## _insn_ (rd_t rd, rs1_t rs1, rs2_t rs2) { __asm__(#insn " %0, %1, %2" : "+r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
#define DECLARE_INTRIN_VOLATILE_RR_DESTRUCTIVE(_insn_, insn, rd_t, rs1_t, rs2_t) static inline rd_t __rv_ ## _insn_ (rd_t rd, rs1_t rs1, rs2_t rs2) { __asm__ volatile(#insn " %0, %1, %2" : "+r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
#define DECLARE_INTRIN_(_insn_, insn, MACRO, types...) DECLARE_INTRIN_##MACRO(_insn_, insn, types)
#define DECLARE_INTRIN(insn, MACRO, types...) DECLARE_INTRIN_(insn, insn, MACRO, types)
#endif

__attribute__((error("WEXT instruction has been removed. Consider FSR/FSRI/FSRW instructions")))
intXLEN_t __rv_wext(int64_t rs1, uint32_t rs2);

DECLARE_INTRIN_(sadd64, add64, RR, int64_t, int64_t, int64_t)
DECLARE_INTRIN_(uadd64, add64, RR, uint64_t, uint64_t, uint64_t)
DECLARE_INTRIN(kadd64, VOLATILE_RR, int64_t, int64_t, int64_t)
DECLARE_INTRIN(kmar64, VOLATILE_RR_DESTRUCTIVE, int64_t, intXLEN_t, intXLEN_t)
DECLARE_INTRIN(kmsr64, VOLATILE_RR_DESTRUCTIVE, int64_t, intXLEN_t, intXLEN_t)
DECLARE_INTRIN(ksub64, VOLATILE_RR, int64_t, int64_t, int64_t)
DECLARE_INTRIN(mulr64, RR, uint64_t, uint32_t, uint32_t)
DECLARE_INTRIN(mulsr64, RR, uint64_t, uint32_t, uint32_t)
DECLARE_INTRIN(radd64, RR, int64_t, int64_t, int64_t)
DECLARE_INTRIN(rsub64, RR, int64_t, int64_t, int64_t)
DECLARE_INTRIN(smal, RR, int64_t, int64_t, uintXLEN_t)
DECLARE_INTRIN(smalbb, RR_DESTRUCTIVE, int64_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(smalbt, RR_DESTRUCTIVE, int64_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(smaltt, RR_DESTRUCTIVE, int64_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(smalda, RR_DESTRUCTIVE, int64_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(smalxda, RR_DESTRUCTIVE, int64_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(smalds, RR_DESTRUCTIVE, int64_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(smaldrs, RR_DESTRUCTIVE, int64_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(smalxds, RR_DESTRUCTIVE, int64_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(smar64, RR_DESTRUCTIVE, int64_t, intXLEN_t, intXLEN_t)
DECLARE_INTRIN(smslda, RR_DESTRUCTIVE, int64_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(smslxda, RR_DESTRUCTIVE, int64_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(smsr64, RR_DESTRUCTIVE, int64_t, intXLEN_t, intXLEN_t)
DECLARE_INTRIN(smul8, RR, uint64_t, uint32_t, uint32_t)
DECLARE_INTRIN(smulx8, RR, uint64_t, uint32_t, uint32_t)
DECLARE_INTRIN(smul16, RR, uint64_t, uint32_t, uint32_t)
DECLARE_INTRIN(smulx16, RR, uint64_t, uint32_t, uint32_t)
DECLARE_INTRIN_(ssub64, sub64, RR, int64_t, int64_t, int64_t)
DECLARE_INTRIN_(usub64, sub64, RR, uint64_t, uint64_t, uint64_t)
DECLARE_INTRIN(ukadd64, VOLATILE_RR, uint64_t, uint64_t, uint64_t)
DECLARE_INTRIN(ukmar64, VOLATILE_RR_DESTRUCTIVE, uint64_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(ukmsr64, VOLATILE_RR_DESTRUCTIVE, uint64_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(uksub64, VOLATILE_RR, uint64_t, uint64_t, uint64_t)
DECLARE_INTRIN(umar64, RR_DESTRUCTIVE, uint64_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(umsr64, RR_DESTRUCTIVE, uint64_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(umul8, RR, uint64_t, uint32_t, uint32_t)
DECLARE_INTRIN(umulx8, RR, uint64_t, uint32_t, uint32_t)
DECLARE_INTRIN(umul16, RR, uint64_t, uint32_t, uint32_t)
DECLARE_INTRIN(umulx16, RR, uint64_t, uint32_t, uint32_t)
DECLARE_INTRIN(uradd64, RR, uint64_t, uint64_t, uint64_t)
DECLARE_INTRIN(ursub64, RR, uint64_t, uint64_t, uint64_t)

#if __riscv_xlen == 32
static inline uint32_t __rv_fsr(uint32_t rs1, uint32_t rs2, uint32_t rs3)
{
    uint32_t rd;
    if (__builtin_constant_p(rs2))
         __asm__("fsri %0, %1, %2, %3" : "=r"(rd) : "r"(rs1), "r"(rs3), "i"(rs2 & 0x3F));
    else
        __asm__("fsr %0, %1, %2, %3" : "=r"(rd) : "r"(rs1), "r"(rs3), "r"(rs2));
    return rd;
}
#else
static inline uint32_t __rv_fsrw(uint32_t rs1, uint32_t rs2, uint32_t rs3)
{
    uint32_t rd;
    __asm__("fsrw %0, %1, %2, %3" : "=r"(rd) : "r"(rs1), "r"(rs3), "r"(rs2));
    return rd;
}
#endif

#undef DECLARE_INTRIN_RR
#undef DECLARE_INTRIN_VOLATILE_RR
#undef DECLARE_INTRIN_RI_
#undef DECLARE_INTRIN_RI
#undef DECLARE_INTRIN_RR_DESTRUCTIVE
#undef DECLARE_INTRIN_VOLATILE_RR_DESTRUCTIVE
#undef DECLARE_INTRIN_
#undef DECLARE_INTRIN

#else // ^^^  __riscv_zpsfoperand ^^^ | vvv !__riscv_zpsfoperand vvv
// FIXME[dzakharov]: Issue: CB-2720:: update code and remove warnings suppression
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wshift-count-overflow"

static inline int64_t __rv_sadd64(int64_t rs1, int64_t rs2)
{
    return rs1 + rs2;
}

static inline uint64_t __rv_uadd64(uint64_t rs1, uint64_t rs2)
{
    return rs1 + rs2;
}

static inline int64_t __rv_ssub64(int64_t rs1, int64_t rs2)
{
    return rs1 - rs2;
}

static inline uint64_t __rv_usub64(uint64_t rs1, uint64_t rs2)
{
    return rs1 - rs2;
}

static inline int64_t __rv_kadd64(int64_t rs1, int64_t rs2)
{
    if (rs1 > 0 && rs2 > 0 && INT64_MAX - rs2 < rs1)
        return __rv_vxsat = 1, INT64_MAX;
    if (rs1 < 0 && rs2 < 0 && INT64_MIN - rs2 > rs1)
        return __rv_vxsat = 1, INT64_MIN;
    return rs1 + rs2;
}

static inline int64_t __rv_ksub64(int64_t rs1, int64_t rs2)
{
    if (rs1 == 0 && rs2 == INT64_MIN) return INT64_MIN;
    if (rs1 > 0 && rs2 < 0 && INT64_MAX + rs2 < rs1)
        return __rv_vxsat = 1, INT64_MAX;
    if (rs1 < 0 && rs2 > 0 && INT64_MIN + rs2 > rs1)
        return __rv_vxsat = 1, INT64_MIN;
    return rs1 - rs2;
}

static inline int64_t __rv_radd64(int64_t rs1, int64_t rs2)
{
    return ( (rs1 >> 1) + (rs2 >> 1) + ((rs1&1) && (rs2&1)) );
}

static inline int64_t __rv_rsub64(int64_t rs1, int64_t rs2)
{
    if (rs1 == INT64_MIN && rs2 == INT64_MIN) return INT64_MIN;
    if (rs2 == INT64_MIN) --rs1, ++rs2;
    rs2 = -rs2;
    return ( (rs1 >> 1) + (rs2 >> 1) + ((rs1&1) && (rs2&1)) );
}

static inline uint64_t __rv_ukadd64(uint64_t rs1, uint64_t rs2)
{
    if (UINT64_MAX - rs2 < rs1) return __rv_vxsat = 1, UINT64_MAX;
    return rs1 + rs2;
}

static inline uint64_t __rv_uksub64(uint64_t rs1, uint64_t rs2)
{
    if (rs2 > rs1) return __rv_vxsat = 1, 0;
    return rs1 - rs2;
}

static inline uint64_t __rv_uradd64(uint64_t rs1, uint64_t rs2)
{
    return ( (rs1 >> 1) + (rs2 >> 1) + ((rs1&1) && (rs2&1)) );
}

static inline uint64_t __rv_ursub64(uint64_t rs1, uint64_t rs2)
{
    uint64_t ans = __rv_uradd64(rs1, -rs2);
    // If rs1 == 0 and rs2 == 1, 0x00... + 0xFF... == 0xFF -- unsigned value
    // 0xFF... >> 1 == 0x7F... so we are 'losing' sign of -1 (not +0xFF...).
    // In other words, uradd 'can not' give a negative pre-answer while both of
    // its operands are non-negative (unsigned) values. On the other hand, in
    // terms of addition, the second operand of ursub is always non-positive
    // number, so 65-th bit of 'addition' could be 1 (despite of uradd case).
    // This check allows to keep this bit after shift.
    ans |= (uint64_t)(rs1 < rs2) << 63;
    // As opposed to the previous case, if let's say rs1 == 1 and rs1 == 1,
    // -1 treats as +0xFF... so 0x00..01 + 0xFF..FF = 0x100..00 -- and then
    // 0x100... >> 1 = 0x80... so we again are 'losing' sign of this -1 but
    // this time it should collapse in a 2s complement's overflow.
    // This check allows to provide the above logic properly.
    ans &= ~((uint64_t)(rs1 >= rs2) << 63);
    return ans;
}

#define ADD64_RD_RES_CHECK_OV                       \
    if (res > 0 && rd > 0 && INT64_MAX - res < rd)  \
        return __rv_vxsat = 1, INT64_MAX;           \
    if (res < 0 && rd < 0 && INT64_MIN - res > rd)  \
        return __rv_vxsat = 1, INT64_MIN;           \
    return res + rd

static inline int64_t __rv_kmar64(int64_t rd, intXLEN_t rs1, intXLEN_t rs2)
{
    if (rs1 == -0x8000000080000000 && rs2 == -0x8000000080000000)
    {
        if (rd >= 0) return __rv_vxsat = 1, INT64_MAX;
        else return (rd + 1) + INT64_MAX;
    }
    int64_t res = (int64_t)(int32_t)rs1 * (int32_t)rs2
#if __riscv_xlen == 64
        + (rs1 >> 32) * (rs2 >> 32)
#endif
    ; ADD64_RD_RES_CHECK_OV;
}

static inline int64_t __rv_kmsr64(int64_t rd, intXLEN_t rs1, intXLEN_t rs2)
{
    if (rs1 == -0x8000000080000000 && rs2 == -0x8000000080000000)
    {
        if (rd < 0) return __rv_vxsat = 1, INT64_MIN;
        else return rd + INT64_MIN;
    }
    int64_t res = (int64_t)(int32_t)rs1 * (int32_t)rs2
#if __riscv_xlen == 64
        + (rs1 >> 32) * (rs2 >> 32)
#endif
    ; res = -res; ADD64_RD_RES_CHECK_OV;
}

#undef ADD64_RD_RES_CHECK_OV // signed saturation helper

static inline uint64_t __rv_ukmar64(uint64_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    uint64_t m1 = (uint64_t)(rs1 & UINT32_MAX) * (rs2 & UINT32_MAX);
    uint64_t m2 = ((uint64_t)rs1 >> 32) * ((uint64_t)rs2 >> 32);
    if (m1 > UINT64_MAX - m2) {return __rv_vxsat = 1, UINT64_MAX;} m1 += m2;
    if (m1 > UINT64_MAX - rd) {return __rv_vxsat = 1, UINT64_MAX;} return m1 + rd;
}

static inline uint64_t __rv_ukmsr64(uint64_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    uint64_t m1 = (uint64_t)(rs1 & UINT32_MAX) * (rs2 & UINT32_MAX);
    uint64_t m2 = ((uint64_t)rs1 >> 32) * ((uint64_t)rs2 >> 32);
    if (m1 > UINT64_MAX - m2 || m1 + m2 > rd) return __rv_vxsat = 1, 0;
    return rd - (m1 + m2);
}

static inline uint64_t __rv_umar64(uint64_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    return rd + (__riscv_xlen == 64 ? (rs1 & UINT32_MAX) * (rs2 & UINT32_MAX) + (rs1 >> 32) * (rs2 >> 32) : (uint64_t)rs1 * rs2);
}

static inline uint64_t __rv_umsr64(uint64_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    return rd - (__riscv_xlen == 64 ? (rs1 & UINT32_MAX) * (rs2 & UINT32_MAX) + (rs1 >> 32) * (rs2 >> 32) : (uint64_t)rs1 * rs2);
}

static inline uint64_t __rv_mulr64(uint32_t rs1, uint32_t rs2)
{
    return (uint64_t)rs1 * rs2;
}

static inline int64_t __rv_mulsr64(int32_t rs1, int32_t rs2)
{
    return (int64_t)rs1 * rs2;
}

static inline int64_t __rv_smal(int64_t rs1, uintXLEN_t rs2)
{
    rs1 += (int64_t)(int16_t)(rs2) * (int16_t)(rs2 >> 16);
    rs2 >>= 16; rs2 >>= 16;
    rs1 += (int64_t)(int16_t)(rs2) * (int16_t)(rs2 >> 16);
    return rs1;
}

static inline int64_t __rv_smalbb(int64_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    int64_t res = (int64_t)(int16_t)rs1 * (int16_t)rs2;
    rs1 >>= 16; rs1 >>= 16; rs2 >>= 16; rs2 >>= 16;
    res += (int64_t)(int16_t)rs1 * (int16_t)rs2;
    return rd + (intXLEN_t)res;
}

static inline int64_t __rv_smalbt(int64_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    return __rv_smalbb(rd, rs1, rs2 >> 16);
}

static inline int64_t __rv_smaltt(int64_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    return __rv_smalbb(rd, rs1 >> 16, rs2 >> 16);
}

#define SMALD(BODY1, BODY2)                         \
    int64_t res1 = (BODY1), res2 = (BODY2);         \
    rs1 >>= 16; rs1 >>= 16; rs2 >>= 16; rs2 >>= 16; \
    res1 += (BODY1); res2 += (BODY2);               \
    return rd + res1 + res2

static inline int64_t __rv_smalda(int64_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    SMALD((int64_t)(int16_t)(rs1) * (int64_t)(int16_t)(rs2),
        (int64_t)(int16_t)(rs1 >> 16) * (int64_t)(int16_t)(rs2 >> 16));
}

static inline int64_t __rv_smalxda(int64_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    SMALD((int64_t)(int16_t)(rs1) * (int64_t)(int16_t)(rs2 >> 16),
        (int64_t)(int16_t)(rs1 >> 16) * (int64_t)(int16_t)(rs2));
}

static inline int64_t __rv_smalds(int64_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    SMALD(-(int64_t)(int16_t)(rs1) * (int64_t)(int16_t)(rs2),
        (int64_t)(int16_t)(rs1 >> 16) * (int64_t)(int16_t)(rs2 >> 16));
}

static inline int64_t __rv_smaldrs(int64_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    SMALD((int64_t)(int16_t)(rs1) * (int64_t)(int16_t)(rs2),
        -(int64_t)(int16_t)(rs1 >> 16) * (int64_t)(int16_t)(rs2 >> 16));
}

static inline int64_t __rv_smalxds(int64_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    SMALD(-(int64_t)(int16_t)(rs1) * (int64_t)(int16_t)(rs2 >> 16),
        (int64_t)(int16_t)(rs1 >> 16) * (int64_t)(int16_t)(rs2));
}

static inline int64_t __rv_smar64(int64_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    SMALD((int64_t)(int32_t)(rs1) * (int64_t)(int32_t)(rs2), 0);
}

static inline int64_t __rv_smslda(int64_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    SMALD(-(int64_t)(int16_t)(rs1) * (int64_t)(int16_t)(rs2),
        -(int64_t)(int16_t)(rs1 >> 16) * (int64_t)(int16_t)(rs2 >> 16));
}

static inline int64_t __rv_smslxda(int64_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    SMALD(-(int64_t)(int16_t)(rs1) * (int64_t)(int16_t)(rs2 >> 16),
        -(int64_t)(int16_t)(rs1 >> 16) * (int64_t)(int16_t)(rs2));
}

static inline int64_t __rv_smsr64(int64_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    SMALD(-(int64_t)(int32_t)(rs1) * (int64_t)(int32_t)(rs2), 0);
}

#undef SMALD

#define MUL(u, n)                                       \
    int64_t rd = 0;                                     \
    for (size_t i = 0; i < 32; i += n) {                \
        rd |= (((u##int64_t)(u##int##n##_t)(rs1 >> i)   \
              * (u##int64_t)(u##int##n##_t)(rs2 >> i))  \
             & ((1ull << 2 * n) - 1)) << (2 * i);       \
    }                                                   \
    return rd

static inline uint64_t __rv_smul8(uint32_t rs1, uint32_t rs2)
{
    MUL(, 8);
}

static inline uint64_t __rv_smulx8(uint32_t rs1, uint32_t rs2)
{
    rs2 = __rv_swap8(rs2); MUL(, 8);
}

static inline uint64_t __rv_smul16(uint32_t rs1, uint32_t rs2)
{
    MUL(, 16);
}

static inline uint64_t __rv_smulx16(uint32_t rs1, uint32_t rs2)
{
    rs2 = __rv_swap16(rs2); MUL(, 16);
}

static inline uint64_t __rv_umul8(uint32_t rs1, uint32_t rs2)
{
    MUL(u, 8);
}

static inline uint64_t __rv_umulx8(uint32_t rs1, uint32_t rs2)
{
    rs2 = __rv_swap8(rs2); MUL(u, 8);
}

static inline uint64_t __rv_umul16(uint32_t rs1, uint32_t rs2)
{
    MUL(u, 16);
}

static inline uint64_t __rv_umulx16(uint32_t rs1, uint32_t rs2)
{
    rs2 = __rv_swap16(rs2); MUL(u, 16);
}

#undef MUL

static inline uintXLEN_t __rv_wext(int64_t rs1, uint32_t rs2)
{
    rs2 &= 31;
    return (int32_t)(rs1 >> rs2);
}
// FIXME[dzakharov]: Issue: CB-2720:: update code and remove warnings suppression
#pragma GCC diagnostic pop

#endif // ^^^  !__riscv_zpsfoperand ^^^
#endif // __RVZPSFOPERANDINTRIN_H__
