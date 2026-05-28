#include "../include/device_eval.h"

NortonStamp evalDiode(double vd, const DiodeParams& params) {
    (void)vd;
    (void)params;

    /**
     * @todo
     * I  = Is * (exp(Vd / (N * Vt)) - 1)
     * G  = (Is / (N * Vt)) * exp(Vd / (N * Vt))
     * Ieq = I - G * Vd
     * 对 Vd 做 pnjlim，避免 exp 溢出。
     */
    return {};
}

NortonStamp evalBjtNpn(double vbe, double vbc, const BjtParams& params) {
    (void)vbe;
    (void)vbc;
    (void)params;

    /**
     * @todo
     * 实现简化 Ebers-Moll 在 C-B-E 上的线性化，供 DcSolver::stampBjt 使用。
     */
    return {};
}

NortonStamp evalBjtPnp(double vbe, double vbc, const BjtParams& params) {
    (void)vbe;
    (void)vbc;
    (void)params;

    /**
     * @todo
     * PNP 版本，与 evalBjtNpn 符号约定一致。
     */
    return {};
}

NortonStamp evalMosNmos(double vgs, double vds, double vbs, const MosParams& params) {
    (void)vgs;
    (void)vds;
    (void)vbs;
    (void)params;

    /**
     * @todo
     * Shichman-Hodges Level 1：cutoff / linear / saturation 分区及 gm、gds。
     */
    return {};
}

NortonStamp evalMosPmos(double vgs, double vds, double vbs, const MosParams& params) {
    (void)vgs;
    (void)vds;
    (void)vbs;
    (void)params;

    /**
     * @todo
     * PMOS：将端电压变换到 NMOS 形式后求 Id 与偏导。
     */
    return {};
}
