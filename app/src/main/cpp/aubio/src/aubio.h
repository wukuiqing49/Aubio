#ifndef AUBIO_H
#define AUBIO_H

#ifdef __cplusplus
extern "C" {
#endif

/* 基础类型和向量 */
#include "types.h"
#include "fvec.h"
#include "cvec.h"
#include "lvec.h"
#include "fmat.h"
#include "musicutils.h"
#include "vecutils.h"

/* 时域处理 */
#include "temporal/filter.h"
#include "temporal/a_weighting.h"
#include "temporal/c_weighting.h"

/* 频域处理 */
#include "spectral/fft.h"
#include "spectral/phasevoc.h"
#include "spectral/specdesc.h"
#include "spectral/awhitening.h"

/* 音高检测、节拍与 Onset */
#include "pitch/pitch.h"
#include "pitch/pitchyin.h"
#include "pitch/pitchfcomb.h"
#include "pitch/pitchmcomb.h"
#include "pitch/pitchschmitt.h"
#include "pitch/pitchspecacf.h"
#include "pitch/pitchyinfast.h"
#include "onset/onset.h"
#include "tempo/tempo.h"

/* 工具函数 */
#include "utils/log.h"
#include "utils/hist.h"
#include "utils/scale.h"

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
