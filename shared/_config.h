#pragma once

#define MPCORE_EXPORT __attribute__((visibility("default")))

#ifdef __cplusplus
#define MPCORE_EXPORT_FUNC extern "C" MPCORE_EXPORT
#else
#define MPCORE_EXPORT_FUNC MPCORE_EXPORT
#endif
