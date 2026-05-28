#pragma once

#include "./model.h"

/** 诺顿伴随等效：电导 g 与等效电流 ieq（由 eval* 计算，由 DcSolver::stampNorton 写入 MNA）。 */
struct NortonStamp {
    double g = 0.0;
    double ieq = 0.0;
};

/** 热电压 Vt (V)，300 K，非线性 eval 共用。 */
constexpr double kThermalVoltage = 25.85e-3;

/**
 * @todo
 * 二极管：I = Is*(exp(Vd/(N*Vt))-1)，返回 g=dI/dVd 与 Ieq=I-G*Vd；可选 pnjlim 限制 Vd。
 */
NortonStamp evalDiode(double vd, const DiodeParams& params);

/**
 * @todo
 * NPN BJT：简化 Ebers-Moll，由 Vbe、Vbc 与 BjtParams 得到端口伴随（或 3×3 电导 + 电流源向量）。
 */
NortonStamp evalBjtNpn(double vbe, double vbc, const BjtParams& params);

/**
 * @todo
 * PNP BJT：与 NPN 对称，注意电流方向与结电压定义。
 */
NortonStamp evalBjtPnp(double vbe, double vbc, const BjtParams& params);

/**
 * @todo
 * NMOS Level 1：由 Vgs、Vds、Vbs 与 MosParams 得到 Id 及对 Vgs/Vds 的偏导，再转为 D-S 诺顿等效。
 */
NortonStamp evalMosNmos(double vgs, double vds, double vbs, const MosParams& params);

/**
 * @todo
 * PMOS Level 1：电压极性变换后调用与 NMOS 相同的分区公式。
 */
NortonStamp evalMosPmos(double vgs, double vds, double vbs, const MosParams& params);
