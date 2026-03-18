#include "RevoSDK/mtx.h"

ASM void PSMTX44MultVec(const register Mtx m, //r3
                        const register Vec* src, //r4
                        register Vec* dst //r5 
)
{
    #ifdef __MWERKS__ // clang-format off
    nofralloc

    psq_l       fp0, 0(src), 0, 0
    psq_l       fp2, 48(m), 0, 0
    psq_l       fp1, 8(src), 1, 0
    ps_mul      fp4, fp0, fp2
    psq_l       fp3, 56(m), 0, 0
    ps_madd     fp5, fp1, fp3, fp4
    ps_merge11  fp12, fp1, fp1
    ps_sum0     fp13, fp5, fp5, fp5
    psq_l       fp4, 0(m), 0, 0
    ps_merge00  fp13, fp13, fp13
    psq_l       fp5, 8(m), 0, 0
    ps_div f13, f12, f13
    psq_l       fp6, 16(m), 0, 0
    psq_l       fp7, 24(m), 0, 0
    psq_l       fp8, 32(m), 0, 0
    psq_l       fp9, 40(m), 0, 0
    ps_mul      fp4, fp0, fp4
    ps_madd     fp2, fp1, fp5, fp4
    ps_mul      fp6, fp0, fp6
    ps_madd     fp3, fp1, fp7, fp6
    ps_mul      fp8, fp0, fp8
    ps_sum0     fp2, fp2, fp2, fp2
    ps_madd     fp9, fp1, fp9, fp8
    ps_sum1     fp2, fp3, fp2, fp3
    ps_sum0     fp3, fp9, fp9, fp9
    ps_mul      fp2, fp2, fp13
    psq_st      fp2, 0(dst), 0, 0
    ps_mul      fp3, fp3, fp13
    psq_st      fp3, 8(dst), 1, 0

    blr
    #endif // clang-format on
}