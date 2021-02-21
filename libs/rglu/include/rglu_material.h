#ifndef __RGLU_MATERIAL_H__
#define __RGLU_MATERIAL_H__

#include "./rglu_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

float* material_goldAmbient();
float* material_goldDiffuse();
float* material_goldSpecular();
float  material_goldShininess();

float* material_silverAmbient();
float* material_silverDiffuse();
float* material_silverSpecular();
float  material_silverShininess();

float* material_bronzeAmbient();
float* material_bronzeDiffuse();
float* material_bronzeSpecular();
float  material_bronzeShininess();

#ifdef __cplusplus
}
#endif

#endif //  __RGLU_MATERIAL_H__
