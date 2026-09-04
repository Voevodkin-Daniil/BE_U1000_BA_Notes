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

#ifndef __RVZPNINTRIN_H__
#define __RVZPNINTRIN_H__

#ifndef __RVINTRIN_H__
#error Never use <rvzpnintrin.h> directly; include <rvintrin.h> instead.
#endif

#include <stdint.h>
#include <stddef.h>

#define EVAL(MACRO, args...) MACRO(args)
#define TYPEDEF(what, length) typedef what##length##_t what##XLEN_t;
EVAL(TYPEDEF, int, __riscv_xlen)
EVAL(TYPEDEF, uint, __riscv_xlen)
#undef TYPEDEF
#undef EVAL

#ifdef __riscv_zpn
#define DECLARE_INTRIN_R(_insn_, insn, rd_t, rs1_t) static inline rd_t __rv_ ## _insn_ (rs1_t rs1) { rd_t rd; __asm__(#insn " %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }
#define DECLARE_INTRIN_RR(_insn_, insn, rd_t, rs1_t, rs2_t) static inline rd_t __rv_ ## _insn_ (rs1_t rs1, rs2_t rs2) { rd_t rd; __asm__(#insn " %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
#define DECLARE_INTRIN_VOLATILE_RR(_insn_, insn, rd_t, rs1_t, rs2_t) static inline rd_t __rv_ ## _insn_ (rs1_t rs1, rs2_t rs2) { rd_t rd; __asm__ volatile(#insn " %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
#define DECLARE_INTRIN_RIMM_ONLY(_insn_, insn, imm_mask, rd_t, rs1_t, rs2_t) static inline rd_t __rv_ ## _insn_ (rs1_t rs1, rs2_t rs2) { rd_t rd; __asm__(#insn " %0, %1, %2" : "=r"(rd) : "r"(rs1), "i"(rs2 & (imm_mask))); return rd; }
#define DECLARE_INTRIN_VOLATILE_RIMM_ONLY(_insn_, insn, imm_mask, rd_t, rs1_t, rs2_t) static inline rd_t __rv_ ## _insn_ (rs1_t rs1, rs2_t rs2) { rd_t rd; __asm__ volatile(#insn " %0, %1, %2" : "=r"(rd) : "r"(rs1), "i"(rs2 & (imm_mask))); return rd; }
#define DECLARE_INTRIN_RI_(_insn_, insn, tail, imm_mask, rd_t, rs1_t, rs2_t) static inline rd_t __rv_ ## _insn_ (rs1_t rs1, rs2_t rs2) { rd_t rd; if (__builtin_constant_p(rs2)) __asm__(#insn "i" #tail " %0, %1, %2" : "=r"(rd) : "r"(rs1), "i"(rs2 & (imm_mask))); else __asm__(#insn #tail " %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
#define DECLARE_INTRIN_RI(_insn_, insn, imm_mask, rd_t, rs1_t, rs2_t) DECLARE_INTRIN_RI_(_insn_, insn,, imm_mask, rd_t, rs1_t, rs2_t)
#define DECLARE_INTRIN_VOLATILE_RI_(_insn_, insn, tail, imm_mask, rd_t, rs1_t, rs2_t) static inline rd_t __rv_ ## _insn_ (rs1_t rs1, rs2_t rs2) { rd_t rd; if (__builtin_constant_p(rs2)) __asm__ volatile(#insn "i" #tail " %0, %1, %2" : "=r"(rd) : "r"(rs1), "i"(rs2 & (imm_mask))); else __asm__ volatile(#insn #tail " %0, %1, %2" : "=r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
#define DECLARE_INTRIN_RR_DESTRUCTIVE(_insn_, insn, rd_t, rs1_t, rs2_t) static inline rd_t __rv_ ## _insn_ (rd_t rd, rs1_t rs1, rs2_t rs2) { __asm__(#insn " %0, %1, %2" : "+r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
#define DECLARE_INTRIN_VOLATILE_RR_DESTRUCTIVE(_insn_, insn, rd_t, rs1_t, rs2_t) static inline rd_t __rv_ ## _insn_ (rd_t rd, rs1_t rs1, rs2_t rs2) { __asm__ volatile(#insn " %0, %1, %2" : "+r"(rd) : "r"(rs1), "r"(rs2)); return rd; }
#define DECLARE_INTRIN_(_insn_, insn, MACRO, types...) DECLARE_INTRIN_##MACRO(_insn_, insn, types)
#define DECLARE_INTRIN(insn, MACRO, types...) DECLARE_INTRIN_(insn, insn, MACRO, types)


__attribute__((error("BPICK instruction has been removed. Consider CMIX instruction")))
uintXLEN_t __rv_bpick (uintXLEN_t rs1, uintXLEN_t rs2, uintXLEN_t rc);

__attribute__((error("MAXW instruction has been removed. Consider MAX instruction")))
intXLEN_t __rv_maxw (int32_t rs1, int32_t r2);

__attribute__((error("MINW instruction has been removed. Consider MIN instruction")))
intXLEN_t __rv_minw ( int32_t rs1, int32_t r2);

DECLARE_INTRIN(add8, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(add16, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(ave, RR, intXLEN_t, intXLEN_t, intXLEN_t)
static inline uintXLEN_t __rv_cmix(uintXLEN_t rs1, uintXLEN_t rs2, uintXLEN_t rs3) { uintXLEN_t rd; __asm__("cmix %0, %1, %2, %3" : "=r"(rd) : "r"(rs2), "r"(rs1), "r"(rs3)); return rd; }
static inline void __rv_clrov (void) { __asm__ volatile("clrov"); }
DECLARE_INTRIN(clrs8, R, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(clrs16, R, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(clrs32, R, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(clz8, R, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(clz16, R, uintXLEN_t, uintXLEN_t)
#if (__riscv_xlen == 64)
DECLARE_INTRIN(clz32, R, uintXLEN_t, uintXLEN_t)
#elif (__riscv_xlen == 32)
static inline uintXLEN_t __rv_clz32 (uintXLEN_t rs1) { uintXLEN_t rd; __asm__("clz %0, %1" : "=r"(rd) : "r"(rs1)); return rd; }
#endif
DECLARE_INTRIN(cmpeq8, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(cmpeq16, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(cras16, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(crsa16, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
static inline uintXLEN_t __rv_insb (uintXLEN_t rd, uintXLEN_t rs1, uint32_t rs2) { __asm__("insb %0, %1, %2" : "+r"(rd) : "r"(rs1), "i"(rs2 & (__riscv_xlen / 8 - 1))); return rd; }
DECLARE_INTRIN(kabs8, R, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kabs16, R, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kabsw, R, intXLEN_t, intXLEN_t)
DECLARE_INTRIN(kadd8, VOLATILE_RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kadd16, VOLATILE_RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kaddh, VOLATILE_RR, intXLEN_t, int32_t, int32_t)
DECLARE_INTRIN(kaddw, VOLATILE_RR, intXLEN_t, int32_t, int32_t)
DECLARE_INTRIN(kcras16, VOLATILE_RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kcrsa16, VOLATILE_RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kdmbb, VOLATILE_RR, int32_t, uint32_t, uint32_t)
DECLARE_INTRIN(kdmbt, VOLATILE_RR, int32_t, uint32_t, uint32_t)
DECLARE_INTRIN(kdmtt, VOLATILE_RR, int32_t, uint32_t, uint32_t)
DECLARE_INTRIN(kdmabb, VOLATILE_RR_DESTRUCTIVE, int32_t, uint32_t, uint32_t)
DECLARE_INTRIN(kdmabt, VOLATILE_RR_DESTRUCTIVE, int32_t, uint32_t, uint32_t)
DECLARE_INTRIN(kdmatt, VOLATILE_RR_DESTRUCTIVE, int32_t, uint32_t, uint32_t)
DECLARE_INTRIN(khm8, VOLATILE_RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(khmx8, VOLATILE_RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(khm16, VOLATILE_RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(khmx16, VOLATILE_RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(khmbb, VOLATILE_RR, intXLEN_t, uint32_t, uint32_t)
DECLARE_INTRIN(khmbt, VOLATILE_RR, intXLEN_t, uint32_t, uint32_t)
DECLARE_INTRIN(khmtt, VOLATILE_RR, intXLEN_t, uint32_t, uint32_t)
DECLARE_INTRIN(kmabb, VOLATILE_RR_DESTRUCTIVE, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kmabt, VOLATILE_RR_DESTRUCTIVE, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kmatt, VOLATILE_RR_DESTRUCTIVE, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kmada, VOLATILE_RR_DESTRUCTIVE, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kmaxda, VOLATILE_RR_DESTRUCTIVE, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kmads, VOLATILE_RR_DESTRUCTIVE, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kmadrs, VOLATILE_RR_DESTRUCTIVE, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kmaxds, VOLATILE_RR_DESTRUCTIVE, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kmda, VOLATILE_RR, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kmxda, VOLATILE_RR, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kmmac, VOLATILE_RR_DESTRUCTIVE, intXLEN_t, intXLEN_t, intXLEN_t)
DECLARE_INTRIN_(kmmac_u, kmmac.u, VOLATILE_RR_DESTRUCTIVE, intXLEN_t, intXLEN_t, intXLEN_t)
DECLARE_INTRIN(kmmawb, VOLATILE_RR_DESTRUCTIVE, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN_(kmmawb_u, kmmawb.u, VOLATILE_RR_DESTRUCTIVE, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kmmawb2, VOLATILE_RR_DESTRUCTIVE, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN_(kmmawb2_u, kmmawb2.u, VOLATILE_RR_DESTRUCTIVE, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kmmawt, VOLATILE_RR_DESTRUCTIVE, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN_(kmmawt_u, kmmawt.u, VOLATILE_RR_DESTRUCTIVE, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kmmawt2, VOLATILE_RR_DESTRUCTIVE, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN_(kmmawt2_u, kmmawt2.u, VOLATILE_RR_DESTRUCTIVE, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kmmsb, VOLATILE_RR_DESTRUCTIVE, intXLEN_t, intXLEN_t, intXLEN_t)
DECLARE_INTRIN_(kmmsb_u, kmmsb.u, VOLATILE_RR_DESTRUCTIVE, intXLEN_t, intXLEN_t, intXLEN_t)
DECLARE_INTRIN(kmmwb2, VOLATILE_RR, intXLEN_t, intXLEN_t, uintXLEN_t)
DECLARE_INTRIN_(kmmwb2_u, kmmwb2.u, VOLATILE_RR, intXLEN_t, intXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kmmwt2, VOLATILE_RR, intXLEN_t, intXLEN_t, uintXLEN_t)
DECLARE_INTRIN_(kmmwt2_u, kmmwt2.u, VOLATILE_RR, intXLEN_t, intXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kmsda, VOLATILE_RR_DESTRUCTIVE, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kmsxda, VOLATILE_RR_DESTRUCTIVE, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN_(ksllw, ksll, VOLATILE_RI_, w, 31, int32_t, int32_t, int32_t)
DECLARE_INTRIN_(ksll8, ksll, VOLATILE_RI_, 8, 7, uintXLEN_t, uintXLEN_t, int32_t)
DECLARE_INTRIN_(ksll16, ksll, VOLATILE_RI_, 16, 15, uintXLEN_t, uintXLEN_t, int32_t)
DECLARE_INTRIN(kslra8, VOLATILE_RR, uintXLEN_t, uintXLEN_t, int)
DECLARE_INTRIN_(kslra8_u, kslra8.u, VOLATILE_RR, uintXLEN_t, uintXLEN_t, int)
DECLARE_INTRIN(kslra16, VOLATILE_RR, uintXLEN_t, uintXLEN_t, int)
DECLARE_INTRIN_(kslra16_u, kslra16.u, VOLATILE_RR, uintXLEN_t, uintXLEN_t, int)
DECLARE_INTRIN(kslraw, VOLATILE_RR, intXLEN_t, int32_t, int32_t)
DECLARE_INTRIN_(kslraw_u, kslraw.u, VOLATILE_RR, intXLEN_t, int32_t, int32_t)
DECLARE_INTRIN(kstas16, VOLATILE_RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kstsa16, VOLATILE_RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(ksub8, VOLATILE_RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(ksub16, VOLATILE_RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(ksubh, VOLATILE_RR, intXLEN_t, int32_t, int32_t)
DECLARE_INTRIN(ksubw, VOLATILE_RR, intXLEN_t, int32_t, int32_t)
DECLARE_INTRIN(kwmmul, VOLATILE_RR, intXLEN_t, intXLEN_t, intXLEN_t)
DECLARE_INTRIN_(kwmmul_u, kwmmul.u, VOLATILE_RR, intXLEN_t, intXLEN_t, intXLEN_t)
DECLARE_INTRIN(maddr32, RR_DESTRUCTIVE, int32_t, int32_t, int32_t)
DECLARE_INTRIN(max, RR, intXLEN_t, intXLEN_t, intXLEN_t)
DECLARE_INTRIN(min, RR, intXLEN_t, intXLEN_t, intXLEN_t)
DECLARE_INTRIN(msubr32, RR_DESTRUCTIVE, int32_t, int32_t, int32_t)
DECLARE_INTRIN(pbsad, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(pbsada, RR_DESTRUCTIVE, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(pkbb16, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(pkbt16, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(pktb16, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(pktt16, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(radd8, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(radd16, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(raddw, RR, intXLEN_t, int32_t, int32_t)
DECLARE_INTRIN(rcras16, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(rcrsa16, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
static inline uintXLEN_t __rv_rdov (void) { uintXLEN_t rd; __asm__ volatile("rdov %0" : "=r"(rd)); return rd; }
DECLARE_INTRIN(rstas16, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(rstsa16, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(rsub8, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(rsub16, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(rsubw, RR, intXLEN_t, int32_t, int32_t)
DECLARE_INTRIN(sclip8, VOLATILE_RIMM_ONLY, 7, uintXLEN_t, uintXLEN_t, uint32_t)
DECLARE_INTRIN(sclip16, VOLATILE_RIMM_ONLY, 15, uintXLEN_t, uintXLEN_t, uint32_t)
DECLARE_INTRIN(sclip32, VOLATILE_RIMM_ONLY, 31, uintXLEN_t, uintXLEN_t, uint32_t)
DECLARE_INTRIN(scmple8, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(scmple16, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(scmplt8, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(scmplt16, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN_(sll8, sll, RI_, 8, 7, uintXLEN_t, uintXLEN_t, int32_t)
DECLARE_INTRIN_(sll16, sll, RI_, 16, 15, uintXLEN_t, uintXLEN_t, int32_t)
DECLARE_INTRIN(smaqa, RR_DESTRUCTIVE, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN_(smaqa_su, smaqa.su, RR_DESTRUCTIVE, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(smax8, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(smax16, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(smbb16, RR, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(smbt16, RR, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(smtt16, RR, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(smds, RR, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(smdrs, RR, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(smxds, RR, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(smin8, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(smin16, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(smmul, RR, intXLEN_t, intXLEN_t, intXLEN_t)
DECLARE_INTRIN_(smmul_u, smmul.u, RR, intXLEN_t, intXLEN_t, intXLEN_t)
DECLARE_INTRIN(smmwb, RR, intXLEN_t, intXLEN_t, uintXLEN_t)
DECLARE_INTRIN_(smmwb_u, smmwb.u, RR, intXLEN_t, intXLEN_t, uintXLEN_t)
DECLARE_INTRIN(smmwt, RR, intXLEN_t, intXLEN_t, uintXLEN_t)
DECLARE_INTRIN_(smmwt_u, smmwt.u, RR, intXLEN_t, intXLEN_t, uintXLEN_t)
DECLARE_INTRIN_(sra_u, sra, RI_, .u, __riscv_xlen - 1, intXLEN_t, intXLEN_t, int32_t)
DECLARE_INTRIN_(sra8, sra, RI_, 8, 7, uintXLEN_t, uintXLEN_t, int32_t)
DECLARE_INTRIN_(sra8_u, sra, RI_, 8.u, 7, uintXLEN_t, uintXLEN_t, int32_t)
DECLARE_INTRIN_(sra16, sra, RI_, 16, 15, uintXLEN_t, uintXLEN_t, int32_t)
DECLARE_INTRIN_(sra16_u, sra, RI_, 16.u, 15, uintXLEN_t, uintXLEN_t, int32_t)
DECLARE_INTRIN_(srl8, srl, RI_, 8, 7, uintXLEN_t, uintXLEN_t, int32_t)
DECLARE_INTRIN_(srl8_u, srl, RI_, 8.u, 7, uintXLEN_t, uintXLEN_t, int32_t)
DECLARE_INTRIN_(srl16, srl, RI_, 16, 15, uintXLEN_t, uintXLEN_t, int32_t)
DECLARE_INTRIN_(srl16_u, srl, RI_, 16.u, 15, uintXLEN_t, uintXLEN_t, int32_t)
DECLARE_INTRIN(stas16, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(stsa16, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(sub8, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(sub16, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(sunpkd810, R, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(sunpkd820, R, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(sunpkd830, R, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(sunpkd831, R, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(sunpkd832, R, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(swap8, R, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(swap16, R, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(uclip8, VOLATILE_RIMM_ONLY, 7, uintXLEN_t, uintXLEN_t, uint32_t)
DECLARE_INTRIN(uclip16, VOLATILE_RIMM_ONLY, 15, uintXLEN_t, uintXLEN_t, uint32_t)
DECLARE_INTRIN(uclip32, VOLATILE_RIMM_ONLY, 31, uintXLEN_t, uintXLEN_t, uint32_t)
DECLARE_INTRIN(ucmple8, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(ucmple16, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(ucmplt8, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(ucmplt16, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(ukadd8, VOLATILE_RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(ukadd16, VOLATILE_RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(ukaddh, VOLATILE_RR, uintXLEN_t, uint32_t, uint32_t)
DECLARE_INTRIN(ukaddw, VOLATILE_RR, uintXLEN_t, uint32_t, uint32_t)
DECLARE_INTRIN(ukcras16, VOLATILE_RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(ukcrsa16, VOLATILE_RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(ukstas16, VOLATILE_RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(ukstsa16, VOLATILE_RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(uksub8, VOLATILE_RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(uksub16, VOLATILE_RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(uksubh, VOLATILE_RR, uintXLEN_t, uint32_t, uint32_t)
DECLARE_INTRIN(uksubw, VOLATILE_RR, uintXLEN_t, uint32_t, uint32_t)
DECLARE_INTRIN(umaqa, RR_DESTRUCTIVE, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(umax8, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(umax16, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(umin8, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(umin16, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(uradd8, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(uradd16, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(uraddw, RR, uintXLEN_t, uint32_t, uint32_t)
DECLARE_INTRIN(urcras16, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(urcrsa16, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(urstas16, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(urstsa16, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(ursub8, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(ursub16, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(ursubw, RR, uintXLEN_t, uint32_t, uint32_t)
DECLARE_INTRIN(zunpkd810, R, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(zunpkd820, R, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(zunpkd830, R, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(zunpkd831, R, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(zunpkd832, R, uintXLEN_t, uintXLEN_t)

#if __riscv_xlen == 64
DECLARE_INTRIN(add32, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(cras32, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(crsa32, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kabs32, R, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kadd32, VOLATILE_RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kcras32, VOLATILE_RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kcrsa32, VOLATILE_RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kdmbb16, VOLATILE_RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kdmbt16, VOLATILE_RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kdmtt16, VOLATILE_RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kdmabb16, VOLATILE_RR_DESTRUCTIVE, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kdmabt16, VOLATILE_RR_DESTRUCTIVE, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kdmatt16, VOLATILE_RR_DESTRUCTIVE, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(khmbb16, VOLATILE_RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(khmbt16, VOLATILE_RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(khmtt16, VOLATILE_RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kmabb32, VOLATILE_RR_DESTRUCTIVE, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kmabt32, VOLATILE_RR_DESTRUCTIVE, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kmatt32, VOLATILE_RR_DESTRUCTIVE, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kmada32, VOLATILE_RR_DESTRUCTIVE, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kmaxda32, VOLATILE_RR_DESTRUCTIVE, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kmda32, VOLATILE_RR, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kmxda32, VOLATILE_RR, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kmads32, VOLATILE_RR_DESTRUCTIVE, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kmadrs32, VOLATILE_RR_DESTRUCTIVE, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kmaxds32, VOLATILE_RR_DESTRUCTIVE, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kmsda32, VOLATILE_RR_DESTRUCTIVE, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kmsxda32, VOLATILE_RR_DESTRUCTIVE, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN_(ksll32, ksll, VOLATILE_RI_, 32, 31, uintXLEN_t, uintXLEN_t, int32_t)
DECLARE_INTRIN(kslra32, VOLATILE_RR, uintXLEN_t, uintXLEN_t, int32_t)
DECLARE_INTRIN_(kslra32_u, kslra32.u, VOLATILE_RR, uintXLEN_t, uintXLEN_t, int32_t)
DECLARE_INTRIN(kstas32, VOLATILE_RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(kstsa32, VOLATILE_RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(ksub32, VOLATILE_RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(pkbb32, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(pkbt32, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(pktb32, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(pktt32, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(radd32, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(rcras32, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(rcrsa32, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(rstas32, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(rstsa32, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(rsub32, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN_(sll32, sll, RI_, 32, 31, uintXLEN_t, uintXLEN_t, int32_t)
DECLARE_INTRIN(smax32, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(smbb32, RR, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(smbt32, RR, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(smtt32, RR, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(smds32, RR, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(smdrs32, RR, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(smxds32, RR, intXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(smin32, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN_(sra32, sra, RI_, 32, 31, uintXLEN_t, uintXLEN_t, int32_t)
DECLARE_INTRIN_(sra32_u, sra, RI_, 32.u, 31, uintXLEN_t, uintXLEN_t, int32_t)
DECLARE_INTRIN_(sraw_u, sraiw.u, RIMM_ONLY, 31, uintXLEN_t, uintXLEN_t, uint32_t)
DECLARE_INTRIN_(srl32, srl, RI_, 32, 31, uintXLEN_t, uintXLEN_t, int32_t)
DECLARE_INTRIN_(srl32_u, srl, RI_, 32.u, 31, uintXLEN_t, uintXLEN_t, int32_t)
DECLARE_INTRIN(stas32, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(stsa32, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(sub32, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(ukadd32, VOLATILE_RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(ukcras32, VOLATILE_RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(ukcrsa32, VOLATILE_RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(ukstas32, VOLATILE_RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(ukstsa32, VOLATILE_RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(uksub32, VOLATILE_RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(umax32, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(umin32, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(uradd32, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(urcras32, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(urcrsa32, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(urstas32, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(urstsa32, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)
DECLARE_INTRIN(ursub32, RR, uintXLEN_t, uintXLEN_t, uintXLEN_t)

#endif // RV64 only

#undef DECLARE_INTRIN_R
#undef DECLARE_INTRIN_RR
#undef DECLARE_INTRIN_VOLATILE_RR
#undef DECLARE_INTRIN_RIMM_ONLY
#undef DECLARE_INTRIN_VOLATILE_RIMM_ONLY
#undef DECLARE_INTRIN_RI_
#undef DECLARE_INTRIN_RI
#undef DECLARE_INTRIN_VOLATILE_RI_
#undef DECLARE_INTRIN_RR_DESTRUCTIVE
#undef DECLARE_INTRIN_VOLATILE_RR_DESTRUCTIVE
#undef DECLARE_INTRIN_
#undef DECLARE_INTRIN

#else // ^^^  __riscv_zpn ^^^ | vvv !__riscv_zpn vvv
// FIXME[dzakharov]: Issue: CB-2720:: update code and remove warnings suppression
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wshift-count-overflow"

static uintXLEN_t __rv_vxsat = 0;

#define SAT(U, N)                                                           \
    val = val > U##INT##N##_MAX ? __rv_vxsat = 1, U##INT##N##_MAX : val;    \
    val = val < U##INT##N##_MIN ? __rv_vxsat = 1, U##INT##N##_MIN : val;    \
    return val & UINT##N##_MAX

static inline intXLEN_t __rv_sat_q7(int64_t val)
{
    SAT(, 8);
}

static inline intXLEN_t __rv_sat_q15(int64_t val)
{
    SAT(, 16);
}

static inline intXLEN_t __rv_sat_q31(int64_t val)
{
    SAT(, 32);
}

#define UINT8_MIN 0
#define UINT16_MIN 0
#define UINT32_MIN 0
static inline intXLEN_t __rv_sat_u8(int64_t val)
{
    SAT(U, 8);
}

static inline intXLEN_t __rv_sat_u16(int64_t val)
{
    SAT(U, 16);
}

static inline intXLEN_t __rv_sat_u32(int64_t val)
{
    SAT(U, 32);
}

#undef SAT

#define ADD_SUB(u, n, BODY)                                 \
    uintXLEN_t rd = 0;                                      \
    for (size_t i = 0; i < __riscv_xlen; i += n) {          \
        u##int##n##_t rs1s = rs1 >> i, rs2s = rs2 >> i;     \
        rd |= (uintXLEN_t)((BODY) & UINT##n##_MAX) << i;    \
    }                                                       \
    return rd

static inline uintXLEN_t __rv_add8(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ADD_SUB( , 8, rs1s + rs2s );
}

static inline uintXLEN_t __rv_add16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ADD_SUB( , 16, rs1s + rs2s );
}

#if __riscv_xlen == 64
static inline uintXLEN_t __rv_add32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ADD_SUB( , 32, rs1s + rs2s );
}
#endif

static inline uintXLEN_t __rv_sub8(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ADD_SUB( , 8, rs1s - rs2s );
}

static inline uintXLEN_t __rv_sub16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ADD_SUB( , 16, rs1s - rs2s );
}

#if __riscv_xlen == 64
static inline uintXLEN_t __rv_sub32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ADD_SUB( , 32, rs1s - rs2s );
}
#endif

static inline uintXLEN_t __rv_kadd8(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ADD_SUB( , 8, __rv_sat_q7((intXLEN_t)rs1s + (intXLEN_t)rs2s) );
}

static inline uintXLEN_t __rv_kadd16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ADD_SUB( , 16, __rv_sat_q15((intXLEN_t)rs1s + (intXLEN_t)rs2s) );
}

#if __riscv_xlen == 64
static inline uintXLEN_t __rv_kadd32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ADD_SUB( , 32, __rv_sat_q31((intXLEN_t)rs1s + (intXLEN_t)rs2s) );
}
#endif

static inline uintXLEN_t __rv_ksub8(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ADD_SUB( , 8, __rv_sat_q7((intXLEN_t)rs1s - (intXLEN_t)rs2s) );
}

static inline uintXLEN_t __rv_ksub16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ADD_SUB( , 16, __rv_sat_q15((intXLEN_t)rs1s - (intXLEN_t)rs2s) );
}

#if __riscv_xlen == 64
static inline uintXLEN_t __rv_ksub32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ADD_SUB( , 32, __rv_sat_q31((intXLEN_t)rs1s - (intXLEN_t)rs2s) );
}
#endif

static inline uintXLEN_t __rv_radd8(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ADD_SUB( , 8, ((intXLEN_t)rs1s + (intXLEN_t)rs2s) >> 1 );
}

static inline uintXLEN_t __rv_radd16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ADD_SUB( , 16, ((intXLEN_t)rs1s + (intXLEN_t)rs2s) >> 1 );
}

#if __riscv_xlen == 64
static inline uintXLEN_t __rv_radd32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ADD_SUB( , 32, ((intXLEN_t)rs1s + (intXLEN_t)rs2s) >> 1 );
}
#endif

static inline uintXLEN_t __rv_rsub8(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ADD_SUB( , 8, ((intXLEN_t)rs1s - (intXLEN_t)rs2s) >> 1 );
}

static inline uintXLEN_t __rv_rsub16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ADD_SUB( , 16, ((intXLEN_t)rs1s - (intXLEN_t)rs2s) >> 1 );
}

#if __riscv_xlen == 64
static inline uintXLEN_t __rv_rsub32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ADD_SUB( , 32, ((intXLEN_t)rs1s - (intXLEN_t)rs2s) >> 1 );
}
#endif

static inline uintXLEN_t __rv_ukadd8(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ADD_SUB( u, 8, __rv_sat_u8((uintXLEN_t)rs1s + (uintXLEN_t)rs2s) );
}

static inline uintXLEN_t __rv_ukadd16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ADD_SUB( u, 16, __rv_sat_u16((uintXLEN_t)rs1s + (uintXLEN_t)rs2s) );
}

#if __riscv_xlen == 64
static inline uintXLEN_t __rv_ukadd32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ADD_SUB( u, 32, __rv_sat_u32((uintXLEN_t)rs1s + (uintXLEN_t)rs2s) );
}
#endif

static inline uintXLEN_t __rv_uksub8(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ADD_SUB( u, 8, __rv_sat_u8((intXLEN_t)((uintXLEN_t)rs1s - (uintXLEN_t)rs2s)) );
}

static inline uintXLEN_t __rv_uksub16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ADD_SUB( u, 16, __rv_sat_u16((intXLEN_t)((uintXLEN_t)rs1s - (uintXLEN_t)rs2s)) );
}

#if __riscv_xlen == 64
static inline uintXLEN_t __rv_uksub32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ADD_SUB( u, 32, __rv_sat_u32((intXLEN_t)((uintXLEN_t)rs1s - (uintXLEN_t)rs2s)) );
}
#endif

static inline uintXLEN_t __rv_uradd8(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ADD_SUB( u, 8, ((uintXLEN_t)rs1s + (uintXLEN_t)rs2s) >> 1 );
}

static inline uintXLEN_t __rv_uradd16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ADD_SUB( u, 16, ((uintXLEN_t)rs1s + (uintXLEN_t)rs2s) >> 1 );
}

#if __riscv_xlen == 64
static inline uintXLEN_t __rv_uradd32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ADD_SUB( u, 32, ((uintXLEN_t)rs1s + (uintXLEN_t)rs2s) >> 1 );
}
#endif

static inline uintXLEN_t __rv_ursub8(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ADD_SUB( u, 8, ((uintXLEN_t)rs1s - (uintXLEN_t)rs2s) >> 1 );
}

static inline uintXLEN_t __rv_ursub16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ADD_SUB( u, 16, ((uintXLEN_t)rs1s - (uintXLEN_t)rs2s) >> 1 );
}

#if __riscv_xlen == 64
static inline uintXLEN_t __rv_ursub32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ADD_SUB( u, 32, ((uintXLEN_t)rs1s - (uintXLEN_t)rs2s) >> 1 );
}
#endif

#undef ADD_SUB

#define ADD_SUB(U, N, R, BODY)                                          \
    int64_t rd = BODY;                                                  \
    if (R) return (int32_t)(rd >> 1);                                   \
    if (rd > U##INT##N##_MAX) __rv_vxsat = 1, rd = U##INT##N##_MAX;     \
    if (rd < U##INT##N##_MIN) __rv_vxsat = 1, rd = U##INT##N##_MIN;     \
    return (int##N##_t)rd

static inline intXLEN_t __rv_kaddh(int32_t rs1, int32_t rs2)
{
    ADD_SUB(, 16, 0, (int64_t)rs1 + (int64_t)rs2);
}

static inline intXLEN_t __rv_kaddw(int32_t rs1, int32_t rs2)
{
    ADD_SUB(, 32, 0, (int64_t)rs1 + (int64_t)rs2);
}

static inline intXLEN_t __rv_ksubh(int32_t rs1, int32_t rs2)
{
    ADD_SUB(, 16, 0, (int64_t)rs1 - (int64_t)rs2);
}

static inline intXLEN_t __rv_ksubw(int32_t rs1, int32_t rs2)
{
    ADD_SUB(, 32, 0, (int64_t)rs1 - (int64_t)rs2);
}

static inline intXLEN_t __rv_raddw(int32_t rs1, int32_t rs2)
{
    ADD_SUB(, 32, 1, (int64_t)rs1 + (int64_t)rs2);
}

static inline intXLEN_t __rv_rsubw(int32_t rs1, int32_t rs2)
{
    ADD_SUB(, 32, 1, (int64_t)rs1 - (int64_t)rs2);
}

static inline uintXLEN_t __rv_ukaddh(uint32_t rs1, uint32_t rs2)
{
    ADD_SUB(U, 16, 0, (uint64_t)rs1 + (uint64_t)rs2);
}

static inline uintXLEN_t __rv_ukaddw(uint32_t rs1, uint32_t rs2)
{
    ADD_SUB(U, 32, 0, (uint64_t)rs1 + (uint64_t)rs2);
}

static inline uintXLEN_t __rv_uksubh(uint32_t rs1, uint32_t rs2)
{
    ADD_SUB(U, 16, 0, (uint64_t)rs1 - (uint64_t)rs2);
}

static inline uintXLEN_t __rv_uksubw(uint32_t rs1, uint32_t rs2)
{
    ADD_SUB(U, 32, 0, (uint64_t)rs1 - (uint64_t)rs2);
}

static inline uintXLEN_t __rv_uraddw(uint32_t rs1, uint32_t rs2)
{
    ADD_SUB(U, 32, 1, (uint64_t)rs1 + (uint64_t)rs2);
}

static inline uintXLEN_t __rv_ursubw(uint32_t rs1, uint32_t rs2)
{
    ADD_SUB(U, 32, 1, (uint64_t)rs1 - (uint64_t)rs2);
}

#undef UINT8_MIN
#undef UINT16_MIN
#undef UINT32_MIN
#undef ADD_SUB

static inline intXLEN_t __rv_ave(intXLEN_t rs1, intXLEN_t rs2)
{
    return (rs1 >> 1) + (rs2 >> 1) + ((rs1 & 1) | (rs2 & 1));
}

static inline uintXLEN_t __rv_bpick(uintXLEN_t rs1, uintXLEN_t rs2, uintXLEN_t rc) {
    return (rs1 & rc) | (~rc & rs2);
}

static inline void __rv_clrov(void) {
    __rv_vxsat = 0;
}

#define COUNT_LEADING(n, rs, what)                                                      \
    uintXLEN_t rd = 0;                                                                  \
    for (size_t i = 0; i < __riscv_xlen; i += n) {                                      \
        uint##n##_t counter = 0, part = rs1 >> i; part = what part;                     \
        if ((n /  1) && !(part))  { rd |= (uintXLEN_t)(n - rs) << i; continue;        } \
        if ((n /  2) && !(part >> (n - n /  2))){ counter += n /  2; part <<= n /  2; } \
        if ((n /  4) && !(part >> (n - n /  4))){ counter += n /  4; part <<= n /  4; } \
        if ((n /  8) && !(part >> (n - n /  8))){ counter += n /  8; part <<= n /  8; } \
        if ((n / 16) && !(part >> (n - n / 16))){ counter += n / 16; part <<= n / 16; } \
        if ((n / 32) && !(part >> (n - n / 32))){ counter += n / 32; part <<= n / 32; } \
        rd |= (uintXLEN_t)(counter - rs) << i;                                          \
    }                                                                                   \
    return rd

static inline uintXLEN_t __rv_clrs8(uintXLEN_t rs1)
{
    COUNT_LEADING(8, 1, part >> 7 ? ~part :);
}

static inline uintXLEN_t __rv_clrs16(uintXLEN_t rs1)
{
    COUNT_LEADING(16, 1, part >> 15 ? ~part :);
}

static inline uintXLEN_t __rv_clrs32(uintXLEN_t rs1)
{
    COUNT_LEADING(32, 1, part >> 31 ? ~part :);
}

static inline uintXLEN_t __rv_clz8(uintXLEN_t rs1)
{
    COUNT_LEADING(8, 0, );
}

static inline uintXLEN_t __rv_clz16(uintXLEN_t rs1)
{
    COUNT_LEADING(16, 0, );
}

static inline uintXLEN_t __rv_clz32(uintXLEN_t rs1)
{
    COUNT_LEADING(32, 0, );
}

#undef COUNT_LEADING

#define CMPEQ(n)                                    \
    uintXLEN_t rd = 0;                              \
    for (size_t i = 0; i < __riscv_xlen; i += n) {  \
        if ((uint##n##_t)rs1 == (uint##n##_t)rs2)   \
            rd |= (uintXLEN_t)UINT##n##_MAX << i;   \
        rs1 >>= n; rs2 >>= n;                       \
    }                                               \
    return rd

static inline uintXLEN_t __rv_cmpeq8(uintXLEN_t rs1, uintXLEN_t rs2)
{
    CMPEQ(8);
}

static inline uintXLEN_t __rv_cmpeq16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    CMPEQ(16);
}

#undef CMPEQ

#define CROSS_ADD_SUB(u, n, BODY1, BODY2)                       \
    uintXLEN_t rd = 0;                                          \
    for (size_t i = 0; i < __riscv_xlen; i += n * 2) {          \
        u##int##n##_t rs1b = rs1 >> i, rs1t = rs1 >> (i + n)    \
                    , rs2b = rs2 >> i, rs2t = rs2 >> (i + n);   \
        rd |= ((uintXLEN_t)(uint##n##_t)(BODY1) << (i + n))     \
            | ((uintXLEN_t)(uint##n##_t)(BODY2) << i);          \
    }                                                           \
    return rd

static inline uintXLEN_t __rv_cras16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    CROSS_ADD_SUB(, 16, rs1t + rs2b, rs1b - rs2t);
}

static inline uintXLEN_t __rv_crsa16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    CROSS_ADD_SUB(, 16, rs1t - rs2b, rs1b + rs2t);
}

static inline uintXLEN_t __rv_kcras16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    CROSS_ADD_SUB(, 16, __rv_sat_q15((intXLEN_t)rs1t + (intXLEN_t)rs2b),
                        __rv_sat_q15((intXLEN_t)rs1b - (intXLEN_t)rs2t));
}

static inline uintXLEN_t __rv_kcrsa16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    CROSS_ADD_SUB(, 16, __rv_sat_q15((intXLEN_t)rs1t - (intXLEN_t)rs2b),
                        __rv_sat_q15((intXLEN_t)rs1b + (intXLEN_t)rs2t));
}

static inline uintXLEN_t __rv_rcras16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    CROSS_ADD_SUB(, 16, ((intXLEN_t)rs1t + (intXLEN_t)rs2b) >> 1,
                        ((intXLEN_t)rs1b - (intXLEN_t)rs2t) >> 1);
}

static inline uintXLEN_t __rv_rcrsa16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    CROSS_ADD_SUB(, 16, ((intXLEN_t)rs1t - (intXLEN_t)rs2b) >> 1,
                        ((intXLEN_t)rs1b + (intXLEN_t)rs2t) >> 1);
}

static inline uintXLEN_t __rv_ukcras16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    CROSS_ADD_SUB(u, 16, __rv_sat_u16((intXLEN_t)rs1t + (intXLEN_t)rs2b),
                         __rv_sat_u16((intXLEN_t)rs1b - (intXLEN_t)rs2t));
}

static inline uintXLEN_t __rv_ukcrsa16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    CROSS_ADD_SUB(u, 16, __rv_sat_u16((intXLEN_t)rs1t - (intXLEN_t)rs2b),
                         __rv_sat_u16((intXLEN_t)rs1b + (intXLEN_t)rs2t));
}

static inline uintXLEN_t __rv_urcras16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    CROSS_ADD_SUB(u, 16, ((intXLEN_t)rs1t + (intXLEN_t)rs2b) >> 1,
                         ((intXLEN_t)rs1b - (intXLEN_t)rs2t) >> 1);
}

static inline uintXLEN_t __rv_urcrsa16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    CROSS_ADD_SUB(u, 16, ((intXLEN_t)rs1t - (intXLEN_t)rs2b) >> 1,
                         ((intXLEN_t)rs1b + (intXLEN_t)rs2t) >> 1);
}

#if __riscv_xlen == 64

static inline uintXLEN_t __rv_cras32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    CROSS_ADD_SUB(, 32, rs1t + rs2b, rs1b - rs2t);
}

static inline uintXLEN_t __rv_crsa32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    CROSS_ADD_SUB(, 32, rs1t - rs2b, rs1b + rs2t);
}

static inline uintXLEN_t __rv_kcras32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    CROSS_ADD_SUB(, 32, __rv_sat_q31((intXLEN_t)rs1t + (intXLEN_t)rs2b),
                        __rv_sat_q31((intXLEN_t)rs1b - (intXLEN_t)rs2t));
}

static inline uintXLEN_t __rv_kcrsa32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    CROSS_ADD_SUB(, 32, __rv_sat_q31((intXLEN_t)rs1t - (intXLEN_t)rs2b),
                        __rv_sat_q31((intXLEN_t)rs1b + (intXLEN_t)rs2t));
}

static inline uintXLEN_t __rv_rcras32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    CROSS_ADD_SUB(, 32, (((intXLEN_t)rs1t + (intXLEN_t)rs2b) >> 1),
                        (((intXLEN_t)rs1b - (intXLEN_t)rs2t) >> 1));
}

static inline uintXLEN_t __rv_rcrsa32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    CROSS_ADD_SUB(, 32, (((intXLEN_t)rs1t - (intXLEN_t)rs2b) >> 1),
                        (((intXLEN_t)rs1b + (intXLEN_t)rs2t) >> 1));
}

static inline uintXLEN_t __rv_ukcras32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    CROSS_ADD_SUB(u, 32, __rv_sat_u32((intXLEN_t)rs1t + (intXLEN_t)rs2b),
                         __rv_sat_u32((intXLEN_t)rs1b - (intXLEN_t)rs2t));
}

static inline uintXLEN_t __rv_ukcrsa32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    CROSS_ADD_SUB(u, 32, __rv_sat_u32((intXLEN_t)rs1t - (intXLEN_t)rs2b),
                         __rv_sat_u32((intXLEN_t)rs1b + (intXLEN_t)rs2t));
}

static inline uintXLEN_t __rv_urcras32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    CROSS_ADD_SUB(u, 32, (((uintXLEN_t)rs1t + (uintXLEN_t)rs2b) >> 1),
                         (((uintXLEN_t)rs1b - (uintXLEN_t)rs2t) >> 1));
}

static inline uintXLEN_t __rv_urcrsa32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    CROSS_ADD_SUB(u, 32, (((uintXLEN_t)rs1t - (uintXLEN_t)rs2b) >> 1),
                         (((uintXLEN_t)rs1b + (uintXLEN_t)rs2t) >> 1));
}

#endif // RV64 only

#undef CROSS_ADD_SUB

static inline uintXLEN_t __rv_insb(uintXLEN_t rd, uintXLEN_t rs1, uint32_t imm)
{
    imm &= __riscv_xlen / 8 - 1;
    rs1 <<= 8 * imm;
    return __rv_bpick(rs1, rd, (uintXLEN_t)UINT8_MAX << (8 * imm));
}

#define KABS(n)                                     \
    uintXLEN_t rd = 0;                              \
    for (size_t i = 0; i < __riscv_xlen; i += n) {  \
        int##n##_t p = rs1 >> i;                    \
        if (p == INT##n##_MIN) {                    \
            __rv_vxsat = 1; p = INT##n##_MAX;       \
        } else if (p < 0) { p = -p; }               \
        rd |= (uintXLEN_t)p << i;                   \
    }                                               \
    return rd

static inline uintXLEN_t __rv_kabs8(uintXLEN_t rs1)
{
    KABS(8);
}

static inline uintXLEN_t __rv_kabs16(uintXLEN_t rs1)
{
    KABS(16);
}

#if __riscv_xlen == 64
static inline uintXLEN_t __rv_kabs32(uintXLEN_t rs1)
{
    KABS(32);
}
#endif

static inline intXLEN_t __rv_kabsw(intXLEN_t rs1)
{
    rs1 &= UINT32_MAX; KABS(32) & UINT32_MAX;
}

#undef KABS

#define KXM(n)                                  \
    intXLEN_t res; int16_t a = rs1, b = rs2;    \
    if (a == INT16_MIN && b == INT16_MIN) {     \
        __rv_vxsat = 1;                         \
        res = INT32_MAX;                        \
    } else res = (intXLEN_t)2 * a * b;          \
    res >>= (32 - n);                           \
    return rd ? (int32_t)__rv_sat_q31((int64_t)res + (int32_t)rd) : res

static inline int32_t __rv_kdmbb(uint32_t rs1, uint32_t rs2)
{
    const int rd = 0;
    KXM(32);
}

static inline int32_t __rv_kdmbt(uint32_t rs1, uint32_t rs2)
{
    const int rd = 0; rs2 >>= 16;
    KXM(32);
}

static inline int32_t __rv_kdmtt(uint32_t rs1, uint32_t rs2)
{
    const int rd = 0; rs1 >>= 16; rs2 >>= 16;
    KXM(32);
}

static inline int32_t __rv_kdmabb(int32_t rd, uint32_t rs1, uint32_t rs2)
{
    KXM(32);
}

static inline int32_t __rv_kdmabt(int32_t rd, uint32_t rs1, uint32_t rs2)
{
    rs2 >>= 16;
    KXM(32);
}

static inline int32_t __rv_kdmatt(int32_t rd, uint32_t rs1, uint32_t rs2)
{
    rs1 >>= 16; rs2 >>= 16;
    KXM(32);
}

static inline uintXLEN_t __rv_khmbb(uint32_t rs1, uint32_t rs2)
{
    const int rd = 0;
    KXM(16);
}

static inline uintXLEN_t __rv_khmbt(uint32_t rs1, uint32_t rs2)
{
    const int rd = 0; rs2 >>= 16;
    KXM(16);
}

static inline uintXLEN_t __rv_khmtt(uint32_t rs1, uint32_t rs2)
{
    const int rd = 0; rs1 >>= 16; rs2 >>= 16;
    KXM(16);
}

#undef KXM

#if __riscv_xlen == 64

#define GEN_KXMAB16(x, ab)                                                      \
static inline uintXLEN_t __rv_k##x##m##ab##16(intXLEN_t rs1, uintXLEN_t rs2)    \
{                                                                               \
    return ((uint32_t)__rv_k##x##m##ab(rs1, rs2))                               \
            | ((uint64_t)__rv_k##x##m##ab(rs1 >> 32, rs2 >> 32) << 32);         \
}

GEN_KXMAB16(d, bb)
GEN_KXMAB16(d, bt)
GEN_KXMAB16(d, tt)
GEN_KXMAB16(h, bb)
GEN_KXMAB16(h, bt)
GEN_KXMAB16(h, tt)

#undef GEN_KXMAB16

#define GEN_KXMAB16(x, ab)                                                                      \
static inline uintXLEN_t __rv_k##x##ma##ab##16(uintXLEN_t rd, uintXLEN_t rs1, uintXLEN_t rs2)   \
{                                                                                               \
    return ((uint32_t)__rv_k##x##ma##ab(rd, rs1, rs2))                                          \
            | ((uint64_t)__rv_k##x##ma##ab(rd >> 32, rs1 >> 32, rs2 >> 32) << 32);              \
}

GEN_KXMAB16(d, bb)
GEN_KXMAB16(d, bt)
GEN_KXMAB16(d, tt)

#undef GEN_KXMAB16

#endif // RV64 only

static inline uintXLEN_t __rv_khm8(uintXLEN_t rs1, uintXLEN_t rs2)
{
    uintXLEN_t rd = 0;
    for (size_t i = 0; i < __riscv_xlen; i += 16)
    {
        uint8_t rb = __rv_sat_q7(((intXLEN_t)(int8_t)(rs1 >> i) * (int8_t)(rs2 >> i)) >> 7);
        uint8_t rt = __rv_sat_q7(((intXLEN_t)(int8_t)(rs1 >> (i + 8)) * (int8_t)(rs2 >> (i + 8))) >> 7);
        rd |= (((uintXLEN_t)rt << 8) | (uintXLEN_t)rb) << i;
    }
    return rd;
}

static inline uintXLEN_t __rv_khmx8(uintXLEN_t rs1, uintXLEN_t rs2)
{
    uintXLEN_t rd = 0;
    for (size_t i = 0; i < __riscv_xlen; i += 16)
    {
        uint8_t rt = __rv_sat_q7(((intXLEN_t)(int8_t)(rs1 >> (i + 8)) * (int8_t)(rs2 >> i)) >> 7);
        uint8_t rb = __rv_sat_q7(((intXLEN_t)(int8_t)(rs1 >> i) * (int8_t)(rs2 >> (i + 8))) >> 7);
        rd |= (((uintXLEN_t)rt << 8) | (uintXLEN_t)rb) << i;
    }
    return rd;
}

static inline uintXLEN_t __rv_khm16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    uintXLEN_t rd = 0;
    for (size_t i = 0; i < __riscv_xlen; i += 32)
    {
        uint16_t rb = __rv_sat_q15(((intXLEN_t)(int16_t)(rs1 >> i) * (int16_t)(rs2 >> i)) >> 15);
        uint16_t rt = __rv_sat_q15(((intXLEN_t)(int16_t)(rs1 >> (i + 16)) * (int16_t)(rs2 >> (i + 16))) >> 15);
        rd |= (((uintXLEN_t)rt << 16) | (uintXLEN_t)rb) << i;
    }
    return rd;
}

static inline uintXLEN_t __rv_khmx16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    uintXLEN_t rd = 0;
    for (size_t i = 0; i < __riscv_xlen; i += 32)
    {
        uint16_t rt = __rv_sat_q15(((intXLEN_t)(int16_t)(rs1 >> (i + 16)) * (int16_t)(rs2 >> i)) >> 15);
        uint16_t rb = __rv_sat_q15(((intXLEN_t)(int16_t)(rs1 >> i) * (int16_t)(rs2 >> (i + 16))) >> 15);
        rd |= (((uintXLEN_t)rt << 16) | (uintXLEN_t)rb) << i;
    }
    return rd;
}

#define KMA                                         \
    uintXLEN_t res = 0;                             \
    for (size_t i = 0; i < __riscv_xlen; i += 32) { \
        res |= (__rv_sat_q31((int64_t)(int32_t)rd   \
                + (int64_t)(int16_t)rs1             \
                * (int64_t)(int16_t)rs2)) << i;     \
        rd >>= 32; rs1 >>= 32; rs2 >>= 32;          \
    }                                               \
    return res

static inline intXLEN_t __rv_kmabb(intXLEN_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    KMA;
}

static inline intXLEN_t __rv_kmabt(intXLEN_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    rs2 >>= 16; KMA;
}

static inline intXLEN_t __rv_kmatt(intXLEN_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    rs1 >>= 16; rs2 >>= 16; KMA;
}

#undef KMA

#define ADD64_RD_RES_CHECK_OV                       \
    if (res > 0 && rd > 0 && INT64_MAX - res < rd)  \
        return __rv_vxsat = 1, INT64_MAX;           \
    if (res < 0 && rd < 0 && INT64_MIN - res > rd)  \
        return __rv_vxsat = 1, INT64_MIN;           \
    return res + rd

#if __riscv_xlen == 64

static inline intXLEN_t __rv_kmabb32(intXLEN_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    intXLEN_t res = (int64_t)(int32_t)rs1 * (int64_t)(int32_t)rs2;
    ADD64_RD_RES_CHECK_OV;
}

static inline intXLEN_t __rv_kmabt32(intXLEN_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    intXLEN_t res = (int64_t)(int32_t)rs1 * (int64_t)(int32_t)(rs2 >> 32);
    ADD64_RD_RES_CHECK_OV;
}

static inline intXLEN_t __rv_kmatt32(intXLEN_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    intXLEN_t res = (int64_t)(int32_t)(rs1 >> 32) * (int64_t)(int32_t)(rs2 >> 32);
    ADD64_RD_RES_CHECK_OV;
}

#endif // RV64 only

#define KMD(BODY)                                   \
    intXLEN_t ret = 0;                              \
    for (size_t i = 0; i < __riscv_xlen; i += 32) { \
        ret |= __rv_sat_q31(BODY) << i;             \
    }                                               \
    return ret

static inline intXLEN_t __rv_kmda(uintXLEN_t rs1, uintXLEN_t rs2)
{
    KMD( ((int64_t)(int16_t)(rs1 >> (i)) * (int64_t)(int16_t)(rs2 >> (i))
        + (int64_t)(int16_t)(rs1 >> (i + 16)) * (int64_t)(int16_t)(rs2 >> (16 + i))));
}

static inline intXLEN_t __rv_kmxda(uintXLEN_t rs1, uintXLEN_t rs2)
{
    KMD( ((int64_t)(int16_t)(rs1 >> (i)) * (int64_t)(int16_t)(rs2 >> (16 + i))
        + (int64_t)(int16_t)(rs1 >> (i + 16)) * (int64_t)(int16_t)(rs2 >> (i))));
}

static inline intXLEN_t __rv_kmada(intXLEN_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    KMD( ((int64_t)(int16_t)(rs1 >> (i)) * (int64_t)(int16_t)(rs2 >> (i))
        + (int64_t)(int16_t)(rs1 >> (i + 16)) * (int64_t)(int16_t)(rs2 >> (16 + i)))
        + (int64_t)(int32_t) (rd >> i));
}

static inline intXLEN_t __rv_kmaxda(intXLEN_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    KMD( ((int64_t)(int16_t)(rs1 >> (i)) * (int64_t)(int16_t)(rs2 >> (16 + i))
        + (int64_t)(int16_t)(rs1 >> (16 + i)) * (int64_t)(int16_t)(rs2 >> (i)))
        + (int64_t)(int32_t) (rd >> i));
}

static inline intXLEN_t __rv_kmads(intXLEN_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    KMD(  (int64_t)(int32_t) (rd >> i) /* DO NOT MOVE! Operation order is optimized! */
        -((int64_t)(int16_t)(rs1 >> (i)) * (int64_t)(int16_t)(rs2 >> (i))
        - (int64_t)(int16_t)(rs1 >> (16 + i)) * (int64_t)(int16_t)(rs2 >> (16 + i))));
}

static inline intXLEN_t __rv_kmadrs(intXLEN_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    KMD( ((int64_t)(int16_t)(rs1 >> (i)) * (int64_t)(int16_t)(rs2 >> (i)))
        - (int64_t)(int16_t)(rs1 >> (16 + i)) * (int64_t)(int16_t)(rs2 >> (16 + i))
        + (int64_t)(int32_t) (rd >> i) );
}

static inline intXLEN_t __rv_kmaxds(intXLEN_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    KMD( ((int64_t)(int16_t)(rs1 >> (16 + i)) * (int64_t)(int16_t)(rs2 >> (i))
        - (int64_t)(int16_t)(rs1 >> (i)) * (int64_t)(int16_t)(rs2 >> (16 + i)))
        + (int64_t)(int32_t) (rd >> i) );
}

static inline intXLEN_t __rv_kmsda(intXLEN_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    KMD(-((int64_t)(int16_t)(rs1 >> (i)) * (int64_t)(int16_t)(rs2 >> (i))
        + (int64_t)(int16_t)(rs1 >> (16 + i)) * (int64_t)(int16_t)(rs2 >> (16 + i)))
        + (int64_t)(int32_t) (rd >> i) );
}

static inline intXLEN_t __rv_kmsxda(intXLEN_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    KMD(-((int64_t)(int16_t)(rs1 >> (i)) * (int64_t)(int16_t)(rs2 >> (16 + i))
        + (int64_t)(int16_t)(rs1 >> (16 + i)) * (int64_t)(int16_t)(rs2 >> (i)))
        + (int64_t)(int32_t) (rd >> i) );
}

#undef KMD

#if __riscv_xlen == 64

static inline intXLEN_t __rv_kmda32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    if (rs1 == 0x8000000080000000 && rs2 == 0x8000000080000000)
        return __rv_vxsat = 1, INT64_MAX;
    return (int64_t)(int32_t)rs1 * (int64_t)(int32_t)rs2
         + (int64_t)(int32_t)(rs1 >> 32) * (int64_t)(int32_t)(rs2 >> 32);
}

static inline intXLEN_t __rv_kmxda32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    if (rs1 == 0x8000000080000000 && rs2 == 0x8000000080000000)
        return __rv_vxsat = 1, INT64_MAX;
    return (int64_t)(int32_t)rs1 * (int64_t)(int32_t)(rs2 >> 32)
         + (int64_t)(int32_t)(rs1 >> 32) * (int64_t)(int32_t)rs2;
}

static inline intXLEN_t __rv_kmada32(intXLEN_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    if (rs1 == 0x8000000080000000 && rs2 == 0x8000000080000000)
    {
        if (rd >= 0) return __rv_vxsat = 1, INT64_MAX;
        else return (rd + 1) + INT64_MAX;
    }
    intXLEN_t res = (int64_t)(int32_t)rs1 * (int64_t)(int32_t)rs2
            + (int64_t)(int32_t)(rs1 >> 32) * (int64_t)(int32_t)(rs2 >> 32);
    ADD64_RD_RES_CHECK_OV;
}

static inline intXLEN_t __rv_kmaxda32(intXLEN_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    if (rs1 == 0x8000000080000000 && rs2 == 0x8000000080000000)
    {
        if (rd >= 0) return __rv_vxsat = 1, INT64_MAX;
        else return (rd + 1) + INT64_MAX;
    }
    intXLEN_t res = (int64_t)(int32_t)rs1 * (int64_t)(int32_t)(rs2 >> 32)
            + (int64_t)(int32_t)(rs1 >> 32) * (int64_t)(int32_t)rs2;
    ADD64_RD_RES_CHECK_OV;
}

static inline intXLEN_t __rv_kmads32(intXLEN_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    intXLEN_t res = (int64_t)(int32_t)(rs1 >> 32) * (int64_t)(int32_t)(rs2 >> 32)
                - (int64_t)(int32_t)rs1 * (int64_t)(int32_t)rs2;
    ADD64_RD_RES_CHECK_OV;
}

static inline intXLEN_t __rv_kmadrs32(intXLEN_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    intXLEN_t res = (int64_t)(int32_t)rs1 * (int64_t)(int32_t)rs2
                - (int64_t)(int32_t)(rs1 >> 32) * (int64_t)(int32_t)(rs2 >> 32);
    ADD64_RD_RES_CHECK_OV;
}

static inline intXLEN_t __rv_kmaxds32(intXLEN_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    intXLEN_t res = (int64_t)(int32_t)(rs1 >> 32) * (int64_t)(int32_t)rs2
                - (int64_t)(int32_t)rs1 * (int64_t)(int32_t)(rs2 >> 32);
    ADD64_RD_RES_CHECK_OV;
}

static inline intXLEN_t __rv_kmsda32(intXLEN_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    intXLEN_t res;
    if (rs1 == 0x8000000080000000 && rs2 == 0x8000000080000000) res = INT64_MIN;
    else res = - ((int64_t)(int32_t)rs1 * (int64_t)(int32_t)rs2
                + (int64_t)(int32_t)(rs1 >> 32) * (int64_t)(int32_t)(rs2 >> 32));
    ADD64_RD_RES_CHECK_OV;
}

static inline intXLEN_t __rv_kmsxda32(intXLEN_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    intXLEN_t res;
    if (rs1 == 0x8000000080000000 && rs2 == 0x8000000080000000) res = INT64_MIN;
    else res = - ((int64_t)(int32_t)rs1 * (int64_t)(int32_t)(rs2 >> 32)
                + (int64_t)(int32_t)rs2 * (int64_t)(int32_t)(rs1 >> 32));
    ADD64_RD_RES_CHECK_OV;
}

#endif // RV64 only

#undef ADD64_RD_RES_CHECK_OV // signed saturation helper

#define KMM(s, sh, d, u)                                    \
    intXLEN_t ret = 0;                                      \
    for (size_t i = 0; i < __riscv_xlen; i += 32) {         \
        int64_t r1 = (int32_t)(rs1 >> i);                   \
        int64_t r2 = (int##sh##_t)(rs2 >> i);               \
        int64_t res;                                        \
        if (d && r1 == INT32_MIN && r2 == INT##sh##_MIN) {  \
            res = INT32_MAX; __rv_vxsat = 1;                \
        } else {                                            \
            res = (((r1*r2) >> (sh - d - u)) + u) >> u;     \
        }                                                   \
        ret |= __rv_sat_q31((int32_t)(rd >> i) s res) << i; \
    }                                                       \
    return ret

static inline intXLEN_t __rv_kmmac(intXLEN_t rd, intXLEN_t rs1, intXLEN_t rs2)
{
    KMM(+, 32, 0, 0);
}

static inline intXLEN_t __rv_kmmac_u(intXLEN_t rd, intXLEN_t rs1, intXLEN_t rs2)
{
    KMM(+, 32, 0, 1);
}

static inline intXLEN_t __rv_kmmawb(intXLEN_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    KMM(+, 16, 0, 0);
}

static inline intXLEN_t __rv_kmmawb_u(intXLEN_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    KMM(+, 16, 0, 1);
}

static inline intXLEN_t __rv_kmmawb2(intXLEN_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    KMM(+, 16, 1, 0);
}

static inline intXLEN_t __rv_kmmawb2_u(intXLEN_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    KMM(+, 16, 1, 1);
}

static inline intXLEN_t __rv_kmmawt(intXLEN_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    rs2 >>= 16;
    KMM(+, 16, 0, 0);
}

static inline intXLEN_t __rv_kmmawt_u(intXLEN_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    rs2 >>= 16;
    KMM(+, 16, 0, 1);
}

static inline intXLEN_t __rv_kmmawt2(intXLEN_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    rs2 >>= 16;
    KMM(+, 16, 1, 0);
}

static inline intXLEN_t __rv_kmmawt2_u(intXLEN_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    rs2 >>= 16;
    KMM(+, 16, 1, 1);
}

static inline intXLEN_t __rv_kmmsb(intXLEN_t rd, intXLEN_t rs1, intXLEN_t rs2)
{
    KMM(-, 32, 0, 0);
}

static inline intXLEN_t __rv_kmmsb_u(intXLEN_t rd, intXLEN_t rs1, intXLEN_t rs2)
{
    KMM(-, 32, 0, 1);
}

static inline intXLEN_t __rv_kmmwb2(intXLEN_t rs1, uintXLEN_t rs2)
{
    intXLEN_t rd = 0; KMM(+, 16, 1, 0);
}

static inline uintXLEN_t __rv_kmmwb2_u(intXLEN_t rs1, uintXLEN_t rs2)
{
    intXLEN_t rd = 0; KMM(+, 16, 1, 1);
}

static inline uintXLEN_t __rv_kmmwt2(intXLEN_t rs1, uintXLEN_t rs2)
{
    intXLEN_t rd = 0; rs2 >>= 16;
    KMM(+, 16, 1, 0);
}

static inline uintXLEN_t __rv_kmmwt2_u(intXLEN_t rs1, uintXLEN_t rs2)
{
    intXLEN_t rd = 0; rs2 >>= 16;
    KMM(+, 16, 1, 1);
}

static inline uintXLEN_t __rv_kwmmul(uintXLEN_t rs1, uintXLEN_t rs2)
{
    intXLEN_t rd = 0; KMM(+, 32, 1, 0);
}

static inline uintXLEN_t __rv_kwmmul_u(uintXLEN_t rs1, uintXLEN_t rs2)
{
    intXLEN_t rd = 0; KMM(+, 32, 1, 1);
}

#undef KMM

#define KSLL(BODY, n)                               \
    uintXLEN_t res = 0;                             \
    for (size_t i = 0; i < __riscv_xlen; i += n) {  \
        res |= ((BODY) & UINT##n##_MAX) << i;       \
    }                                               \
    return res

static inline uintXLEN_t __rv_ksll8(uintXLEN_t rs1, uint32_t rs2)
{
    rs2 &= 7;
    KSLL(__rv_sat_q7((int64_t)(int8_t)(rs1 >> i) << rs2), 8);
}

static inline uintXLEN_t __rv_ksll16(uintXLEN_t rs1, uint32_t rs2)
{
    rs2 &= 15;
    KSLL(__rv_sat_q15((int64_t)(int16_t)(rs1 >> i) << rs2), 16);
}

#if __riscv_xlen == 64
static inline uintXLEN_t __rv_ksll32(uintXLEN_t rs1, uint32_t rs2)
{
    rs2 &= 31;
    KSLL(__rv_sat_q31((int64_t)(int32_t)(rs1 >> i) << rs2), 32);
}
#endif

#undef KSLL

static inline uintXLEN_t __rv_ksllw(int32_t rs1, uint32_t rs2)
{
    return (int32_t)__rv_sat_q31((int64_t)(int32_t)rs1 << rs2);
}

#define KSLRA(n, u, sat)                            \
    intXLEN_t ret = 0;                              \
    rs2 &= 2 * n - 1; rs2 += rs2 == n;              \
    for (size_t i = 0; i < __riscv_xlen; i += n) {  \
        int64_t res = (int##n##_t)(rs1 >> i);       \
        res = (uint##n##_t)((rs2 >= n)              \
            ? ((res >> ((2*n) - rs2 - u)) + u) >> u \
            : __rv_sat_q##sat (res << rs2));        \
        ret |= res << i;                            \
    }                                               \
    return ret

static inline uintXLEN_t __rv_kslra8(uintXLEN_t rs1, uint32_t rs2)
{
    KSLRA(8, 0, 7);
}

static inline uintXLEN_t __rv_kslra8_u(uintXLEN_t rs1, uint32_t rs2)
{
    KSLRA(8, 1, 7);
}

static inline uintXLEN_t __rv_kslra16(uintXLEN_t rs1, uint32_t rs2)
{
    KSLRA(16, 0, 15);
}

static inline uintXLEN_t __rv_kslra16_u(uintXLEN_t rs1, uint32_t rs2)
{
    KSLRA(16, 1, 15);
}

#if __riscv_xlen == 64
static inline uintXLEN_t __rv_kslra32(uintXLEN_t rs1, uint32_t rs2)
{
    KSLRA(32, 0, 31);
}

static inline uintXLEN_t __rv_kslra32_u(uintXLEN_t rs1, uint32_t rs2)
{
    KSLRA(32, 1, 31);
}
#endif

static inline intXLEN_t __rv_kslraw(int32_t rs1, int32_t rs2)
{
    KSLRA(32, 0, 31) << 32 >> 32;
}

static inline intXLEN_t __rv_kslraw_u(int32_t rs1, uint32_t rs2)
{
    KSLRA(32, 1, 31) << 32 >> 32;
}

#undef KSLRA

static inline int32_t __rv_maddr32(int32_t rd, int32_t rs1, int32_t rs2)
{
    return rd + rs1 * rs2;
}

static inline int32_t __rv_msubr32(int32_t rd, int32_t rs1, int32_t rs2)
{
    return rd - rs1 * rs2;
}

static inline intXLEN_t __rv_maxw(int32_t rs1, int32_t rs2)
{
    return rs1 > rs2 ? rs1 : rs2;
}

static inline intXLEN_t __rv_minw(int32_t rs1, int32_t rs2)
{
    return rs1 < rs2 ? rs1 : rs2;
}

#define PBSAD                                       \
    for (size_t i = 0; i < __riscv_xlen; i += 8) {  \
        uint8_t p1 = rs1 >> i, p2 = rs2 >> i;       \
        rd += (p1 > p2 ? p1 - p2 : p2 - p1);        \
    }                                               \
    return rd

static inline uintXLEN_t __rv_pbsad(uintXLEN_t rs1, uintXLEN_t rs2)
{
    uintXLEN_t rd = 0;
    PBSAD;
}

static inline uintXLEN_t __rv_pbsada(uintXLEN_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    PBSAD;
}

#undef PBSAD

// manual loop unrolling saves up to 20'000 cycles/fft (-O3 -funroll-loops)
#define PKXX16(i1, i2)                                              \
    uintXLEN_t rd = 0; uint16_t                                     \
    rs1s = rs1 >> (     16 * i1), rs2s = rs2 >> (     16 * i2);     \
    rd |= ( (((uintXLEN_t)rs1s) << 16) | (uintXLEN_t)rs2s );        \
    if (__riscv_xlen == 32) return rd; /* compile-time check */     \
    rs1s = rs1 >> (32 + 16 * i1), rs2s = rs2 >> (32 + 16 * i2);     \
    rd |= ( (((uintXLEN_t)rs1s) << 16) | (uintXLEN_t)rs2s ) << 32;  \
    return rd

static inline uintXLEN_t __rv_pkbb16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    PKXX16( 0, 0 );
}

static inline uintXLEN_t __rv_pkbt16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    PKXX16( 0, 1 );
}

static inline uintXLEN_t __rv_pktb16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    PKXX16( 1, 0 );
}

static inline uintXLEN_t __rv_pktt16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    PKXX16( 1, 1 );
}

#undef PKXX16

#if __riscv_xlen == 64

#define PKXX32(i1, i2)                                  \
    uint64_t rd = 0;                                    \
    uint32_t rs1s = (uint32_t)(rs1 >> (32*i1));         \
    uint32_t rs2s = (uint32_t)(rs2 >> (32*i2));         \
    rd |= (((uint64_t)rs1s) << 32) | (uint64_t)rs2s;    \
    return rd

static inline uintXLEN_t __rv_pkbb32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    PKXX32( 0, 0 );
}

static inline uintXLEN_t __rv_pkbt32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    PKXX32( 0, 1 );
}

static inline uintXLEN_t __rv_pktb32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    PKXX32( 1, 0 );
}

static inline uintXLEN_t __rv_pktt32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    PKXX32( 1, 1 );
}

#undef PKXX32

#endif // RV64 only

static inline uintXLEN_t __rv_rdov(void)
{
    return __rv_vxsat;
}

static inline uintXLEN_t __rv_swap8(uintXLEN_t rs1)
{
    return ((rs1 & 0xFF00FF00FF00FF00) >>  8) | ((rs1 & 0x00FF00FF00FF00FF) <<  8);
}

static inline uintXLEN_t __rv_swap16(uintXLEN_t rs1)
{
    return ((rs1 & 0xFFFF0000FFFF0000) >> 16) | ((rs1 & 0x0000FFFF0000FFFF) << 16);
}

#define CLIP(n)                                     \
    __asm__("" :: "i"(imm));  /*no register clips*/ \
    uintXLEN_t ret = 0;                             \
    for (size_t i = 0; i < __riscv_xlen; i += n) {  \
        intXLEN_t res = (int##n##_t)(rs1 >> i);     \
        res = res < min ? __rv_vxsat = 1, min : res;\
        res = res > max ? __rv_vxsat = 1, max : res;\
        ret |= (res & UINT##n##_MAX) << i;          \
    }                                               \
    return ret

__attribute__((always_inline)) static inline uintXLEN_t __rv_sclip8(uintXLEN_t rs1, uint32_t imm)
{
    int8_t max = (1 << (imm & 7)) - 1, min = -max - 1;
    CLIP(8);
}

__attribute__((always_inline)) static inline uintXLEN_t __rv_sclip16(uintXLEN_t rs1, uint32_t imm)
{
    int16_t max = (1 << (imm & 15)) - 1, min = -max - 1;
    CLIP(16);
}

__attribute__((always_inline)) static inline uintXLEN_t __rv_sclip32(uintXLEN_t rs1, uint32_t imm)
{
    int32_t max = (1 << (imm & 31)) - 1, min = -max - 1;
    CLIP(32);
}

__attribute__((always_inline)) static inline uintXLEN_t __rv_uclip8(uintXLEN_t rs1, uint32_t imm)
{
    int8_t max = (1 << (imm & 7)) - 1, min = 0;
    CLIP(8);
}

__attribute__((always_inline)) static inline uintXLEN_t __rv_uclip16(uintXLEN_t rs1, uint32_t imm)
{
    int16_t max = (1 << (imm & 15)) - 1, min = 0;
    CLIP(16);
}

__attribute__((always_inline)) static inline uintXLEN_t __rv_uclip32(uintXLEN_t rs1, uint32_t imm)
{
    int32_t max = (1 << (imm & 31)) - 1, min = 0;
    CLIP(32);
}

#undef CLIP

#define CMPL(u, s, n)                               \
    uintXLEN_t res = 0;                             \
    for (size_t i = 0; i < __riscv_xlen; i += n) {  \
        res |= (uintXLEN_t)                         \
            ((u##int##n##_t)(rs1 >> i) s /* <[=] */ \
            (u##int##n##_t)(rs2 >> i)               \
            ? UINT##n##_MAX : 0) << i;              \
    }                                               \
    return res

static inline uintXLEN_t __rv_scmple8(uintXLEN_t rs1, uintXLEN_t rs2)
{
    CMPL(, <=, 8);
}

static inline uintXLEN_t __rv_scmple16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    CMPL(, <=, 16);
}

static inline uintXLEN_t __rv_scmplt8(uintXLEN_t rs1, uintXLEN_t rs2)
{
    CMPL(, <, 8);
}

static inline uintXLEN_t __rv_scmplt16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    CMPL(, <, 16);
}

static inline uintXLEN_t __rv_ucmple8(uintXLEN_t rs1, uintXLEN_t rs2)
{
    CMPL(u, <=, 8);
}

static inline uintXLEN_t __rv_ucmple16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    CMPL(u, <=, 16);
}

static inline uintXLEN_t __rv_ucmplt8(uintXLEN_t rs1, uintXLEN_t rs2)
{
    CMPL(u, <, 8);
}

static inline uintXLEN_t __rv_ucmplt16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    CMPL(u, <, 16);
}

#undef CMPL

#define SLL(n)                                              \
    uintXLEN_t res = 0; sh &= n - 1;                        \
    for (size_t i = 0; i < __riscv_xlen; i += n) {          \
        res |= (((rs1 >> i) << sh) & UINT##n##_MAX) << i;   \
    }                                                       \
    return res

static inline uintXLEN_t __rv_sll8(uintXLEN_t rs1, uintXLEN_t sh)
{
    SLL(8);
}

static inline uintXLEN_t __rv_sll16(uintXLEN_t rs1, uintXLEN_t sh)
{
    SLL(16);
}

#if __riscv_xlen == 64
static inline uintXLEN_t __rv_sll32(uintXLEN_t rs1, uintXLEN_t sh)
{
    SLL(32);
}
#endif

#undef SLL

#define MAQA(u1, u2)                                    \
    intXLEN_t ret = 0, res, r1, r2;                     \
    for (size_t i = 0; i < __riscv_xlen; i += 32) {     \
        res = (int32_t)(rd >> i);                       \
        for (size_t j = 0; j < 4; ++j) {                \
            r1 = (u1##int8_t)rs1, r2 = (u2##int8_t)rs2; \
            res += r1 * r2; rs1 >>= 8; rs2 >>= 8;       \
        }                                               \
        ret |= (uintXLEN_t)(uint32_t)res << i;          \
    }                                                   \
    return ret

static inline uintXLEN_t __rv_smaqa(uintXLEN_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    MAQA(,);
}

static inline uintXLEN_t __rv_smaqa_su(uintXLEN_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    MAQA(, u);
}

static inline uintXLEN_t __rv_umaqa(uintXLEN_t rd, uintXLEN_t rs1, uintXLEN_t rs2)
{
    MAQA(u, u);
}

#undef MAQA

#define MM(u, s, n)                                     \
    intXLEN_t rd = 0;                                   \
    for (size_t i = 0; i < __riscv_xlen; i += n) {      \
        rd |= (uintXLEN_t)                              \
            ((u##int##n##_t)rs1 s (u##int##n##_t)rs2    \
           ? (uint##n##_t)rs1 : (uint##n##_t)rs2) << i; \
        rs1 >>= n; rs2 >>= n;                           \
    }                                                   \
    return rd

static inline uintXLEN_t __rv_smax8(uintXLEN_t rs1, uintXLEN_t rs2)
{
    MM(, >, 8);
}

static inline uintXLEN_t __rv_smax16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    MM(, >, 16);
}

#if __riscv_xlen == 64
static inline uintXLEN_t __rv_smax32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    MM(, >, 32);
}
#endif

static inline uintXLEN_t __rv_umax8(uintXLEN_t rs1, uintXLEN_t rs2)
{
    MM(u, >, 8);
}

static inline uintXLEN_t __rv_umax16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    MM(u, >, 16);
}

#if __riscv_xlen == 64
static inline uintXLEN_t __rv_umax32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    MM(u, >, 32);
}
#endif

static inline uintXLEN_t __rv_smin8(uintXLEN_t rs1, uintXLEN_t rs2)
{
    MM(, <, 8);
}

static inline uintXLEN_t __rv_smin16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    MM(, <, 16);
}

#if __riscv_xlen == 64
static inline uintXLEN_t __rv_smin32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    MM(, <, 32);
}
#endif

static inline uintXLEN_t __rv_umin8(uintXLEN_t rs1, uintXLEN_t rs2)
{
    MM(u, <, 8);
}

static inline uintXLEN_t __rv_umin16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    MM(u, <, 16);
}

#if __riscv_xlen == 64
static inline uintXLEN_t __rv_umin32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    MM(u, <, 32);
}
#endif

#undef MM

#define SM(n)                                                               \
    intXLEN_t rd = (intXLEN_t)(int##n##_t)rs1 * (intXLEN_t)(int##n##_t)rs2; \
    return (__riscv_xlen == 32 || n == 32) ? rd                             \
        : (((intXLEN_t)(int##n##_t)(rs1 >> 32) * (intXLEN_t)(int##n##_t)(rs2 >> 32)) << 32) + (rd & UINT32_MAX)

static inline uintXLEN_t __rv_smbb16(uintXLEN_t rs1, uintXLEN_t rs2) {
    SM(16);
}

static inline uintXLEN_t __rv_smbt16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    rs2 >>= 16; SM(16);
}

static inline uintXLEN_t __rv_smtt16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    rs1 >>= 16; rs2 >>= 16; SM(16);
}

#if __riscv_xlen == 64
static inline uintXLEN_t __rv_smbb32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    SM(32);
}

static inline uintXLEN_t __rv_smbt32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    rs2 >>= 32; SM(32);
}

static inline uintXLEN_t __rv_smtt32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    rs1 >>= 32; rs2 >>= 32; SM(32);
}
#endif // RV64 only

#undef SM

#define SMD(N, SET_PARTS...)                            \
    intXLEN_t n = N, SET_PARTS, rd = lt * rt - lb * rb; \
    if (__riscv_xlen == 32 || n == 32) return rd;       \
    rd &= UINT32_MAX; rs1 >>= n * 2; rs2 >>= n * 2;     \
    SET_PARTS; return rd | ((lt * rt - lb * rb) << 32)

static inline uintXLEN_t __rv_smds(uintXLEN_t rs1, uintXLEN_t rs2)
{
    SMD(16, lb = (int16_t)(rs1), lt = (int16_t)(rs1 >> 16), rb = (int16_t)(rs2), rt = (int16_t)(rs2 >> 16));
}

static inline uintXLEN_t __rv_smdrs(uintXLEN_t rs1, uintXLEN_t rs2)
{
    SMD(16, lt = (int16_t)(rs1), lb = (int16_t)(rs1 >> 16), rt = (int16_t)(rs2), rb = (int16_t)(rs2 >> 16));
}

static inline uintXLEN_t __rv_smxds(uintXLEN_t rs1, uintXLEN_t rs2)
{
    SMD(16, lb = (int16_t)(rs1), lt = (int16_t)(rs1 >> 16), rt = (int16_t)(rs2), rb = (int16_t)(rs2 >> 16));
}

#if __riscv_xlen == 64
static inline uintXLEN_t __rv_smds32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    SMD(32, lb = (int32_t)(rs1), lt = (int32_t)(rs1 >> 32), rb = (int32_t)(rs2), rt = (int32_t)(rs2 >> 32));
}

static inline uintXLEN_t __rv_smdrs32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    SMD(32, lt = (int32_t)(rs1), lb = (int32_t)(rs1 >> 32), rt = (int32_t)(rs2), rb = (int32_t)(rs2 >> 32));
}

static inline uintXLEN_t __rv_smxds32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    SMD(32, lb = (int32_t)(rs1), lt = (int32_t)(rs1 >> 32), rt = (int32_t)(rs2), rb = (int32_t)(rs2 >> 32));
}
#endif // RV64 only

#undef SMD

#define SMM(u, n)                                                                               \
    intXLEN_t rd = ((((int64_t)(int32_t)rs1 * (int64_t)(int##n##_t)rs2) >> (n - u)) + u) >> u;  \
    if (__riscv_xlen == 32) return rd;                                                          \
    return (rd & UINT32_MAX) | ((((((int64_t)(int32_t)(rs1 >> 32) * (int64_t)(int##n##_t)(rs2 >> 32)) >> (n - u)) + u) >> u) << 32)

static inline uintXLEN_t __rv_smmul(uintXLEN_t rs1, uintXLEN_t rs2)
{
    SMM(0, 32);
}

static inline uintXLEN_t __rv_smmul_u(uintXLEN_t rs1, uintXLEN_t rs2)
{
    SMM(1, 32);
}

static inline uintXLEN_t __rv_smmwb(uintXLEN_t rs1, uintXLEN_t rs2)
{
    SMM(0, 16);
}

static inline uintXLEN_t __rv_smmwb_u(uintXLEN_t rs1, uintXLEN_t rs2)
{
    SMM(1, 16);
}

static inline uintXLEN_t __rv_smmwt(uintXLEN_t rs1, uintXLEN_t rs2)
{
    rs2 >>= 16; SMM(0, 16);
}

static inline uintXLEN_t __rv_smmwt_u(uintXLEN_t rs1, uintXLEN_t rs2)
{
    rs2 >>= 16; SMM(1, 16);
}

#undef SMM

#define SR(u, ru, n)                                \
    rs2 &= n - 1;                                   \
    if (!rs2) return rs1;                           \
    intXLEN_t rd = 0;                               \
    for (size_t i = 0; i < __riscv_xlen; i += n)    \
        rd |= ((uintXLEN_t)(uint##n##_t)((          \
            (u##intXLEN_t)((u##int##n##_t)(rs1 >> i)\
                >> (rs2 - ru)) + ru) >> ru)) << i;  \
    return rd

static inline intXLEN_t __rv_sra_u(intXLEN_t rs1, uint32_t rs2)
{
    size_t XLEN = __riscv_xlen;
    SR(, 1, XLEN);
}

static inline uintXLEN_t __rv_sra8(uintXLEN_t rs1, uint32_t rs2)
{
    SR(, 0, 8);
}

static inline uintXLEN_t __rv_sra8_u(uintXLEN_t rs1, uint32_t rs2)
{
    SR(, 1, 8);
}

static inline uintXLEN_t __rv_sra16(uintXLEN_t rs1, uint32_t rs2)
{
    SR(, 0, 16);
}

static inline uintXLEN_t __rv_sra16_u(uintXLEN_t rs1, uint32_t rs2)
{
    SR(, 1, 16);
}

#if __riscv_xlen == 64
static inline uintXLEN_t __rv_sra32(uintXLEN_t rs1, uint32_t rs2)
{
    SR(, 0, 32);
}

static inline uintXLEN_t __rv_sra32_u(uintXLEN_t rs1, uint32_t rs2)
{
    SR(, 1, 32);
}

__attribute__((always_inline)) static inline int32_t __rv_sraw_u(int32_t rs1, uint32_t rs2)
{
    __asm__("" :: "i"(rs2)); // no sraw_u insn, only sraiw_u
    SR(, 1, 32);
}
#endif

static inline uintXLEN_t __rv_srl8(uintXLEN_t rs1, uint32_t rs2)
{
    SR(u, 0, 8);
}

static inline uintXLEN_t __rv_srl8_u(uintXLEN_t rs1, uint32_t rs2)
{
    SR(u, 1, 8);
}

static inline uintXLEN_t __rv_srl16(uintXLEN_t rs1, uint32_t rs2)
{
    SR(u, 0, 16);
}

static inline uintXLEN_t __rv_srl16_u(uintXLEN_t rs1, uint32_t rs2)
{
    SR(u, 1, 16);
}

#if __riscv_xlen == 64
static inline uintXLEN_t __rv_srl32(uintXLEN_t rs1, uint32_t rs2)
{
    SR(u, 0, 32);
}

static inline uintXLEN_t __rv_srl32_u(uintXLEN_t rs1, uint32_t rs2)
{
    SR(u, 1, 32);
}
#endif

#undef SR

#define ST(n, BODY1, BODY2)                             \
    uintXLEN_t rd = 0, r1, r2;                          \
    for (size_t i = 0; i < __riscv_xlen; i += 2 * n) {  \
        r1 = (uint##n##_t)(BODY1);                      \
        r2 = (uint##n##_t)(BODY2);                      \
        rd |= (((uintXLEN_t)r1 << n) | r2) << i;        \
        rs1 >>= n; rs1 >>= n; rs2 >>= n; rs2 >>= n;     \
    }                                                   \
    return rd

static inline uintXLEN_t __rv_stas16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ST(16, (rs1 >> (16)) + (rs2 >> (16)), (rs1) - (rs2));
}

static inline uintXLEN_t __rv_stsa16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ST(16, (rs1 >> (16)) - (rs2 >> (16)), (rs1) + (rs2));
}

#if __riscv_xlen == 64
static inline uintXLEN_t __rv_stas32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ST(32, (rs1 >> (32)) + (rs2 >> (32)), (rs1) - (rs2));
}

static inline uintXLEN_t __rv_stsa32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ST(32, (rs1 >> (32)) - (rs2 >> (32)), (rs1) + (rs2));
}
#endif

static inline uintXLEN_t __rv_kstas16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ST(16, __rv_sat_q15((intXLEN_t)(int16_t)(rs1 >> 16) + (intXLEN_t)(int16_t)(rs2 >> 16))
         , __rv_sat_q15((intXLEN_t)(int16_t)(rs1 >> 00) - (intXLEN_t)(int16_t)(rs2 >> 00)));
}

static inline uintXLEN_t __rv_kstsa16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ST(16, __rv_sat_q15((intXLEN_t)(int16_t)(rs1 >> 16) - (intXLEN_t)(int16_t)(rs2 >> 16))
         , __rv_sat_q15((intXLEN_t)(int16_t)(rs1 >> 00) + (intXLEN_t)(int16_t)(rs2 >> 00)));
}

#if __riscv_xlen == 64
static inline uintXLEN_t __rv_kstas32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ST(32, __rv_sat_q31((intXLEN_t)(int32_t)(rs1 >> 32) + (intXLEN_t)(int32_t)(rs2 >> 32))
         , __rv_sat_q31((intXLEN_t)(int32_t)(rs1 >> 00) - (intXLEN_t)(int32_t)(rs2 >> 00)));
}

static inline uintXLEN_t __rv_kstsa32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ST(32, __rv_sat_q31((intXLEN_t)(int32_t)(rs1 >> 32) - (intXLEN_t)(int32_t)(rs2 >> 32))
         , __rv_sat_q31((intXLEN_t)(int32_t)(rs1 >> 00) + (intXLEN_t)(int32_t)(rs2 >> 00)));
}
#endif

static inline uintXLEN_t __rv_ukstas16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ST(16, __rv_sat_u16((intXLEN_t)(uint16_t)(rs1 >> 16) + (intXLEN_t)(uint16_t)(rs2 >> 16))
         , __rv_sat_u16((intXLEN_t)(uint16_t)(rs1 >> 00) - (intXLEN_t)(uint16_t)(rs2 >> 00)));
}

static inline uintXLEN_t __rv_ukstsa16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ST(16, __rv_sat_u16((intXLEN_t)(uint16_t)(rs1 >> 16) - (intXLEN_t)(uint16_t)(rs2 >> 16))
         , __rv_sat_u16((intXLEN_t)(uint16_t)(rs1 >> 00) + (intXLEN_t)(uint16_t)(rs2 >> 00)));
}

#if __riscv_xlen == 64
static inline uintXLEN_t __rv_ukstas32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ST(32, __rv_sat_u32((intXLEN_t)(uint32_t)(rs1 >> 32) + (intXLEN_t)(uint32_t)(rs2 >> 32))
         , __rv_sat_u32((intXLEN_t)(uint32_t)(rs1 >> 00) - (intXLEN_t)(uint32_t)(rs2 >> 00)));
}

static inline uintXLEN_t __rv_ukstsa32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ST(32, __rv_sat_u32((intXLEN_t)(uint32_t)(rs1 >> 32) - (intXLEN_t)(uint32_t)(rs2 >> 32))
         , __rv_sat_u32((intXLEN_t)(uint32_t)(rs1 >> 00) + (intXLEN_t)(uint32_t)(rs2 >> 00)));
}
#endif

static inline uintXLEN_t __rv_rstas16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ST(16, ((intXLEN_t)(int16_t)(rs1 >> 16) + (intXLEN_t)(int16_t)(rs2 >> 16)) >> 1
         , ((intXLEN_t)(int16_t)(rs1 >> 00) - (intXLEN_t)(int16_t)(rs2 >> 00)) >> 1);
}

static inline uintXLEN_t __rv_rstsa16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ST(16, ((intXLEN_t)(int16_t)(rs1 >> 16) - (intXLEN_t)(int16_t)(rs2 >> 16)) >> 1
         , ((intXLEN_t)(int16_t)(rs1 >> 00) + (intXLEN_t)(int16_t)(rs2 >> 00)) >> 1);
}

#if __riscv_xlen == 64
static inline uintXLEN_t __rv_rstas32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ST(32, ((intXLEN_t)(int32_t)(rs1 >> 32) + (intXLEN_t)(int32_t)(rs2 >> 32)) >> 1
         , ((intXLEN_t)(int32_t)(rs1 >> 00) - (intXLEN_t)(int32_t)(rs2 >> 00)) >> 1);
}

static inline uintXLEN_t __rv_rstsa32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ST(32, ((intXLEN_t)(int32_t)(rs1 >> 32) - (intXLEN_t)(int32_t)(rs2 >> 32)) >> 1
         , ((intXLEN_t)(int32_t)(rs1 >> 00) + (intXLEN_t)(int32_t)(rs2 >> 00)) >> 1);
}
#endif

static inline uintXLEN_t __rv_urstas16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ST(16, ((intXLEN_t)(uint16_t)(rs1 >> 16) + (intXLEN_t)(uint16_t)(rs2 >> 16)) >> 1
         , ((intXLEN_t)(uint16_t)(rs1 >> 00) - (intXLEN_t)(uint16_t)(rs2 >> 00)) >> 1);
}

static inline uintXLEN_t __rv_urstsa16(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ST(16, ((intXLEN_t)(uint16_t)(rs1 >> 16) - (intXLEN_t)(uint16_t)(rs2 >> 16)) >> 1
         , ((intXLEN_t)(uint16_t)(rs1 >> 00) + (intXLEN_t)(uint16_t)(rs2 >> 00)) >> 1);
}

#if __riscv_xlen == 64
static inline uintXLEN_t __rv_urstas32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ST(32, ((intXLEN_t)(uint32_t)(rs1 >> 32) + (intXLEN_t)(uint32_t)(rs2 >> 32)) >> 1
         , ((intXLEN_t)(uint32_t)(rs1 >> 00) - (intXLEN_t)(uint32_t)(rs2 >> 00)) >> 1);
}

static inline uintXLEN_t __rv_urstsa32(uintXLEN_t rs1, uintXLEN_t rs2)
{
    ST(32, ((intXLEN_t)(uint32_t)(rs1 >> 32) - (intXLEN_t)(uint32_t)(rs2 >> 32)) >> 1
         , ((intXLEN_t)(uint32_t)(rs1 >> 00) + (intXLEN_t)(uint32_t)(rs2 >> 00)) >> 1);
}
#endif

#undef ST

#define UNPKD8(u, x, y)                                         \
    uintXLEN_t rd = 0;                                          \
    for (size_t i = 0; i < __riscv_xlen; i += 32) {             \
        u##int8_t bx = rs1 >> (8 * x), by = rs1 >> (8 * y);     \
        uintXLEN_t blx = (uint16_t)bx, bly = (uint16_t)by;      \
        rd |= (bly | (blx << 16)) << i; rs1 >>= 16; rs1 >>= 16; \
    }                                                           \
    return rd

static inline uintXLEN_t __rv_sunpkd810(uintXLEN_t rs1)
{
    UNPKD8(, 1, 0);
}

static inline uintXLEN_t __rv_sunpkd820(uintXLEN_t rs1)
{
    UNPKD8(, 2, 0);
}

static inline uintXLEN_t __rv_sunpkd830(uintXLEN_t rs1)
{
    UNPKD8(, 3, 0);
}

static inline uintXLEN_t __rv_sunpkd831(uintXLEN_t rs1)
{
    UNPKD8(, 3, 1);
}

static inline uintXLEN_t __rv_sunpkd832(uintXLEN_t rs1)
{
    UNPKD8(, 3, 2);
}

static inline uintXLEN_t __rv_zunpkd810(uintXLEN_t rs1)
{
    UNPKD8(u, 1, 0);
}

static inline uintXLEN_t __rv_zunpkd820(uintXLEN_t rs1)
{
    UNPKD8(u, 2, 0);
}

static inline uintXLEN_t __rv_zunpkd830(uintXLEN_t rs1)
{
    UNPKD8(u, 3, 0);
}

static inline uintXLEN_t __rv_zunpkd831(uintXLEN_t rs1)
{
    UNPKD8(u, 3, 1);
}

static inline uintXLEN_t __rv_zunpkd832(uintXLEN_t rs1)
{
    UNPKD8(u, 3, 2);
}

#undef UNPKD8
// FIXME[dzakharov]: Issue: CB-2720:: update code and remove warnings suppression
#pragma GCC diagnostic pop

#endif // ^^^  !__riscv_zpn ^^^
#endif // __RVZPNINTRIN_H__
