#ifndef HG_LVN_GMATH_H
#define HG_LVN_GMATH_H

#include <stdint.h>

typedef float                   LvnType_f;
typedef double                  LvnType_d;
typedef int8_t                  LvnType_i8;
typedef uint8_t                 LvnType_ui8;
typedef int16_t                 LvnType_i16;
typedef uint16_t                LvnType_ui16;
typedef int32_t                 LvnType_i32;
typedef uint32_t                LvnType_ui32;
typedef int64_t                 LvnType_i64;
typedef uint64_t                LvnType_ui64;

#define LVN_DEFINE_VEC_TYPE(n) \
typedef LvnType_f               LvnVec##n##_f[n]; \
typedef LvnType_d               LvnVec##n##_d[n]; \
typedef LvnType_i8              LvnVec##n##_i8[n]; \
typedef LvnType_ui8             LvnVec##n##_ui8[n]; \
typedef LvnType_i16             LvnVec##n##_i16[n]; \
typedef LvnType_ui16            LvnVec##n##_ui16[n]; \
typedef LvnType_i32             LvnVec##n##_i32[n]; \
typedef LvnType_ui32            LvnVec##n##_ui32[n]; \
typedef LvnType_i64             LvnVec##n##_i64[n]; \
typedef LvnType_ui64            LvnVec##n##_ui64[n]; \
typedef LvnVec##n##_f           LvnVec##n; \
typedef LvnVec##n##_i32         LvnVec##n##i; \
typedef LvnVec##n##_ui32        LvnVec##n##ui;

#define LVN_DEFINE_MAT_TYPE(r,c) \
typedef LvnVec##c##_f           LvnMat##r##x##c##_f[r]; \
typedef LvnVec##c##_d           LvnMat##r##x##c##_d[r]; \
typedef LvnVec##c##_i8          LvnMat##r##x##c##_i8[r]; \
typedef LvnVec##c##_ui8         LvnMat##r##x##c##_ui8[r]; \
typedef LvnVec##c##_i16         LvnMat##r##x##c##_i16[r]; \
typedef LvnVec##c##_ui16        LvnMat##r##x##c##_ui16[r]; \
typedef LvnVec##c##_i32         LvnMat##r##x##c##_i32[r]; \
typedef LvnVec##c##_ui32        LvnMat##r##x##c##_ui32[r]; \
typedef LvnVec##c##_i64         LvnMat##r##x##c##_i64[r]; \
typedef LvnVec##c##_ui64        LvnMat##r##x##c##_ui64[r]; \
typedef LvnMat##r##x##c##_f     LvnMat##r##x##c; \
typedef LvnMat##r##x##c##_i32   LvnMat##r##x##c##i; \
typedef LvnMat##r##x##c##_ui32  LvnMat##r##x##c##ui;


LVN_DEFINE_VEC_TYPE(2)
LVN_DEFINE_VEC_TYPE(3)
LVN_DEFINE_VEC_TYPE(4)

LVN_DEFINE_MAT_TYPE(2,2)
LVN_DEFINE_MAT_TYPE(2,3)
LVN_DEFINE_MAT_TYPE(2,4)
LVN_DEFINE_MAT_TYPE(3,2)
LVN_DEFINE_MAT_TYPE(3,3)
LVN_DEFINE_MAT_TYPE(3,4)
LVN_DEFINE_MAT_TYPE(4,2)
LVN_DEFINE_MAT_TYPE(4,3)
LVN_DEFINE_MAT_TYPE(4,4)

typedef LvnMat2x2 LvnMat2;
typedef LvnMat3x3 LvnMat3;
typedef LvnMat4x4 LvnMat4;


#ifndef LVN_GMATH_WHITELIST_INCLUDES
#define LVN_GMATH_INCLUDE_VEC2_F
#define LVN_GMATH_INCLUDE_VEC2_D
#define LVN_GMATH_INCLUDE_VEC2_I8
#define LVN_GMATH_INCLUDE_VEC2_UI8
#define LVN_GMATH_INCLUDE_VEC2_I16
#define LVN_GMATH_INCLUDE_VEC2_UI16
#define LVN_GMATH_INCLUDE_VEC2_I32
#define LVN_GMATH_INCLUDE_VEC2_UI32
#define LVN_GMATH_INCLUDE_VEC2_I64
#define LVN_GMATH_INCLUDE_VEC2_UI64
#define LVN_GMATH_INCLUDE_VEC3_F
#define LVN_GMATH_INCLUDE_VEC3_D
#define LVN_GMATH_INCLUDE_VEC3_I8
#define LVN_GMATH_INCLUDE_VEC3_UI8
#define LVN_GMATH_INCLUDE_VEC3_I16
#define LVN_GMATH_INCLUDE_VEC3_UI16
#define LVN_GMATH_INCLUDE_VEC3_I32
#define LVN_GMATH_INCLUDE_VEC3_UI32
#define LVN_GMATH_INCLUDE_VEC3_I64
#define LVN_GMATH_INCLUDE_VEC3_UI64
#define LVN_GMATH_INCLUDE_VEC4_F
#define LVN_GMATH_INCLUDE_VEC4_D
#define LVN_GMATH_INCLUDE_VEC4_I8
#define LVN_GMATH_INCLUDE_VEC4_UI8
#define LVN_GMATH_INCLUDE_VEC4_I16
#define LVN_GMATH_INCLUDE_VEC4_UI16
#define LVN_GMATH_INCLUDE_VEC4_I32
#define LVN_GMATH_INCLUDE_VEC4_UI32
#define LVN_GMATH_INCLUDE_VEC4_I64
#define LVN_GMATH_INCLUDE_VEC4_UI64
#define LVN_GMATH_INCLUDE_MAT2_F
#define LVN_GMATH_INCLUDE_MAT2_D
#define LVN_GMATH_INCLUDE_MAT2_I8
#define LVN_GMATH_INCLUDE_MAT2_UI8
#define LVN_GMATH_INCLUDE_MAT2_I16
#define LVN_GMATH_INCLUDE_MAT2_UI16
#define LVN_GMATH_INCLUDE_MAT2_I32
#define LVN_GMATH_INCLUDE_MAT2_UI32
#define LVN_GMATH_INCLUDE_MAT2_I64
#define LVN_GMATH_INCLUDE_MAT2_UI64
#define LVN_GMATH_INCLUDE_MAT3_F
#define LVN_GMATH_INCLUDE_MAT3_D
#define LVN_GMATH_INCLUDE_MAT3_I8
#define LVN_GMATH_INCLUDE_MAT3_UI8
#define LVN_GMATH_INCLUDE_MAT3_I16
#define LVN_GMATH_INCLUDE_MAT3_UI16
#define LVN_GMATH_INCLUDE_MAT3_I32
#define LVN_GMATH_INCLUDE_MAT3_UI32
#define LVN_GMATH_INCLUDE_MAT3_I64
#define LVN_GMATH_INCLUDE_MAT3_UI64
#define LVN_GMATH_INCLUDE_MAT4_F
#define LVN_GMATH_INCLUDE_MAT4_D
#define LVN_GMATH_INCLUDE_MAT4_I8
#define LVN_GMATH_INCLUDE_MAT4_UI8
#define LVN_GMATH_INCLUDE_MAT4_I16
#define LVN_GMATH_INCLUDE_MAT4_UI16
#define LVN_GMATH_INCLUDE_MAT4_I32
#define LVN_GMATH_INCLUDE_MAT4_UI32
#define LVN_GMATH_INCLUDE_MAT4_I64
#define LVN_GMATH_INCLUDE_MAT4_UI64
#endif // !LVN_GMATH_WHITELIST_INCLUDES


#define LVN_DEFINE_VEC2_TYPE_MATH_DECL(t)                                                                              \
void lvn_vec2_##t(LvnType_##t* v, LvnVec2_##t dst);                                                                    \
void lvn_vec2_##t##_s(LvnType_##t s, LvnVec2_##t dst);                                                                 \
void lvn_vec2_##t##_zero(LvnVec2_##t dst);                                                                             \
void lvn_vec2_##t##_one(LvnVec2_##t dst);                                                                              \
void lvn_vec2_##t##_copy(LvnVec2_##t src, LvnVec2_##t dst);                                                            \
void lvn_vec2_##t##_negate(LvnVec2_##t v);                                                                             \
void lvn_vec2_##t##_negate_to(LvnVec2_##t v, LvnVec2_##t dst);                                                         \
void lvn_vec2_##t##_add(LvnVec2_##t v1, LvnVec2_##t v2, LvnVec2_##t dst);                                              \
void lvn_vec2_##t##_sub(LvnVec2_##t v1, LvnVec2_##t v2, LvnVec2_##t dst);                                              \
void lvn_vec2_##t##_mult(LvnVec2_##t v1, LvnVec2_##t v2, LvnVec2_##t dst);                                             \
void lvn_vec2_##t##_div(LvnVec2_##t v1, LvnVec2_##t v2, LvnVec2_##t dst);                                              \
void lvn_vec2_##t##_addvs(LvnVec2_##t v, LvnType_##t s, LvnVec2_##t dst);                                              \
void lvn_vec2_##t##_subvs(LvnVec2_##t v, LvnType_##t s, LvnVec2_##t dst);                                              \
void lvn_vec2_##t##_multvs(LvnVec2_##t v, LvnType_##t s, LvnVec2_##t dst);                                             \
void lvn_vec2_##t##_divvs(LvnVec2_##t v, LvnType_##t s, LvnVec2_##t dst);                                              \
void lvn_vec2_##t##_addsv(LvnType_##t s, LvnVec2_##t v, LvnVec2_##t dst);                                              \
void lvn_vec2_##t##_subsv(LvnType_##t s, LvnVec2_##t v, LvnVec2_##t dst);                                              \
void lvn_vec2_##t##_multsv(LvnType_##t s, LvnVec2_##t v, LvnVec2_##t dst);                                             \
void lvn_vec2_##t##_divsv(LvnType_##t s, LvnVec2_##t v, LvnVec2_##t dst);                                              \
void lvn_vec2_##t##_addpv(LvnVec2_##t* pv[], uint32_t count, LvnVec2_##t dst);                                         \
void lvn_vec2_##t##_subpv(LvnVec2_##t* pv[], uint32_t count, LvnVec2_##t dst);                                         \
void lvn_vec2_##t##_multpv(LvnVec2_##t* pv[], uint32_t count, LvnVec2_##t dst);                                        \
void lvn_vec2_##t##_divpv(LvnVec2_##t* pv[], uint32_t count, LvnVec2_##t dst);                                         \
LvnType_##t lvn_vec2_##t##_mag(LvnVec2_##t v);                                                                         \
LvnType_##t lvn_vec2_##t##_mag2(LvnVec2_##t v);                                                                        \
LvnType_##t lvn_vec2_##t##_dot(LvnVec2_##t v1, LvnVec2_##t v2);

#define LVN_DEFINE_VEC3_TYPE_MATH_DECL(t)                                                                              \
void lvn_vec3_##t(LvnType_##t* v, LvnVec3_##t dst);                                                                    \
void lvn_vec3_##t##_s(LvnType_##t s, LvnVec3_##t dst);                                                                 \
void lvn_vec3_##t##_zero(LvnVec3_##t dst);                                                                             \
void lvn_vec3_##t##_one(LvnVec3_##t dst);                                                                              \
void lvn_vec3_##t##_copy(LvnVec3_##t src, LvnVec3_##t dst);                                                            \
void lvn_vec3_##t##_negate(LvnVec3_##t v);                                                                             \
void lvn_vec3_##t##_negate_to(LvnVec3_##t v, LvnVec3_##t dst);                                                         \
void lvn_vec3_##t##_add(LvnVec3_##t v1, LvnVec3_##t v2, LvnVec3_##t dst);                                              \
void lvn_vec3_##t##_sub(LvnVec3_##t v1, LvnVec3_##t v2, LvnVec3_##t dst);                                              \
void lvn_vec3_##t##_mult(LvnVec3_##t v1, LvnVec3_##t v2, LvnVec3_##t dst);                                             \
void lvn_vec3_##t##_div(LvnVec3_##t v1, LvnVec3_##t v2, LvnVec3_##t dst);                                              \
void lvn_vec3_##t##_addvs(LvnVec3_##t v, LvnType_##t s, LvnVec3_##t dst);                                              \
void lvn_vec3_##t##_subvs(LvnVec3_##t v, LvnType_##t s, LvnVec3_##t dst);                                              \
void lvn_vec3_##t##_multvs(LvnVec3_##t v, LvnType_##t s, LvnVec3_##t dst);                                             \
void lvn_vec3_##t##_divvs(LvnVec3_##t v, LvnType_##t s, LvnVec3_##t dst);                                              \
void lvn_vec3_##t##_addsv(LvnType_##t s, LvnVec3_##t v, LvnVec3_##t dst);                                              \
void lvn_vec3_##t##_subsv(LvnType_##t s, LvnVec3_##t v, LvnVec3_##t dst);                                              \
void lvn_vec3_##t##_multsv(LvnType_##t s, LvnVec3_##t v, LvnVec3_##t dst);                                             \
void lvn_vec3_##t##_divsv(LvnType_##t s, LvnVec3_##t v, LvnVec3_##t dst);                                              \
void lvn_vec3_##t##_addpv(LvnVec3_##t* pv[], uint32_t count, LvnVec3_##t dst);                                         \
void lvn_vec3_##t##_subpv(LvnVec3_##t* pv[], uint32_t count, LvnVec3_##t dst);                                         \
void lvn_vec3_##t##_multpv(LvnVec3_##t* pv[], uint32_t count, LvnVec3_##t dst);                                        \
void lvn_vec3_##t##_divpv(LvnVec3_##t* pv[], uint32_t count, LvnVec3_##t dst);                                         \
void lvn_vec3_##t##_cross(LvnVec3_##t v1, LvnVec3_##t v2, LvnVec3_##t dst);                                            \
LvnType_##t lvn_vec3_##t##_mag(LvnVec3_##t v);                                                                         \
LvnType_##t lvn_vec3_##t##_mag2(LvnVec3_##t v);                                                                        \
LvnType_##t lvn_vec3_##t##_dot(LvnVec3_##t v1, LvnVec3_##t v2);

#define LVN_DEFINE_VEC4_TYPE_MATH_DECL(t)                                                                              \
void lvn_vec4_##t(LvnType_##t* v, LvnVec4_##t dst);                                                                    \
void lvn_vec4_##t##_s(LvnType_##t s, LvnVec4_##t dst);                                                                 \
void lvn_vec4_##t##_zero(LvnVec4_##t dst);                                                                             \
void lvn_vec4_##t##_one(LvnVec4_##t dst);                                                                              \
void lvn_vec4_##t##_copy(LvnVec4_##t src, LvnVec4_##t dst);                                                            \
void lvn_vec4_##t##_negate(LvnVec4_##t v);                                                                             \
void lvn_vec4_##t##_negate_to(LvnVec4_##t v, LvnVec4_##t dst);                                                         \
void lvn_vec4_##t##_add(LvnVec4_##t v1, LvnVec4_##t v2, LvnVec4_##t dst);                                              \
void lvn_vec4_##t##_sub(LvnVec4_##t v1, LvnVec4_##t v2, LvnVec4_##t dst);                                              \
void lvn_vec4_##t##_mult(LvnVec4_##t v1, LvnVec4_##t v2, LvnVec4_##t dst);                                             \
void lvn_vec4_##t##_div(LvnVec4_##t v1, LvnVec4_##t v2, LvnVec4_##t dst);                                              \
void lvn_vec4_##t##_addvs(LvnVec4_##t v, LvnType_##t s, LvnVec4_##t dst);                                              \
void lvn_vec4_##t##_subvs(LvnVec4_##t v, LvnType_##t s, LvnVec4_##t dst);                                              \
void lvn_vec4_##t##_multvs(LvnVec4_##t v, LvnType_##t s, LvnVec4_##t dst);                                             \
void lvn_vec4_##t##_divvs(LvnVec4_##t v, LvnType_##t s, LvnVec4_##t dst);                                              \
void lvn_vec4_##t##_addsv(LvnType_##t s, LvnVec4_##t v, LvnVec4_##t dst);                                              \
void lvn_vec4_##t##_subsv(LvnType_##t s, LvnVec4_##t v, LvnVec4_##t dst);                                              \
void lvn_vec4_##t##_multsv(LvnType_##t s, LvnVec4_##t v, LvnVec4_##t dst);                                             \
void lvn_vec4_##t##_divsv(LvnType_##t s, LvnVec4_##t v, LvnVec4_##t dst);                                              \
void lvn_vec4_##t##_addpv(LvnVec4_##t* pv[], uint32_t count, LvnVec4_##t dst);                                         \
void lvn_vec4_##t##_subpv(LvnVec4_##t* pv[], uint32_t count, LvnVec4_##t dst);                                         \
void lvn_vec4_##t##_multpv(LvnVec4_##t* pv[], uint32_t count, LvnVec4_##t dst);                                        \
void lvn_vec4_##t##_divpv(LvnVec4_##t* pv[], uint32_t count, LvnVec4_##t dst);                                         \
LvnType_##t lvn_vec4_##t##_mag(LvnVec4_##t v);                                                                         \
LvnType_##t lvn_vec4_##t##_mag2(LvnVec4_##t v);                                                                        \
LvnType_##t lvn_vec4_##t##_dot(LvnVec4_##t v1, LvnVec4_##t v2);

#define LVN_DEFINE_MAT2_TYPE_MATH_DECL(t)                                                                              \
void lvn_mat2_##t(LvnType_##t* v, LvnMat2x2_##t dst);                                                                  \
void lvn_mat2_##t##_zero(LvnMat2x2_##t dst);                                                                           \
void lvn_mat2_##t##_identity(LvnMat2x2_##t dst);                                                                       \
void lvn_mat2_##t##_copy(LvnMat2x2_##t src, LvnMat2x2_##t dst);                                                        \
void lvn_mat2_##t##_inv(LvnMat2x2_##t mat, LvnMat2x2_##t dst);                                                         \
void lvn_mat2_##t##_transpose_to(LvnMat2x2_##t m, LvnMat2x2_##t dst);                                                  \
void lvn_mat2_##t##_transpose(LvnMat2x2_##t m);                                                                        \
void lvn_mat2_##t##_negate_to(LvnMat2x2_##t m, LvnMat2x2_##t dst);                                                     \
void lvn_mat2_##t##_negate(LvnMat2x2_##t m);                                                                           \
void lvn_mat2_##t##_add(LvnMat2x2_##t m1, LvnMat2x2_##t m2, LvnMat2x2_##t dst);                                        \
void lvn_mat2_##t##_sub(LvnMat2x2_##t m1, LvnMat2x2_##t m2, LvnMat2x2_##t dst);                                        \
void lvn_mat2_##t##_mult(LvnMat2x2_##t m1, LvnMat2x2_##t m2, LvnMat2x2_##t dst);                                       \
void lvn_mat2_##t##_multmv(LvnMat2x2_##t m, LvnVec2_##t v, LvnVec2_##t dst);                                           \
void lvn_mat2_##t##_multvm(LvnVec2_##t v, LvnMat2x2_##t m, LvnVec2_##t dst);                                           \
void lvn_mat2_##t##_addms(LvnMat2x2_##t m, LvnType_##t s, LvnMat2x2_##t dst);                                          \
void lvn_mat2_##t##_subms(LvnMat2x2_##t m, LvnType_##t s, LvnMat2x2_##t dst);                                          \
void lvn_mat2_##t##_multms(LvnMat2x2_##t m, LvnType_##t s, LvnMat2x2_##t dst);                                         \
void lvn_mat2_##t##_divms(LvnMat2x2_##t m, LvnType_##t s, LvnMat2x2_##t dst);                                          \
void lvn_mat2_##t##_addsm(LvnType_##t s, LvnMat2x2_##t m, LvnMat2x2_##t dst);                                          \
void lvn_mat2_##t##_subsm(LvnType_##t s, LvnMat2x2_##t m, LvnMat2x2_##t dst);                                          \
void lvn_mat2_##t##_multsm(LvnType_##t s, LvnMat2x2_##t m, LvnMat2x2_##t dst);                                         \
void lvn_mat2_##t##_divsm(LvnType_##t s, LvnMat2x2_##t m, LvnMat2x2_##t dst);

#define LVN_DEFINE_MAT3_TYPE_MATH_DECL(t)                                                                              \
void lvn_mat3_##t(LvnType_##t* v, LvnMat3x3_##t dst);                                                                  \
void lvn_mat3_##t##_zero(LvnMat3x3_##t dst);                                                                           \
void lvn_mat3_##t##_identity(LvnMat3x3_##t dst);                                                                       \
void lvn_mat3_##t##_copy(LvnMat3x3_##t src, LvnMat3x3_##t dst);                                                        \
void lvn_mat3_##t##_inv(LvnMat3x3_##t mat, LvnMat3x3_##t dst);                                                         \
void lvn_mat3_##t##_transpose_to(LvnMat3x3_##t m, LvnMat3x3_##t dst);                                                  \
void lvn_mat3_##t##_transpose(LvnMat3x3_##t m);                                                                        \
void lvn_mat3_##t##_negate_to(LvnMat3x3_##t m, LvnMat3x3_##t dst);                                                     \
void lvn_mat3_##t##_negate(LvnMat3x3_##t m);                                                                           \
void lvn_mat3_##t##_add(LvnMat3x3_##t m1, LvnMat3x3_##t m2, LvnMat3x3_##t dst);                                        \
void lvn_mat3_##t##_sub(LvnMat3x3_##t m1, LvnMat3x3_##t m2, LvnMat3x3_##t dst);                                        \
void lvn_mat3_##t##_mult(LvnMat3x3_##t m1, LvnMat3x3_##t m2, LvnMat3x3_##t dst);                                       \
void lvn_mat3_##t##_multmv(LvnMat3x3_##t m, LvnVec3_##t v, LvnVec3_##t dst);                                           \
void lvn_mat3_##t##_multvm(LvnVec3_##t v, LvnMat3x3_##t m, LvnVec3_##t dst);                                           \
void lvn_mat3_##t##_addms(LvnMat3x3_##t m, LvnType_##t s, LvnMat3x3_##t dst);                                          \
void lvn_mat3_##t##_subms(LvnMat3x3_##t m, LvnType_##t s, LvnMat3x3_##t dst);                                          \
void lvn_mat3_##t##_multms(LvnMat3x3_##t m, LvnType_##t s, LvnMat3x3_##t dst);                                         \
void lvn_mat3_##t##_divms(LvnMat3x3_##t m, LvnType_##t s, LvnMat3x3_##t dst);                                          \
void lvn_mat3_##t##_addsm(LvnType_##t s, LvnMat3x3_##t m, LvnMat3x3_##t dst);                                          \
void lvn_mat3_##t##_subsm(LvnType_##t s, LvnMat3x3_##t m, LvnMat3x3_##t dst);                                          \
void lvn_mat3_##t##_multsm(LvnType_##t s, LvnMat3x3_##t m, LvnMat3x3_##t dst);                                         \
void lvn_mat3_##t##_divsm(LvnType_##t s, LvnMat3x3_##t m, LvnMat3x3_##t dst);

#define LVN_DEFINE_MAT4_TYPE_MATH_DECL(t)                                                                              \
void lvn_mat4_##t(LvnType_##t* v, LvnMat4x4_##t dst);                                                                  \
void lvn_mat4_##t##_zero(LvnMat4x4_##t dst);                                                                           \
void lvn_mat4_##t##_identity(LvnMat4x4_##t dst);                                                                       \
void lvn_mat4_##t##_copy(LvnMat4x4_##t src, LvnMat4x4_##t dst);                                                        \
void lvn_mat4_##t##_inv(LvnMat4x4_##t mat, LvnMat4x4_##t dst);                                                         \
void lvn_mat4_##t##_transpose_to(LvnMat4x4_##t m, LvnMat4x4_##t dst);                                                  \
void lvn_mat4_##t##_transpose(LvnMat4x4_##t m);                                                                        \
void lvn_mat4_##t##_negate_to(LvnMat4x4_##t m, LvnMat4x4_##t dst);                                                     \
void lvn_mat4_##t##_negate(LvnMat4x4_##t m);                                                                           \
void lvn_mat4_##t##_add(LvnMat4x4_##t m1, LvnMat4x4_##t m2, LvnMat4x4_##t dst);                                        \
void lvn_mat4_##t##_sub(LvnMat4x4_##t m1, LvnMat4x4_##t m2, LvnMat4x4_##t dst);                                        \
void lvn_mat4_##t##_mult(LvnMat4x4_##t m1, LvnMat4x4_##t m2, LvnMat4x4_##t dst);                                       \
void lvn_mat4_##t##_multmv(LvnMat4x4_##t m, LvnVec4_##t v, LvnVec4_##t dst);                                           \
void lvn_mat4_##t##_multvm(LvnVec4_##t v, LvnMat4x4_##t m, LvnVec4_##t dst);                                           \
void lvn_mat4_##t##_addms(LvnMat4x4_##t m, LvnType_##t s, LvnMat4x4_##t dst);                                          \
void lvn_mat4_##t##_subms(LvnMat4x4_##t m, LvnType_##t s, LvnMat4x4_##t dst);                                          \
void lvn_mat4_##t##_multms(LvnMat4x4_##t m, LvnType_##t s, LvnMat4x4_##t dst);                                         \
void lvn_mat4_##t##_divms(LvnMat4x4_##t m, LvnType_##t s, LvnMat4x4_##t dst);                                          \
void lvn_mat4_##t##_addsm(LvnType_##t s, LvnMat4x4_##t m, LvnMat4x4_##t dst);                                          \
void lvn_mat4_##t##_subsm(LvnType_##t s, LvnMat4x4_##t m, LvnMat4x4_##t dst);                                          \
void lvn_mat4_##t##_multsm(LvnType_##t s, LvnMat4x4_##t m, LvnMat4x4_##t dst);                                         \
void lvn_mat4_##t##_divsm(LvnType_##t s, LvnMat4x4_##t m, LvnMat4x4_##t dst);

#define LVN_DEFINE_VEC_F_TYPE_MATH_DECL(n)                                                                             \
void lvn_vec##n(float* v, LvnVec##n dst);                                                                              \
void lvn_vec##n##_s(float s, LvnVec##n dst);                                                                           \
void lvn_vec##n##_zero(LvnVec##n dst);                                                                                 \
void lvn_vec##n##_one(LvnVec##n dst);                                                                                  \
void lvn_vec##n##_copy(LvnVec##n src, LvnVec##n dst);                                                                  \
void lvn_vec##n##_negate(LvnVec##n v);                                                                                 \
void lvn_vec##n##_negate_to(LvnVec##n v, LvnVec##n dst);                                                               \
void lvn_vec##n##_add(LvnVec##n v1, LvnVec##n v2, LvnVec##n dst);                                                      \
void lvn_vec##n##_sub(LvnVec##n v1, LvnVec##n v2, LvnVec##n dst);                                                      \
void lvn_vec##n##_mult(LvnVec##n v1, LvnVec##n v2, LvnVec##n dst);                                                     \
void lvn_vec##n##_div(LvnVec##n v1, LvnVec##n v2, LvnVec##n dst);                                                      \
void lvn_vec##n##_addvs(LvnVec##n v, float s, LvnVec##n dst);                                                          \
void lvn_vec##n##_subvs(LvnVec##n v, float s, LvnVec##n dst);                                                          \
void lvn_vec##n##_multvs(LvnVec##n v, float s, LvnVec##n dst);                                                         \
void lvn_vec##n##_divvs(LvnVec##n v, float s, LvnVec##n dst);                                                          \
void lvn_vec##n##_addsv(float s, LvnVec##n v, LvnVec##n dst);                                                          \
void lvn_vec##n##_subsv(float s, LvnVec##n v, LvnVec##n dst);                                                          \
void lvn_vec##n##_multsv(float s, LvnVec##n v, LvnVec##n dst);                                                         \
void lvn_vec##n##_divsv(float s, LvnVec##n v, LvnVec##n dst);                                                          \
void lvn_vec##n##_addpv(LvnVec##n* pv[], uint32_t count, LvnVec##n dst);                                               \
void lvn_vec##n##_subpv(LvnVec##n* pv[], uint32_t count, LvnVec##n dst);                                               \
void lvn_vec##n##_multpv(LvnVec##n* pv[], uint32_t count, LvnVec##n dst);                                              \
void lvn_vec##n##_divpv(LvnVec##n* pv[], uint32_t count, LvnVec##n dst);                                               \
float lvn_vec##n##_mag(LvnVec##n v);                                                                                   \
float lvn_vec##n##_mag2(LvnVec##n v);                                                                                  \
float lvn_vec##n##_dot(LvnVec##n v1, LvnVec##n v2);                                                                    \

#define LVN_DEFINE_VEC_I32_TYPE_MATH_DECL(n)                                                                           \
void lvn_vec##n##i(int32_t* v, LvnVec##n##i dst);                                                                      \
void lvn_vec##n##i_s(int32_t s, LvnVec##n##i dst);                                                                     \
void lvn_vec##n##i_zero(LvnVec##n##i dst);                                                                             \
void lvn_vec##n##i_one(LvnVec##n##i dst);                                                                              \
void lvn_vec##n##i_copy(LvnVec##n##i src, LvnVec##n##i dst);                                                           \
void lvn_vec##n##i_negate(LvnVec##n##i v);                                                                             \
void lvn_vec##n##i_negate_to(LvnVec##n##i v, LvnVec##n##i dst);                                                        \
void lvn_vec##n##i_add(LvnVec##n##i v1, LvnVec##n##i v2, LvnVec##n##i dst);                                            \
void lvn_vec##n##i_sub(LvnVec##n##i v1, LvnVec##n##i v2, LvnVec##n##i dst);                                            \
void lvn_vec##n##i_mult(LvnVec##n##i v1, LvnVec##n##i v2, LvnVec##n##i dst);                                           \
void lvn_vec##n##i_div(LvnVec##n##i v1, LvnVec##n##i v2, LvnVec##n##i dst);                                            \
void lvn_vec##n##i_addvs(LvnVec##n##i v, int32_t s, LvnVec##n##i dst);                                                 \
void lvn_vec##n##i_subvs(LvnVec##n##i v, int32_t s, LvnVec##n##i dst);                                                 \
void lvn_vec##n##i_multvs(LvnVec##n##i v, int32_t s, LvnVec##n##i dst);                                                \
void lvn_vec##n##i_divvs(LvnVec##n##i v, int32_t s, LvnVec##n##i dst);                                                 \
void lvn_vec##n##i_addsv(int32_t s, LvnVec##n##i v, LvnVec##n##i dst);                                                 \
void lvn_vec##n##i_subsv(int32_t s, LvnVec##n##i v, LvnVec##n##i dst);                                                 \
void lvn_vec##n##i_multsv(int32_t s, LvnVec##n##i v, LvnVec##n##i dst);                                                \
void lvn_vec##n##i_divsv(int32_t s, LvnVec##n##i v, LvnVec##n##i dst);                                                 \
void lvn_vec##n##i_addpv(LvnVec##n##i* pv[], uint32_t count, LvnVec##n##i dst);                                        \
void lvn_vec##n##i_subpv(LvnVec##n##i* pv[], uint32_t count, LvnVec##n##i dst);                                        \
void lvn_vec##n##i_multpv(LvnVec##n##i* pv[], uint32_t count, LvnVec##n##i dst);                                       \
void lvn_vec##n##i_divpv(LvnVec##n##i* pv[], uint32_t count, LvnVec##n##i dst);                                        \
int32_t lvn_vec##n##i_mag(LvnVec##n##i v);                                                                             \
int32_t lvn_vec##n##i_mag2(LvnVec##n##i v);                                                                            \
int32_t lvn_vec##n##i_dot(LvnVec##n##i v1, LvnVec##n##i v2);                                                           \

#define LVN_DEFINE_VEC_UI32_TYPE_MATH_DECL(n)                                                                          \
void lvn_vec##n##ui(uint32_t* v, LvnVec##n##ui dst);                                                                   \
void lvn_vec##n##ui_s(uint32_t s, LvnVec##n##ui dst);                                                                  \
void lvn_vec##n##ui_zero(LvnVec##n##ui dst);                                                                           \
void lvn_vec##n##ui_one(LvnVec##n##ui dst);                                                                            \
void lvn_vec##n##ui_copy(LvnVec##n##ui src, LvnVec##n##ui dst);                                                        \
void lvn_vec##n##ui_negate(LvnVec##n##ui v);                                                                           \
void lvn_vec##n##ui_negate_to(LvnVec##n##ui v, LvnVec##n##ui dst);                                                     \
void lvn_vec##n##ui_add(LvnVec##n##ui v1, LvnVec##n##ui v2, LvnVec##n##ui dst);                                        \
void lvn_vec##n##ui_sub(LvnVec##n##ui v1, LvnVec##n##ui v2, LvnVec##n##ui dst);                                        \
void lvn_vec##n##ui_mult(LvnVec##n##ui v1, LvnVec##n##ui v2, LvnVec##n##ui dst);                                       \
void lvn_vec##n##ui_div(LvnVec##n##ui v1, LvnVec##n##ui v2, LvnVec##n##ui dst);                                        \
void lvn_vec##n##ui_addvs(LvnVec##n##ui v, uint32_t s, LvnVec##n##ui dst);                                             \
void lvn_vec##n##ui_subvs(LvnVec##n##ui v, uint32_t s, LvnVec##n##ui dst);                                             \
void lvn_vec##n##ui_multvs(LvnVec##n##ui v, uint32_t s, LvnVec##n##ui dst);                                            \
void lvn_vec##n##ui_divvs(LvnVec##n##ui v, uint32_t s, LvnVec##n##ui dst);                                             \
void lvn_vec##n##ui_addsv(uint32_t s, LvnVec##n##ui v, LvnVec##n##ui dst);                                             \
void lvn_vec##n##ui_subsv(uint32_t s, LvnVec##n##ui v, LvnVec##n##ui dst);                                             \
void lvn_vec##n##ui_multsv(uint32_t s, LvnVec##n##ui v, LvnVec##n##ui dst);                                            \
void lvn_vec##n##ui_divsv(uint32_t s, LvnVec##n##ui v, LvnVec##n##ui dst);                                             \
void lvn_vec##n##ui_addpv(LvnVec##n##ui* pv[], uint32_t count, LvnVec##n##ui dst);                                     \
void lvn_vec##n##ui_subpv(LvnVec##n##ui* pv[], uint32_t count, LvnVec##n##ui dst);                                     \
void lvn_vec##n##ui_multpv(LvnVec##n##ui* pv[], uint32_t count, LvnVec##n##ui dst);                                    \
void lvn_vec##n##ui_divpv(LvnVec##n##ui* pv[], uint32_t count, LvnVec##n##ui dst);                                     \
uint32_t lvn_vec##n##ui_mag(LvnVec##n##ui v);                                                                          \
uint32_t lvn_vec##n##ui_mag2(LvnVec##n##ui v);                                                                         \
uint32_t lvn_vec##n##ui_dot(LvnVec##n##ui v1, LvnVec##n##ui v2);                                                       \

#define LVN_DEFINE_MAT_F_TYPE_MATH_DECL(n)                                                                             \
void lvn_mat##n(float* v, LvnMat##n##x##n dst);                                                                        \
void lvn_mat##n##_zero(LvnMat##n##x##n dst);                                                                           \
void lvn_mat##n##_identity(LvnMat##n##x##n dst);                                                                       \
void lvn_mat##n##_copy(LvnMat##n##x##n src, LvnMat##n##x##n dst);                                                      \
void lvn_mat##n##_inv(LvnMat##n##x##n mat, LvnMat##n##x##n dst);                                                       \
void lvn_mat##n##_transpose_to(LvnMat##n##x##n m, LvnMat##n##x##n dst);                                                \
void lvn_mat##n##_transpose(LvnMat##n##x##n m);                                                                        \
void lvn_mat##n##_negate_to(LvnMat##n##x##n m, LvnMat##n##x##n dst);                                                   \
void lvn_mat##n##_negate(LvnMat##n##x##n m);                                                                           \
void lvn_mat##n##_add(LvnMat##n##x##n m1, LvnMat##n##x##n m2, LvnMat##n##x##n dst);                                    \
void lvn_mat##n##_sub(LvnMat##n##x##n m1, LvnMat##n##x##n m2, LvnMat##n##x##n dst);                                    \
void lvn_mat##n##_mult(LvnMat##n##x##n m1, LvnMat##n##x##n m2, LvnMat##n##x##n dst);                                   \
void lvn_mat##n##_multmv(LvnMat##n##x##n m, LvnVec##n v, LvnVec##n dst);                                               \
void lvn_mat##n##_multvm(LvnVec##n v, LvnMat##n##x##n m, LvnVec##n dst);                                               \
void lvn_mat##n##_addms(LvnMat##n##x##n m, float s, LvnMat##n##x##n dst);                                              \
void lvn_mat##n##_subms(LvnMat##n##x##n m, float s, LvnMat##n##x##n dst);                                              \
void lvn_mat##n##_multms(LvnMat##n##x##n m, float s, LvnMat##n##x##n dst);                                             \
void lvn_mat##n##_divms(LvnMat##n##x##n m, float s, LvnMat##n##x##n dst);                                              \
void lvn_mat##n##_addsm(float s, LvnMat##n##x##n m, LvnMat##n##x##n dst);                                              \
void lvn_mat##n##_subsm(float s, LvnMat##n##x##n m, LvnMat##n##x##n dst);                                              \
void lvn_mat##n##_multsm(float s, LvnMat##n##x##n m, LvnMat##n##x##n dst);                                             \
void lvn_mat##n##_divsm(float s, LvnMat##n##x##n m, LvnMat##n##x##n dst);

#define LVN_DEFINE_MAT_I32_TYPE_MATH_DECL(n)                                                                           \
void lvn_mat##n##i(int32_t* v, LvnMat##n##x##n##i dst);                                                                \
void lvn_mat##n##i_zero(LvnMat##n##x##n##i dst);                                                                       \
void lvn_mat##n##i_identity(LvnMat##n##x##n##i dst);                                                                   \
void lvn_mat##n##i_copy(LvnMat##n##x##n##i src, LvnMat##n##x##n##i dst);                                               \
void lvn_mat##n##i_inv(LvnMat##n##x##n##i mat, LvnMat##n##x##n##i dst);                                                \
void lvn_mat##n##i_transpose_to(LvnMat##n##x##n##i m, LvnMat##n##x##n##i dst);                                         \
void lvn_mat##n##i_transpose(LvnMat##n##x##n##i m);                                                                    \
void lvn_mat##n##i_negate_to(LvnMat##n##x##n##i m, LvnMat##n##x##n##i dst);                                            \
void lvn_mat##n##i_negate(LvnMat##n##x##n##i m);                                                                       \
void lvn_mat##n##i_add(LvnMat##n##x##n##i m1, LvnMat##n##x##n##i m2, LvnMat##n##x##n##i dst);                          \
void lvn_mat##n##i_sub(LvnMat##n##x##n##i m1, LvnMat##n##x##n##i m2, LvnMat##n##x##n##i dst);                          \
void lvn_mat##n##i_mult(LvnMat##n##x##n##i m1, LvnMat##n##x##n##i m2, LvnMat##n##x##n##i dst);                         \
void lvn_mat##n##i_multmv(LvnMat##n##x##n##i m, LvnVec##n##i v, LvnVec##n##i dst);                                     \
void lvn_mat##n##i_multvm(LvnVec##n##i v, LvnMat##n##x##n##i m, LvnVec##n##i dst);                                     \
void lvn_mat##n##i_addms(LvnMat##n##x##n##i m, int32_t s, LvnMat##n##x##n##i dst);                                     \
void lvn_mat##n##i_subms(LvnMat##n##x##n##i m, int32_t s, LvnMat##n##x##n##i dst);                                     \
void lvn_mat##n##i_multms(LvnMat##n##x##n##i m, int32_t s, LvnMat##n##x##n##i dst);                                    \
void lvn_mat##n##i_divms(LvnMat##n##x##n##i m, int32_t s, LvnMat##n##x##n##i dst);                                     \
void lvn_mat##n##i_addsm(int32_t s, LvnMat##n##x##n##i m, LvnMat##n##x##n##i dst);                                     \
void lvn_mat##n##i_subsm(int32_t s, LvnMat##n##x##n##i m, LvnMat##n##x##n##i dst);                                     \
void lvn_mat##n##i_multsm(int32_t s, LvnMat##n##x##n##i m, LvnMat##n##x##n##i dst);                                    \
void lvn_mat##n##i_divsm(int32_t s, LvnMat##n##x##n##i m, LvnMat##n##x##n##i dst);

#define LVN_DEFINE_MAT_UI32_TYPE_MATH_DECL(n)                                                                          \
void lvn_mat##n##ui(uint32_t* v, LvnMat##n##x##n##ui dst);                                                             \
void lvn_mat##n##ui_zero(LvnMat##n##x##n##ui dst);                                                                     \
void lvn_mat##n##ui_identity(LvnMat##n##x##n##ui dst);                                                                 \
void lvn_mat##n##ui_copy(LvnMat##n##x##n##ui src, LvnMat##n##x##n##ui dst);                                            \
void lvn_mat##n##ui_inv(LvnMat##n##x##n##ui mat, LvnMat##n##x##n##ui dst);                                             \
void lvn_mat##n##ui_transpose_to(LvnMat##n##x##n##ui m, LvnMat##n##x##n##ui dst);                                      \
void lvn_mat##n##ui_transpose(LvnMat##n##x##n##ui m);                                                                  \
void lvn_mat##n##ui_negate_to(LvnMat##n##x##n##ui m, LvnMat##n##x##n##ui dst);                                         \
void lvn_mat##n##ui_negate(LvnMat##n##x##n##ui m);                                                                     \
void lvn_mat##n##ui_add(LvnMat##n##x##n##ui m1, LvnMat##n##x##n##ui m2, LvnMat##n##x##n##ui dst);                      \
void lvn_mat##n##ui_sub(LvnMat##n##x##n##ui m1, LvnMat##n##x##n##ui m2, LvnMat##n##x##n##ui dst);                      \
void lvn_mat##n##ui_mult(LvnMat##n##x##n##ui m1, LvnMat##n##x##n##ui m2, LvnMat##n##x##n##ui dst);                     \
void lvn_mat##n##ui_multmv(LvnMat##n##x##n##ui m, LvnVec##n##ui v, LvnVec##n##ui dst);                                 \
void lvn_mat##n##ui_multvm(LvnVec##n##ui v, LvnMat##n##x##n##ui m, LvnVec##n##ui dst);                                 \
void lvn_mat##n##ui_addms(LvnMat##n##x##n##ui m, uint32_t s, LvnMat##n##x##n##ui dst);                                 \
void lvn_mat##n##ui_subms(LvnMat##n##x##n##ui m, uint32_t s, LvnMat##n##x##n##ui dst);                                 \
void lvn_mat##n##ui_multms(LvnMat##n##x##n##ui m, uint32_t s, LvnMat##n##x##n##ui dst);                                \
void lvn_mat##n##ui_divms(LvnMat##n##x##n##ui m, uint32_t s, LvnMat##n##x##n##ui dst);                                 \
void lvn_mat##n##ui_addsm(uint32_t s, LvnMat##n##x##n##ui m, LvnMat##n##x##n##ui dst);                                 \
void lvn_mat##n##ui_subsm(uint32_t s, LvnMat##n##x##n##ui m, LvnMat##n##x##n##ui dst);                                 \
void lvn_mat##n##ui_multsm(uint32_t s, LvnMat##n##x##n##ui m, LvnMat##n##x##n##ui dst);                                \
void lvn_mat##n##ui_divsm(uint32_t s, LvnMat##n##x##n##ui m, LvnMat##n##x##n##ui dst);



#define LVN_DEFINE_VEC2_TYPE_MATH_IMPL(t)                                                                              \
void lvn_vec2_##t(LvnType_##t* v, LvnVec2_##t dst) {                                                                   \
    dst[0] = v[0];                                                                                                     \
    dst[1] = v[1];                                                                                                     \
}                                                                                                                      \
void lvn_vec2_##t##_s(LvnType_##t s, LvnVec2_##t dst) {                                                                \
    dst[0] = s;                                                                                                        \
    dst[1] = s;                                                                                                        \
}                                                                                                                      \
void lvn_vec2_##t##_zero(LvnVec2_##t dst) {                                                                            \
    dst[0] = (LvnType_##t)0;                                                                                           \
    dst[1] = (LvnType_##t)0;                                                                                           \
}                                                                                                                      \
void lvn_vec2_##t##_one(LvnVec2_##t dst) {                                                                             \
    dst[0] = (LvnType_##t)1;                                                                                           \
    dst[1] = (LvnType_##t)1;                                                                                           \
}                                                                                                                      \
void lvn_vec2_##t##_copy(LvnVec2_##t src, LvnVec2_##t dst) {                                                           \
    dst[0] = src[0];                                                                                                   \
    dst[1] = src[1];                                                                                                   \
}                                                                                                                      \
void lvn_vec2_##t##_negate(LvnVec2_##t v) {                                                                            \
    v[0] = -v[0];                                                                                                      \
    v[1] = -v[1];                                                                                                      \
}                                                                                                                      \
void lvn_vec2_##t##_negate_to(LvnVec2_##t v, LvnVec2_##t dst) {                                                        \
    dst[0] = -v[0];                                                                                                    \
    dst[1] = -v[1];                                                                                                    \
}                                                                                                                      \
void lvn_vec2_##t##_add(LvnVec2_##t v1, LvnVec2_##t v2, LvnVec2_##t dst) {                                             \
    dst[0] = v1[0] + v2[0];                                                                                            \
    dst[1] = v1[1] + v2[1];                                                                                            \
}                                                                                                                      \
void lvn_vec2_##t##_sub(LvnVec2_##t v1, LvnVec2_##t v2, LvnVec2_##t dst) {                                             \
    dst[0] = v1[0] - v2[0];                                                                                            \
    dst[1] = v1[1] - v2[1];                                                                                            \
}                                                                                                                      \
void lvn_vec2_##t##_mult(LvnVec2_##t v1, LvnVec2_##t v2, LvnVec2_##t dst) {                                            \
    dst[0] = v1[0] * v2[0];                                                                                            \
    dst[1] = v1[1] * v2[1];                                                                                            \
}                                                                                                                      \
void lvn_vec2_##t##_div(LvnVec2_##t v1, LvnVec2_##t v2, LvnVec2_##t dst) {                                             \
    dst[0] = v1[0] / v2[0];                                                                                            \
    dst[1] = v1[1] / v2[1];                                                                                            \
}                                                                                                                      \
void lvn_vec2_##t##_addvs(LvnVec2_##t v, LvnType_##t s, LvnVec2_##t dst) {                                             \
    dst[0] = v[0] + s;                                                                                                 \
    dst[1] = v[1] + s;                                                                                                 \
}                                                                                                                      \
void lvn_vec2_##t##_subvs(LvnVec2_##t v, LvnType_##t s, LvnVec2_##t dst) {                                             \
    dst[0] = v[0] - s;                                                                                                 \
    dst[1] = v[1] - s;                                                                                                 \
}                                                                                                                      \
void lvn_vec2_##t##_multvs(LvnVec2_##t v, LvnType_##t s, LvnVec2_##t dst) {                                            \
    dst[0] = v[0] * s;                                                                                                 \
    dst[1] = v[1] * s;                                                                                                 \
}                                                                                                                      \
void lvn_vec2_##t##_divvs(LvnVec2_##t v, LvnType_##t s, LvnVec2_##t dst) {                                             \
    dst[0] = v[0] / s;                                                                                                 \
    dst[1] = v[1] / s;                                                                                                 \
}                                                                                                                      \
void lvn_vec2_##t##_addsv(LvnType_##t s, LvnVec2_##t v, LvnVec2_##t dst) {                                             \
    dst[0] = s + v[0];                                                                                                 \
    dst[1] = s + v[1];                                                                                                 \
}                                                                                                                      \
void lvn_vec2_##t##_subsv(LvnType_##t s, LvnVec2_##t v, LvnVec2_##t dst) {                                             \
    dst[0] = s - v[0];                                                                                                 \
    dst[1] = s - v[1];                                                                                                 \
}                                                                                                                      \
void lvn_vec2_##t##_multsv(LvnType_##t s, LvnVec2_##t v, LvnVec2_##t dst) {                                            \
    dst[0] = s * v[0];                                                                                                 \
    dst[1] = s * v[1];                                                                                                 \
}                                                                                                                      \
void lvn_vec2_##t##_divsv(LvnType_##t s, LvnVec2_##t v, LvnVec2_##t dst) {                                             \
    dst[0] = s / v[0];                                                                                                 \
    dst[1] = s / v[1];                                                                                                 \
}                                                                                                                      \
void lvn_vec2_##t##_addpv(LvnVec2_##t* pv[], uint32_t count, LvnVec2_##t dst) {                                        \
    if (!count) { return; }                                                                                            \
    dst[0] = (*(pv[0]))[0];                                                                                            \
    dst[1] = (*(pv[0]))[1];                                                                                            \
    for (uint32_t i = 1; i < count; i++) {                                                                             \
        dst[0] += (*(pv[i]))[0];                                                                                       \
        dst[1] += (*(pv[i]))[1];                                                                                       \
    }                                                                                                                  \
}                                                                                                                      \
void lvn_vec2_##t##_subpv(LvnVec2_##t* pv[], uint32_t count, LvnVec2_##t dst) {                                        \
    if (!count) { return; }                                                                                            \
    dst[0] = (*(pv[0]))[0];                                                                                            \
    dst[1] = (*(pv[0]))[1];                                                                                            \
    for (uint32_t i = 1; i < count; i++) {                                                                             \
        dst[0] -= (*(pv[i]))[0];                                                                                       \
        dst[1] -= (*(pv[i]))[1];                                                                                       \
    }                                                                                                                  \
}                                                                                                                      \
void lvn_vec2_##t##_multpv(LvnVec2_##t* pv[], uint32_t count, LvnVec2_##t dst) {                                       \
    if (!count) { return; }                                                                                            \
    dst[0] = (*(pv[0]))[0];                                                                                            \
    dst[1] = (*(pv[0]))[1];                                                                                            \
    for (uint32_t i = 1; i < count; i++) {                                                                             \
        dst[0] *= (*(pv[i]))[0];                                                                                       \
        dst[1] *= (*(pv[i]))[1];                                                                                       \
    }                                                                                                                  \
}                                                                                                                      \
void lvn_vec2_##t##_divpv(LvnVec2_##t* pv[], uint32_t count, LvnVec2_##t dst) {                                        \
    if (!count) { return; }                                                                                            \
    dst[0] = (*(pv[0]))[0];                                                                                            \
    dst[1] = (*(pv[0]))[1];                                                                                            \
    for (uint32_t i = 1; i < count; i++) {                                                                             \
        dst[0] /= (*(pv[i]))[0];                                                                                       \
        dst[1] /= (*(pv[i]))[1];                                                                                       \
    }                                                                                                                  \
}                                                                                                                      \
LvnType_##t lvn_vec2_##t##_mag(LvnVec2_##t v) {                                                                        \
    return (LvnType_##t) sqrt((LvnType_##t)(v[0] * v[0] + v[1] * v[1]));                                               \
}                                                                                                                      \
LvnType_##t lvn_vec2_##t##_mag2(LvnVec2_##t v) {                                                                       \
    return v[0] * v[0] + v[1] * v[1];                                                                                  \
}                                                                                                                      \
LvnType_##t lvn_vec2_##t##_dot(LvnVec2_##t v1, LvnVec2_##t v2) {                                                       \
    return v1[0] * v2[0] + v1[1] * v2[1];                                                                              \
}                                                                                                                      \

#define LVN_DEFINE_VEC3_TYPE_MATH_IMPL(t)                                                                              \
void lvn_vec3_##t(LvnType_##t* v, LvnVec3_##t dst) {                                                                   \
    dst[0] = v[0];                                                                                                     \
    dst[1] = v[1];                                                                                                     \
    dst[2] = v[2];                                                                                                     \
}                                                                                                                      \
void lvn_vec3_##t##_s(LvnType_##t s, LvnVec3_##t dst) {                                                                \
    dst[0] = s;                                                                                                        \
    dst[1] = s;                                                                                                        \
    dst[2] = s;                                                                                                        \
}                                                                                                                      \
void lvn_vec3_##t##_zero(LvnVec3_##t dst) {                                                                            \
    dst[0] = (LvnType_##t)0;                                                                                           \
    dst[1] = (LvnType_##t)0;                                                                                           \
    dst[2] = (LvnType_##t)0;                                                                                           \
}                                                                                                                      \
void lvn_vec3_##t##_one(LvnVec3_##t dst) {                                                                             \
    dst[0] = (LvnType_##t)1;                                                                                           \
    dst[1] = (LvnType_##t)1;                                                                                           \
    dst[2] = (LvnType_##t)1;                                                                                           \
}                                                                                                                      \
void lvn_vec3_##t##_copy(LvnVec3_##t src, LvnVec3_##t dst) {                                                           \
    dst[0] = src[0];                                                                                                   \
    dst[1] = src[1];                                                                                                   \
    dst[2] = src[2];                                                                                                   \
}                                                                                                                      \
void lvn_vec3_##t##_negate(LvnVec3_##t v) {                                                                            \
    v[0] = -v[0];                                                                                                      \
    v[1] = -v[1];                                                                                                      \
    v[2] = -v[2];                                                                                                      \
}                                                                                                                      \
void lvn_vec3_##t##_negate_to(LvnVec3_##t v, LvnVec3_##t dst) {                                                        \
    dst[0] = -v[0];                                                                                                    \
    dst[1] = -v[1];                                                                                                    \
    dst[2] = -v[2];                                                                                                    \
}                                                                                                                      \
void lvn_vec3_##t##_add(LvnVec3_##t v1, LvnVec3_##t v2, LvnVec3_##t dst) {                                             \
    dst[0] = v1[0] + v2[0];                                                                                            \
    dst[1] = v1[1] + v2[1];                                                                                            \
    dst[2] = v1[2] + v2[2];                                                                                            \
}                                                                                                                      \
void lvn_vec3_##t##_sub(LvnVec3_##t v1, LvnVec3_##t v2, LvnVec3_##t dst) {                                             \
    dst[0] = v1[0] - v2[0];                                                                                            \
    dst[1] = v1[1] - v2[1];                                                                                            \
    dst[2] = v1[2] - v2[2];                                                                                            \
}                                                                                                                      \
void lvn_vec3_##t##_mult(LvnVec3_##t v1, LvnVec3_##t v2, LvnVec3_##t dst) {                                            \
    dst[0] = v1[0] * v2[0];                                                                                            \
    dst[1] = v1[1] * v2[1];                                                                                            \
    dst[2] = v1[2] * v2[2];                                                                                            \
}                                                                                                                      \
void lvn_vec3_##t##_div(LvnVec3_##t v1, LvnVec3_##t v2, LvnVec3_##t dst) {                                             \
    dst[0] = v1[0] / v2[0];                                                                                            \
    dst[1] = v1[1] / v2[1];                                                                                            \
    dst[2] = v1[2] / v2[2];                                                                                            \
}                                                                                                                      \
void lvn_vec3_##t##_addvs(LvnVec3_##t v, LvnType_##t s, LvnVec3_##t dst) {                                             \
    dst[0] = v[0] + s;                                                                                                 \
    dst[1] = v[1] + s;                                                                                                 \
    dst[2] = v[2] + s;                                                                                                 \
}                                                                                                                      \
void lvn_vec3_##t##_subvs(LvnVec3_##t v, LvnType_##t s, LvnVec3_##t dst) {                                             \
    dst[0] = v[0] - s;                                                                                                 \
    dst[1] = v[1] - s;                                                                                                 \
    dst[2] = v[2] - s;                                                                                                 \
}                                                                                                                      \
void lvn_vec3_##t##_multvs(LvnVec3_##t v, LvnType_##t s, LvnVec3_##t dst) {                                            \
    dst[0] = v[0] * s;                                                                                                 \
    dst[1] = v[1] * s;                                                                                                 \
    dst[2] = v[2] * s;                                                                                                 \
}                                                                                                                      \
void lvn_vec3_##t##_divvs(LvnVec3_##t v, LvnType_##t s, LvnVec3_##t dst) {                                             \
    dst[0] = v[0] / s;                                                                                                 \
    dst[1] = v[1] / s;                                                                                                 \
    dst[2] = v[2] / s;                                                                                                 \
}                                                                                                                      \
void lvn_vec3_##t##_addsv(LvnType_##t s, LvnVec3_##t v, LvnVec3_##t dst) {                                             \
    dst[0] = s + v[0];                                                                                                 \
    dst[1] = s + v[1];                                                                                                 \
    dst[2] = s + v[2];                                                                                                 \
}                                                                                                                      \
void lvn_vec3_##t##_subsv(LvnType_##t s, LvnVec3_##t v, LvnVec3_##t dst) {                                             \
    dst[0] = s - v[0];                                                                                                 \
    dst[1] = s - v[1];                                                                                                 \
    dst[2] = s - v[2];                                                                                                 \
}                                                                                                                      \
void lvn_vec3_##t##_multsv(LvnType_##t s, LvnVec3_##t v, LvnVec3_##t dst) {                                            \
    dst[0] = s * v[0];                                                                                                 \
    dst[1] = s * v[1];                                                                                                 \
    dst[2] = s * v[2];                                                                                                 \
}                                                                                                                      \
void lvn_vec3_##t##_divsv(LvnType_##t s, LvnVec3_##t v, LvnVec3_##t dst) {                                             \
    dst[0] = s / v[0];                                                                                                 \
    dst[1] = s / v[1];                                                                                                 \
    dst[2] = s / v[2];                                                                                                 \
}                                                                                                                      \
void lvn_vec3_##t##_addpv(LvnVec3_##t* pv[], uint32_t count, LvnVec3_##t dst) {                                        \
    if (!count) { return; }                                                                                            \
    dst[0] = (*(pv[0]))[0];                                                                                            \
    dst[1] = (*(pv[0]))[1];                                                                                            \
    dst[2] = (*(pv[0]))[2];                                                                                            \
    for (uint32_t i = 1; i < count; i++) {                                                                             \
        dst[0] += (*(pv[i]))[0];                                                                                       \
        dst[1] += (*(pv[i]))[1];                                                                                       \
        dst[2] += (*(pv[i]))[2];                                                                                       \
    }                                                                                                                  \
}                                                                                                                      \
void lvn_vec3_##t##_subpv(LvnVec3_##t* pv[], uint32_t count, LvnVec3_##t dst) {                                        \
    if (!count) { return; }                                                                                            \
    dst[0] = (*(pv[0]))[0];                                                                                            \
    dst[1] = (*(pv[0]))[1];                                                                                            \
    dst[2] = (*(pv[0]))[2];                                                                                            \
    for (uint32_t i = 1; i < count; i++) {                                                                             \
        dst[0] -= (*(pv[i]))[0];                                                                                       \
        dst[1] -= (*(pv[i]))[1];                                                                                       \
        dst[2] -= (*(pv[i]))[2];                                                                                       \
    }                                                                                                                  \
}                                                                                                                      \
void lvn_vec3_##t##_multpv(LvnVec3_##t* pv[], uint32_t count, LvnVec3_##t dst) {                                       \
    if (!count) { return; }                                                                                            \
    dst[0] = (*(pv[0]))[0];                                                                                            \
    dst[1] = (*(pv[0]))[1];                                                                                            \
    dst[2] = (*(pv[0]))[2];                                                                                            \
    for (uint32_t i = 1; i < count; i++) {                                                                             \
        dst[0] *= (*(pv[i]))[0];                                                                                       \
        dst[1] *= (*(pv[i]))[1];                                                                                       \
        dst[2] *= (*(pv[i]))[2];                                                                                       \
    }                                                                                                                  \
}                                                                                                                      \
void lvn_vec3_##t##_divpv(LvnVec3_##t* pv[], uint32_t count, LvnVec3_##t dst) {                                        \
    if (!count) { return; }                                                                                            \
    dst[0] = (*(pv[0]))[0];                                                                                            \
    dst[1] = (*(pv[0]))[1];                                                                                            \
    dst[2] = (*(pv[0]))[2];                                                                                            \
    for (uint32_t i = 1; i < count; i++) {                                                                             \
        dst[0] /= (*(pv[i]))[0];                                                                                       \
        dst[1] /= (*(pv[i]))[1];                                                                                       \
        dst[2] /= (*(pv[i]))[2];                                                                                       \
    }                                                                                                                  \
}                                                                                                                      \
void lvn_vec3_##t##_cross(LvnVec3_##t v1, LvnVec3_##t v2, LvnVec3_##t dst) {                                           \
    dst[0] = v1[1] * v2[2] - v1[2] * v2[1];                                                                            \
    dst[1] = v1[2] * v2[0] - v1[0] * v2[2];                                                                            \
    dst[2] = v1[0] * v2[1] - v1[1] * v2[0];                                                                            \
}                                                                                                                      \
LvnType_##t lvn_vec3_##t##_mag(LvnVec3_##t v) {                                                                        \
    return (LvnType_##t) sqrt((LvnType_##t)(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]));                                 \
}                                                                                                                      \
LvnType_##t lvn_vec3_##t##_mag2(LvnVec3_##t v) {                                                                       \
    return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];                                                                    \
}                                                                                                                      \
LvnType_##t lvn_vec3_##t##_dot(LvnVec3_##t v1, LvnVec3_##t v2) {                                                       \
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];                                                              \
}

#define LVN_DEFINE_VEC4_TYPE_MATH_IMPL(t)                                                                              \
void lvn_vec4_##t(LvnType_##t* v, LvnVec4_##t dst) {                                                                   \
    dst[0] = v[0];                                                                                                     \
    dst[1] = v[1];                                                                                                     \
    dst[2] = v[2];                                                                                                     \
    dst[3] = v[3];                                                                                                     \
}                                                                                                                      \
void lvn_vec4_##t##_s(LvnType_##t s, LvnVec4_##t dst) {                                                                \
    dst[0] = s;                                                                                                        \
    dst[1] = s;                                                                                                        \
    dst[2] = s;                                                                                                        \
    dst[3] = s;                                                                                                        \
}                                                                                                                      \
void lvn_vec4_##t##_zero(LvnVec4_##t dst) {                                                                            \
    dst[0] = (LvnType_##t)0;                                                                                           \
    dst[1] = (LvnType_##t)0;                                                                                           \
    dst[2] = (LvnType_##t)0;                                                                                           \
    dst[3] = (LvnType_##t)0;                                                                                           \
}                                                                                                                      \
void lvn_vec4_##t##_one(LvnVec4_##t dst) {                                                                             \
    dst[0] = (LvnType_##t)1;                                                                                           \
    dst[1] = (LvnType_##t)1;                                                                                           \
    dst[2] = (LvnType_##t)1;                                                                                           \
    dst[3] = (LvnType_##t)1;                                                                                           \
}                                                                                                                      \
void lvn_vec4_##t##_copy(LvnVec4_##t src, LvnVec4_##t dst) {                                                           \
    dst[0] = src[0];                                                                                                   \
    dst[1] = src[1];                                                                                                   \
    dst[2] = src[2];                                                                                                   \
    dst[3] = src[3];                                                                                                   \
}                                                                                                                      \
void lvn_vec4_##t##_negate(LvnVec4_##t v) {                                                                            \
    v[0] = -v[0];                                                                                                      \
    v[1] = -v[1];                                                                                                      \
    v[2] = -v[2];                                                                                                      \
    v[3] = -v[3];                                                                                                      \
}                                                                                                                      \
void lvn_vec4_##t##_negate_to(LvnVec4_##t v, LvnVec4_##t dst) {                                                        \
    dst[0] = -v[0];                                                                                                    \
    dst[1] = -v[1];                                                                                                    \
    dst[2] = -v[2];                                                                                                    \
    dst[3] = -v[3];                                                                                                    \
}                                                                                                                      \
void lvn_vec4_##t##_add(LvnVec4_##t v1, LvnVec4_##t v2, LvnVec4_##t dst) {                                             \
    dst[0] = v1[0] + v2[0];                                                                                            \
    dst[1] = v1[1] + v2[1];                                                                                            \
    dst[2] = v1[2] + v2[2];                                                                                            \
    dst[3] = v1[3] + v2[3];                                                                                            \
}                                                                                                                      \
void lvn_vec4_##t##_sub(LvnVec4_##t v1, LvnVec4_##t v2, LvnVec4_##t dst) {                                             \
    dst[0] = v1[0] - v2[0];                                                                                            \
    dst[1] = v1[1] - v2[1];                                                                                            \
    dst[2] = v1[2] - v2[2];                                                                                            \
    dst[3] = v1[3] - v2[3];                                                                                            \
}                                                                                                                      \
void lvn_vec4_##t##_mult(LvnVec4_##t v1, LvnVec4_##t v2, LvnVec4_##t dst) {                                            \
    dst[0] = v1[0] * v2[0];                                                                                            \
    dst[1] = v1[1] * v2[1];                                                                                            \
    dst[2] = v1[2] * v2[2];                                                                                            \
    dst[3] = v1[3] * v2[3];                                                                                            \
}                                                                                                                      \
void lvn_vec4_##t##_div(LvnVec4_##t v1, LvnVec4_##t v2, LvnVec4_##t dst) {                                             \
    dst[0] = v1[0] / v2[0];                                                                                            \
    dst[1] = v1[1] / v2[1];                                                                                            \
    dst[2] = v1[2] / v2[2];                                                                                            \
    dst[3] = v1[3] / v2[3];                                                                                            \
}                                                                                                                      \
void lvn_vec4_##t##_addvs(LvnVec4_##t v, LvnType_##t s, LvnVec4_##t dst) {                                             \
    dst[0] = v[0] + s;                                                                                                 \
    dst[1] = v[1] + s;                                                                                                 \
    dst[2] = v[2] + s;                                                                                                 \
    dst[3] = v[3] + s;                                                                                                 \
}                                                                                                                      \
void lvn_vec4_##t##_subvs(LvnVec4_##t v, LvnType_##t s, LvnVec4_##t dst) {                                             \
    dst[0] = v[0] - s;                                                                                                 \
    dst[1] = v[1] - s;                                                                                                 \
    dst[2] = v[2] - s;                                                                                                 \
    dst[3] = v[3] - s;                                                                                                 \
}                                                                                                                      \
void lvn_vec4_##t##_multvs(LvnVec4_##t v, LvnType_##t s, LvnVec4_##t dst) {                                            \
    dst[0] = v[0] * s;                                                                                                 \
    dst[1] = v[1] * s;                                                                                                 \
    dst[2] = v[2] * s;                                                                                                 \
    dst[3] = v[3] * s;                                                                                                 \
}                                                                                                                      \
void lvn_vec4_##t##_divvs(LvnVec4_##t v, LvnType_##t s, LvnVec4_##t dst) {                                             \
    dst[0] = v[0] / s;                                                                                                 \
    dst[1] = v[1] / s;                                                                                                 \
    dst[2] = v[2] / s;                                                                                                 \
    dst[3] = v[3] / s;                                                                                                 \
}                                                                                                                      \
void lvn_vec4_##t##_addsv(LvnType_##t s, LvnVec4_##t v, LvnVec4_##t dst) {                                             \
    dst[0] = s + v[0];                                                                                                 \
    dst[1] = s + v[1];                                                                                                 \
    dst[2] = s + v[2];                                                                                                 \
    dst[3] = s + v[3];                                                                                                 \
}                                                                                                                      \
void lvn_vec4_##t##_subsv(LvnType_##t s, LvnVec4_##t v, LvnVec4_##t dst) {                                             \
    dst[0] = s - v[0];                                                                                                 \
    dst[1] = s - v[1];                                                                                                 \
    dst[2] = s - v[2];                                                                                                 \
    dst[3] = s - v[3];                                                                                                 \
}                                                                                                                      \
void lvn_vec4_##t##_multsv(LvnType_##t s, LvnVec4_##t v, LvnVec4_##t dst) {                                            \
    dst[0] = s * v[0];                                                                                                 \
    dst[1] = s * v[1];                                                                                                 \
    dst[2] = s * v[2];                                                                                                 \
    dst[3] = s * v[3];                                                                                                 \
}                                                                                                                      \
void lvn_vec4_##t##_divsv(LvnType_##t s, LvnVec4_##t v, LvnVec4_##t dst) {                                             \
    dst[0] = s / v[0];                                                                                                 \
    dst[1] = s / v[1];                                                                                                 \
    dst[2] = s / v[2];                                                                                                 \
    dst[3] = s / v[3];                                                                                                 \
}                                                                                                                      \
void lvn_vec4_##t##_addpv(LvnVec4_##t* pv[], uint32_t count, LvnVec4_##t dst) {                                        \
    if (!count) { return; }                                                                                            \
    dst[0] = (*(pv[0]))[0];                                                                                            \
    dst[1] = (*(pv[0]))[1];                                                                                            \
    dst[2] = (*(pv[0]))[2];                                                                                            \
    dst[3] = (*(pv[0]))[3];                                                                                            \
    for (uint32_t i = 1; i < count; i++) {                                                                             \
        dst[0] += (*(pv[i]))[0];                                                                                       \
        dst[1] += (*(pv[i]))[1];                                                                                       \
        dst[2] += (*(pv[i]))[2];                                                                                       \
        dst[3] += (*(pv[i]))[3];                                                                                       \
    }                                                                                                                  \
}                                                                                                                      \
void lvn_vec4_##t##_subpv(LvnVec4_##t* pv[], uint32_t count, LvnVec4_##t dst) {                                        \
    if (!count) { return; }                                                                                            \
    dst[0] = (*(pv[0]))[0];                                                                                            \
    dst[1] = (*(pv[0]))[1];                                                                                            \
    dst[2] = (*(pv[0]))[2];                                                                                            \
    dst[3] = (*(pv[0]))[3];                                                                                            \
    for (uint32_t i = 1; i < count; i++) {                                                                             \
        dst[0] -= (*(pv[i]))[0];                                                                                       \
        dst[1] -= (*(pv[i]))[1];                                                                                       \
        dst[2] -= (*(pv[i]))[2];                                                                                       \
        dst[3] -= (*(pv[i]))[3];                                                                                       \
    }                                                                                                                  \
}                                                                                                                      \
void lvn_vec4_##t##_multpv(LvnVec4_##t* pv[], uint32_t count, LvnVec4_##t dst) {                                       \
    if (!count) { return; }                                                                                            \
    dst[0] = (*(pv[0]))[0];                                                                                            \
    dst[1] = (*(pv[0]))[1];                                                                                            \
    dst[2] = (*(pv[0]))[2];                                                                                            \
    dst[3] = (*(pv[0]))[3];                                                                                            \
    for (uint32_t i = 1; i < count; i++) {                                                                             \
        dst[0] *= (*(pv[i]))[0];                                                                                       \
        dst[1] *= (*(pv[i]))[1];                                                                                       \
        dst[2] *= (*(pv[i]))[2];                                                                                       \
        dst[3] *= (*(pv[i]))[3];                                                                                       \
    }                                                                                                                  \
}                                                                                                                      \
void lvn_vec4_##t##_divpv(LvnVec4_##t* pv[], uint32_t count, LvnVec4_##t dst) {                                        \
    if (!count) { return; }                                                                                            \
    dst[0] = (*(pv[0]))[0];                                                                                            \
    dst[1] = (*(pv[0]))[1];                                                                                            \
    dst[2] = (*(pv[0]))[2];                                                                                            \
    dst[3] = (*(pv[0]))[3];                                                                                            \
    for (uint32_t i = 1; i < count; i++) {                                                                             \
        dst[0] /= (*(pv[i]))[0];                                                                                       \
        dst[1] /= (*(pv[i]))[1];                                                                                       \
        dst[2] /= (*(pv[i]))[2];                                                                                       \
        dst[3] /= (*(pv[i]))[3];                                                                                       \
    }                                                                                                                  \
}                                                                                                                      \
LvnType_##t lvn_vec4_##t##_mag(LvnVec4_##t v) {                                                                        \
    return (LvnType_##t) sqrt((LvnType_##t)(v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3]));                   \
}                                                                                                                      \
LvnType_##t lvn_vec4_##t##_mag2(LvnVec4_##t v) {                                                                       \
    return v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];                                                      \
}                                                                                                                      \
LvnType_##t lvn_vec4_##t##_dot(LvnVec4_##t v1, LvnVec4_##t v2) {                                                       \
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2] + v1[3] * v2[3];                                              \
}

#define LVN_DEFINE_MAT2_TYPE_MATH_IMPL(t)                                                                              \
void lvn_mat2_##t(LvnType_##t* v, LvnMat2x2_##t dst) {                                                                 \
    dst[0][0] = v[0];                                                                                                  \
    dst[0][1] = v[1];                                                                                                  \
    dst[1][0] = v[2];                                                                                                  \
    dst[1][1] = v[3];                                                                                                  \
}                                                                                                                      \
void lvn_mat2_##t##_zero(LvnMat2x2_##t dst) {                                                                          \
    dst[0][0] = (LvnType_##t)0;                                                                                        \
    dst[0][1] = (LvnType_##t)0;                                                                                        \
    dst[1][0] = (LvnType_##t)0;                                                                                        \
    dst[1][1] = (LvnType_##t)0;                                                                                        \
}                                                                                                                      \
void lvn_mat2_##t##_identity(LvnMat2x2_##t dst) {                                                                      \
    dst[0][0] = (LvnType_##t)1;                                                                                        \
    dst[0][1] = (LvnType_##t)0;                                                                                        \
    dst[1][0] = (LvnType_##t)0;                                                                                        \
    dst[1][1] = (LvnType_##t)1;                                                                                        \
}                                                                                                                      \
void lvn_mat2_##t##_copy(LvnMat2x2_##t src, LvnMat2x2_##t dst) {                                                       \
    dst[0][0] = src[0][0];                                                                                             \
    dst[0][1] = src[0][1];                                                                                             \
    dst[1][0] = src[1][0];                                                                                             \
    dst[1][1] = src[1][1];                                                                                             \
}                                                                                                                      \
LvnType_##t lvn_mat2_##t##_det(LvnMat2x2_##t m) {                                                                      \
    return m[0][0] * m[1][1] - m[1][0] * m[0][1];                                                                      \
}                                                                                                                      \
void lvn_mat2_##t##_inv(LvnMat2x2_##t mat, LvnMat2x2_##t dst) {                                                        \
    float det;                                                                                                         \
    float a = mat[0][0], b = mat[0][1],                                                                                \
          c = mat[1][0], d = mat[1][1];                                                                                \
          det = 1.0f / (a * d - b * c);                                                                                \
    dst[0][0] =  d * det;                                                                                              \
    dst[0][1] = -b * det;                                                                                              \
    dst[1][0] = -c * det;                                                                                              \
    dst[1][1] =  a * det;                                                                                              \
}                                                                                                                      \
void lvn_mat2_##t##_transpose_to(LvnMat2x2_##t m, LvnMat2x2_##t dst) {                                                 \
    dst[0][0] = m[0][0];                                                                                               \
    dst[0][1] = m[1][0];                                                                                               \
    dst[1][0] = m[0][1];                                                                                               \
    dst[1][1] = m[1][1];                                                                                               \
}                                                                                                                      \
void lvn_mat2_##t##_transpose(LvnMat2x2_##t m) {                                                                       \
    LvnType_##t temp = m[0][1];                                                                                        \
    m[0][1] = m[1][0];                                                                                                 \
    m[1][0] = temp;                                                                                                    \
}                                                                                                                      \
void lvn_mat2_##t##_negate_to(LvnMat2x2_##t m, LvnMat2x2_##t dst) {                                                    \
    dst[0][0] = -m[0][0];                                                                                              \
    dst[0][1] = -m[0][1];                                                                                              \
    dst[1][0] = -m[1][0];                                                                                              \
    dst[1][1] = -m[1][1];                                                                                              \
}                                                                                                                      \
void lvn_mat2_##t##_negate(LvnMat2x2_##t m) {                                                                          \
    m[0][0] = -m[0][0];                                                                                                \
    m[0][1] = -m[0][1];                                                                                                \
    m[1][0] = -m[1][0];                                                                                                \
    m[1][1] = -m[1][1];                                                                                                \
}                                                                                                                      \
void lvn_mat2_##t##_add(LvnMat2x2_##t m1, LvnMat2x2_##t m2, LvnMat2x2_##t dst) {                                       \
    dst[0][0] = m1[0][0] + m2[0][0];                                                                                   \
    dst[0][1] = m1[0][1] + m2[0][1];                                                                                   \
    dst[1][0] = m1[1][0] + m2[1][0];                                                                                   \
    dst[1][1] = m1[1][1] + m2[1][1];                                                                                   \
}                                                                                                                      \
void lvn_mat2_##t##_sub(LvnMat2x2_##t m1, LvnMat2x2_##t m2, LvnMat2x2_##t dst) {                                       \
    dst[0][0] = m1[0][0] - m2[0][0];                                                                                   \
    dst[0][1] = m1[0][1] - m2[0][1];                                                                                   \
    dst[1][0] = m1[1][0] - m2[1][0];                                                                                   \
    dst[1][1] = m1[1][1] - m2[1][1];                                                                                   \
}                                                                                                                      \
void lvn_mat2_##t##_mult(LvnMat2x2_##t m1, LvnMat2x2_##t m2, LvnMat2x2_##t dst) {                                      \
    dst[0][0] = m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1];                                                             \
    dst[0][1] = m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1];                                                             \
    dst[1][0] = m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1];                                                             \
    dst[1][1] = m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1];                                                             \
}                                                                                                                      \
void lvn_mat2_##t##_multmv(LvnMat2x2_##t m, LvnVec2_##t v, LvnVec2_##t dst) {                                          \
    dst[0] = m[0][0] * v[0] + m[1][0] * v[1];                                                                          \
    dst[1] = m[0][1] * v[0] + m[1][1] * v[1];                                                                          \
}                                                                                                                      \
void lvn_mat2_##t##_multvm(LvnVec2_##t v, LvnMat2x2_##t m, LvnVec2_##t dst) {                                          \
    dst[0] = v[0] * m[0][0] + v[1] * m[0][1];                                                                          \
    dst[1] = v[0] * m[1][0] + v[1] * m[1][1];                                                                          \
}                                                                                                                      \
void lvn_mat2_##t##_addms(LvnMat2x2_##t m, LvnType_##t s, LvnMat2x2_##t dst) {                                         \
    dst[0][0] = m[0][0] + s;                                                                                           \
    dst[0][1] = m[0][1] + s;                                                                                           \
    dst[1][0] = m[1][0] + s;                                                                                           \
    dst[1][1] = m[1][1] + s;                                                                                           \
}                                                                                                                      \
void lvn_mat2_##t##_subms(LvnMat2x2_##t m, LvnType_##t s, LvnMat2x2_##t dst) {                                         \
    dst[0][0] = m[0][0] - s;                                                                                           \
    dst[0][1] = m[0][1] - s;                                                                                           \
    dst[1][0] = m[1][0] - s;                                                                                           \
    dst[1][1] = m[1][1] - s;                                                                                           \
}                                                                                                                      \
void lvn_mat2_##t##_multms(LvnMat2x2_##t m, LvnType_##t s, LvnMat2x2_##t dst) {                                        \
    dst[0][0] = m[0][0] * s;                                                                                           \
    dst[0][1] = m[0][1] * s;                                                                                           \
    dst[1][0] = m[1][0] * s;                                                                                           \
    dst[1][1] = m[1][1] * s;                                                                                           \
}                                                                                                                      \
void lvn_mat2_##t##_divms(LvnMat2x2_##t m, LvnType_##t s, LvnMat2x2_##t dst) {                                         \
    dst[0][0] = m[0][0] / s;                                                                                           \
    dst[0][1] = m[0][1] / s;                                                                                           \
    dst[1][0] = m[1][0] / s;                                                                                           \
    dst[1][1] = m[1][1] / s;                                                                                           \
}                                                                                                                      \
void lvn_mat2_##t##_addsm(LvnType_##t s, LvnMat2x2_##t m, LvnMat2x2_##t dst) {                                         \
    dst[0][0] = s + m[0][0];                                                                                           \
    dst[0][1] = s + m[0][1];                                                                                           \
    dst[1][0] = s + m[1][0];                                                                                           \
    dst[1][1] = s + m[1][1];                                                                                           \
}                                                                                                                      \
void lvn_mat2_##t##_subsm(LvnType_##t s, LvnMat2x2_##t m, LvnMat2x2_##t dst) {                                         \
    dst[0][0] = s - m[0][0];                                                                                           \
    dst[0][1] = s - m[0][1];                                                                                           \
    dst[1][0] = s - m[1][0];                                                                                           \
    dst[1][1] = s - m[1][1];                                                                                           \
}                                                                                                                      \
void lvn_mat2_##t##_multsm(LvnType_##t s, LvnMat2x2_##t m, LvnMat2x2_##t dst) {                                        \
    dst[0][0] = s * m[0][0];                                                                                           \
    dst[0][1] = s * m[0][1];                                                                                           \
    dst[1][0] = s * m[1][0];                                                                                           \
    dst[1][1] = s * m[1][1];                                                                                           \
}                                                                                                                      \
void lvn_mat2_##t##_divsm(LvnType_##t s, LvnMat2x2_##t m, LvnMat2x2_##t dst) {                                         \
    dst[0][0] = s / m[0][0];                                                                                           \
    dst[0][1] = s / m[0][1];                                                                                           \
    dst[1][0] = s / m[1][0];                                                                                           \
    dst[1][1] = s / m[1][1];                                                                                           \
}

#define LVN_DEFINE_MAT3_TYPE_MATH_IMPL(t)                                                                              \
void lvn_mat3_##t(LvnType_##t* v, LvnMat3x3_##t dst) {                                                                 \
    dst[0][0] = v[0];                                                                                                  \
    dst[0][1] = v[1];                                                                                                  \
    dst[0][2] = v[2];                                                                                                  \
    dst[1][0] = v[3];                                                                                                  \
    dst[1][1] = v[4];                                                                                                  \
    dst[1][2] = v[5];                                                                                                  \
    dst[2][0] = v[6];                                                                                                  \
    dst[2][1] = v[7];                                                                                                  \
    dst[2][2] = v[8];                                                                                                  \
}                                                                                                                      \
void lvn_mat3_##t##_zero(LvnMat3x3_##t dst) {                                                                          \
    dst[0][0] = (LvnType_##t)0;                                                                                        \
    dst[0][1] = (LvnType_##t)0;                                                                                        \
    dst[0][2] = (LvnType_##t)0;                                                                                        \
    dst[1][0] = (LvnType_##t)0;                                                                                        \
    dst[1][1] = (LvnType_##t)0;                                                                                        \
    dst[1][2] = (LvnType_##t)0;                                                                                        \
    dst[2][0] = (LvnType_##t)0;                                                                                        \
    dst[2][1] = (LvnType_##t)0;                                                                                        \
    dst[2][2] = (LvnType_##t)0;                                                                                        \
}                                                                                                                      \
void lvn_mat3_##t##_identity(LvnMat3x3_##t dst) {                                                                      \
    dst[0][0] = (LvnType_##t)1;                                                                                        \
    dst[0][1] = (LvnType_##t)0;                                                                                        \
    dst[0][2] = (LvnType_##t)0;                                                                                        \
    dst[1][0] = (LvnType_##t)0;                                                                                        \
    dst[1][1] = (LvnType_##t)1;                                                                                        \
    dst[1][2] = (LvnType_##t)0;                                                                                        \
    dst[2][0] = (LvnType_##t)0;                                                                                        \
    dst[2][1] = (LvnType_##t)0;                                                                                        \
    dst[2][2] = (LvnType_##t)1;                                                                                        \
}                                                                                                                      \
void lvn_mat3_##t##_copy(LvnMat3x3_##t src, LvnMat3x3_##t dst) {                                                       \
    dst[0][0] = src[0][0];                                                                                             \
    dst[0][1] = src[0][1];                                                                                             \
    dst[0][2] = src[0][2];                                                                                             \
    dst[1][0] = src[1][0];                                                                                             \
    dst[1][1] = src[1][1];                                                                                             \
    dst[1][2] = src[1][2];                                                                                             \
    dst[2][0] = src[2][0];                                                                                             \
    dst[2][1] = src[2][1];                                                                                             \
    dst[2][2] = src[2][2];                                                                                             \
}                                                                                                                      \
LvnType_##t lvn_mat3_##t##_det(LvnMat3x3_##t m) {                                                                      \
    return + m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2])                                                         \
           - m[1][0] * (m[0][1] * m[2][2] - m[2][1] * m[0][2])                                                         \
           + m[2][0] * (m[0][1] * m[1][2] - m[1][1] * m[0][2]);                                                        \
}                                                                                                                      \
void lvn_mat3_##t##_inv(LvnMat3x3_##t mat, LvnMat3x3_##t dst) {                                                        \
    float a = mat[0][0], b = mat[0][1], c = mat[0][2],                                                                 \
          d = mat[1][0], e = mat[1][1], f = mat[1][2],                                                                 \
          g = mat[2][0], h = mat[2][1], i = mat[2][2],                                                                 \
          c1  = e * i - f * h, c2 = d * i - g * f, c3 = d * h - g * e,                                                 \
          idt = 1.0f / (a * c1 - b * c2 + c * c3), ndt = -idt;                                                         \
    dst[0][0] = idt * c1;                                                                                              \
    dst[0][1] = ndt * (b * i - h * c);                                                                                 \
    dst[0][2] = idt * (b * f - e * c);                                                                                 \
    dst[1][0] = ndt * c2;                                                                                              \
    dst[1][1] = idt * (a * i - g * c);                                                                                 \
    dst[1][2] = ndt * (a * f - d * c);                                                                                 \
    dst[2][0] = idt * c3;                                                                                              \
    dst[2][1] = ndt * (a * h - g * b);                                                                                 \
    dst[2][2] = idt * (a * e - d * b);                                                                                 \
}                                                                                                                      \
void lvn_mat3_##t##_transpose_to(LvnMat3x3_##t m, LvnMat3x3_##t dst) {                                                 \
    dst[0][0] = m[0][0];                                                                                               \
    dst[0][1] = m[1][0];                                                                                               \
    dst[0][2] = m[2][0];                                                                                               \
    dst[1][0] = m[0][1];                                                                                               \
    dst[1][1] = m[1][1];                                                                                               \
    dst[1][2] = m[2][1];                                                                                               \
    dst[2][0] = m[0][2];                                                                                               \
    dst[2][1] = m[1][2];                                                                                               \
    dst[2][2] = m[2][2];                                                                                               \
}                                                                                                                      \
void lvn_mat3_##t##_transpose(LvnMat3x3_##t m) {                                                                       \
    LvnMat3x3_##t temp;                                                                                                \
    temp[0][0] = m[0][0];                                                                                              \
    temp[0][1] = m[1][0];                                                                                              \
    temp[0][2] = m[2][0];                                                                                              \
    temp[1][0] = m[0][1];                                                                                              \
    temp[1][1] = m[1][1];                                                                                              \
    temp[1][2] = m[2][1];                                                                                              \
    temp[2][0] = m[0][2];                                                                                              \
    temp[2][1] = m[1][2];                                                                                              \
    temp[2][2] = m[2][2];                                                                                              \
    m[0][0] = temp[0][0];                                                                                              \
    m[0][1] = temp[0][1];                                                                                              \
    m[0][2] = temp[0][2];                                                                                              \
    m[1][0] = temp[1][0];                                                                                              \
    m[1][1] = temp[1][1];                                                                                              \
    m[1][2] = temp[1][2];                                                                                              \
    m[2][0] = temp[2][0];                                                                                              \
    m[2][1] = temp[2][1];                                                                                              \
    m[2][2] = temp[2][2];                                                                                              \
}                                                                                                                      \
void lvn_mat3_##t##_negate_to(LvnMat3x3_##t m, LvnMat3x3_##t dst) {                                                    \
    dst[0][0] = -m[0][0];                                                                                              \
    dst[0][1] = -m[0][1];                                                                                              \
    dst[0][2] = -m[0][2];                                                                                              \
    dst[1][0] = -m[1][0];                                                                                              \
    dst[1][1] = -m[1][1];                                                                                              \
    dst[1][2] = -m[1][2];                                                                                              \
    dst[2][0] = -m[2][0];                                                                                              \
    dst[2][1] = -m[2][1];                                                                                              \
    dst[2][2] = -m[2][2];                                                                                              \
}                                                                                                                      \
void lvn_mat3_##t##_negate(LvnMat3x3_##t m) {                                                                          \
    m[0][0] = -m[0][0];                                                                                                \
    m[0][1] = -m[0][1];                                                                                                \
    m[0][2] = -m[0][2];                                                                                                \
    m[1][0] = -m[1][0];                                                                                                \
    m[1][1] = -m[1][1];                                                                                                \
    m[1][2] = -m[1][2];                                                                                                \
    m[2][0] = -m[2][0];                                                                                                \
    m[2][1] = -m[2][1];                                                                                                \
    m[2][2] = -m[2][2];                                                                                                \
}                                                                                                                      \
void lvn_mat3_##t##_add(LvnMat3x3_##t m1, LvnMat3x3_##t m2, LvnMat3x3_##t dst) {                                       \
    dst[0][0] = m1[0][0] + m2[0][0];                                                                                   \
    dst[0][1] = m1[0][1] + m2[0][1];                                                                                   \
    dst[0][2] = m1[0][2] + m2[0][2];                                                                                   \
    dst[1][0] = m1[1][0] + m2[1][0];                                                                                   \
    dst[1][1] = m1[1][1] + m2[1][1];                                                                                   \
    dst[1][2] = m1[1][2] + m2[1][2];                                                                                   \
    dst[2][0] = m1[2][0] + m2[2][0];                                                                                   \
    dst[2][1] = m1[2][1] + m2[2][1];                                                                                   \
    dst[2][2] = m1[2][2] + m2[2][2];                                                                                   \
}                                                                                                                      \
void lvn_mat3_##t##_sub(LvnMat3x3_##t m1, LvnMat3x3_##t m2, LvnMat3x3_##t dst) {                                       \
    dst[0][0] = m1[0][0] - m2[0][0];                                                                                   \
    dst[0][1] = m1[0][1] - m2[0][1];                                                                                   \
    dst[0][2] = m1[0][2] - m2[0][2];                                                                                   \
    dst[1][0] = m1[1][0] - m2[1][0];                                                                                   \
    dst[1][1] = m1[1][1] - m2[1][1];                                                                                   \
    dst[1][2] = m1[1][2] - m2[1][2];                                                                                   \
    dst[2][0] = m1[2][0] - m2[2][0];                                                                                   \
    dst[2][1] = m1[2][1] - m2[2][1];                                                                                   \
    dst[2][2] = m1[2][2] - m2[2][2];                                                                                   \
}                                                                                                                      \
void lvn_mat3_##t##_mult(LvnMat3x3_##t m1, LvnMat3x3_##t m2, LvnMat3x3_##t dst) {                                      \
    dst[0][0] = m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2];                                       \
    dst[0][1] = m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2];                                       \
    dst[0][2] = m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1] + m1[2][2] * m2[0][2];                                       \
    dst[1][0] = m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2];                                       \
    dst[1][1] = m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2];                                       \
    dst[1][2] = m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1] + m1[2][2] * m2[1][2];                                       \
    dst[2][0] = m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1] + m1[2][0] * m2[2][2];                                       \
    dst[2][1] = m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1] + m1[2][1] * m2[2][2];                                       \
    dst[2][2] = m1[0][2] * m2[2][0] + m1[1][2] * m2[2][1] + m1[2][2] * m2[2][2];                                       \
}                                                                                                                      \
void lvn_mat3_##t##_multmv(LvnMat3x3_##t m, LvnVec3_##t v, LvnVec3_##t dst) {                                          \
    dst[0] = m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2];                                                         \
    dst[1] = m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2];                                                         \
    dst[2] = m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2];                                                         \
}                                                                                                                      \
void lvn_mat3_##t##_multvm(LvnVec3_##t v, LvnMat3x3_##t m, LvnVec3_##t dst) {                                          \
    dst[0] = v[0] * m[0][0] + v[1] * m[0][1] + v[2] * m[0][2];                                                         \
    dst[1] = v[0] * m[1][0] + v[1] * m[1][1] + v[2] * m[1][2];                                                         \
    dst[2] = v[0] * m[2][0] + v[1] * m[2][1] + v[2] * m[2][2];                                                         \
}                                                                                                                      \
void lvn_mat3_##t##_addms(LvnMat3x3_##t m, LvnType_##t s, LvnMat3x3_##t dst) {                                         \
    dst[0][0] = m[0][0] + s;                                                                                           \
    dst[0][1] = m[0][1] + s;                                                                                           \
    dst[0][2] = m[0][2] + s;                                                                                           \
    dst[1][0] = m[1][0] + s;                                                                                           \
    dst[1][1] = m[1][1] + s;                                                                                           \
    dst[1][2] = m[1][2] + s;                                                                                           \
    dst[2][0] = m[2][0] + s;                                                                                           \
    dst[2][1] = m[2][1] + s;                                                                                           \
    dst[2][2] = m[2][2] + s;                                                                                           \
}                                                                                                                      \
void lvn_mat3_##t##_subms(LvnMat3x3_##t m, LvnType_##t s, LvnMat3x3_##t dst) {                                         \
    dst[0][0] = m[0][0] - s;                                                                                           \
    dst[0][1] = m[0][1] - s;                                                                                           \
    dst[0][2] = m[0][2] - s;                                                                                           \
    dst[1][0] = m[1][0] - s;                                                                                           \
    dst[1][1] = m[1][1] - s;                                                                                           \
    dst[1][2] = m[1][2] - s;                                                                                           \
    dst[2][0] = m[2][0] - s;                                                                                           \
    dst[2][1] = m[2][1] - s;                                                                                           \
    dst[2][2] = m[2][2] - s;                                                                                           \
}                                                                                                                      \
void lvn_mat3_##t##_multms(LvnMat3x3_##t m, LvnType_##t s, LvnMat3x3_##t dst) {                                        \
    dst[0][0] = m[0][0] * s;                                                                                           \
    dst[0][1] = m[0][1] * s;                                                                                           \
    dst[0][2] = m[0][2] * s;                                                                                           \
    dst[1][0] = m[1][0] * s;                                                                                           \
    dst[1][1] = m[1][1] * s;                                                                                           \
    dst[1][2] = m[1][2] * s;                                                                                           \
    dst[2][0] = m[2][0] * s;                                                                                           \
    dst[2][1] = m[2][1] * s;                                                                                           \
    dst[2][2] = m[2][2] * s;                                                                                           \
}                                                                                                                      \
void lvn_mat3_##t##_divms(LvnMat3x3_##t m, LvnType_##t s, LvnMat3x3_##t dst) {                                         \
    dst[0][0] = m[0][0] / s;                                                                                           \
    dst[0][1] = m[0][1] / s;                                                                                           \
    dst[0][2] = m[0][2] / s;                                                                                           \
    dst[1][0] = m[1][0] / s;                                                                                           \
    dst[1][1] = m[1][1] / s;                                                                                           \
    dst[1][2] = m[1][2] / s;                                                                                           \
    dst[2][0] = m[2][0] / s;                                                                                           \
    dst[2][1] = m[2][1] / s;                                                                                           \
    dst[2][2] = m[2][2] / s;                                                                                           \
}                                                                                                                      \
void lvn_mat3_##t##_addsm(LvnType_##t s, LvnMat3x3_##t m, LvnMat3x3_##t dst) {                                         \
    dst[0][0] = s + m[0][0];                                                                                           \
    dst[0][1] = s + m[0][1];                                                                                           \
    dst[0][2] = s + m[0][2];                                                                                           \
    dst[1][0] = s + m[1][0];                                                                                           \
    dst[1][1] = s + m[1][1];                                                                                           \
    dst[1][2] = s + m[1][2];                                                                                           \
    dst[2][0] = s + m[2][0];                                                                                           \
    dst[2][1] = s + m[2][1];                                                                                           \
    dst[2][2] = s + m[2][2];                                                                                           \
}                                                                                                                      \
void lvn_mat3_##t##_subsm(LvnType_##t s, LvnMat3x3_##t m, LvnMat3x3_##t dst) {                                         \
    dst[0][0] = s - m[0][0];                                                                                           \
    dst[0][1] = s - m[0][1];                                                                                           \
    dst[0][2] = s - m[0][2];                                                                                           \
    dst[1][0] = s - m[1][0];                                                                                           \
    dst[1][1] = s - m[1][1];                                                                                           \
    dst[1][2] = s - m[1][2];                                                                                           \
    dst[2][0] = s - m[2][0];                                                                                           \
    dst[2][1] = s - m[2][1];                                                                                           \
    dst[2][2] = s - m[2][2];                                                                                           \
}                                                                                                                      \
void lvn_mat3_##t##_multsm(LvnType_##t s, LvnMat3x3_##t m, LvnMat3x3_##t dst) {                                        \
    dst[0][0] = s * m[0][0];                                                                                           \
    dst[0][1] = s * m[0][1];                                                                                           \
    dst[0][2] = s * m[0][2];                                                                                           \
    dst[1][0] = s * m[1][0];                                                                                           \
    dst[1][1] = s * m[1][1];                                                                                           \
    dst[1][2] = s * m[1][2];                                                                                           \
    dst[2][0] = s * m[2][0];                                                                                           \
    dst[2][1] = s * m[2][1];                                                                                           \
    dst[2][2] = s * m[2][2];                                                                                           \
}                                                                                                                      \
void lvn_mat3_##t##_divsm(LvnType_##t s, LvnMat3x3_##t m, LvnMat3x3_##t dst) {                                         \
    dst[0][0] = s / m[0][0];                                                                                           \
    dst[0][1] = s / m[0][1];                                                                                           \
    dst[0][2] = s / m[0][2];                                                                                           \
    dst[1][0] = s / m[1][0];                                                                                           \
    dst[1][1] = s / m[1][1];                                                                                           \
    dst[1][2] = s / m[1][2];                                                                                           \
    dst[2][0] = s / m[2][0];                                                                                           \
    dst[2][1] = s / m[2][1];                                                                                           \
    dst[2][2] = s / m[2][2];                                                                                           \
}

#define LVN_DEFINE_MAT4_TYPE_MATH_IMPL(t)                                                                              \
void lvn_mat4_##t(LvnType_##t* v, LvnMat4x4_##t dst) {                                                                 \
    dst[0][0] = v[0];                                                                                                  \
    dst[0][1] = v[1];                                                                                                  \
    dst[0][2] = v[2];                                                                                                  \
    dst[0][3] = v[3];                                                                                                  \
    dst[1][0] = v[4];                                                                                                  \
    dst[1][1] = v[5];                                                                                                  \
    dst[1][2] = v[6];                                                                                                  \
    dst[1][3] = v[7];                                                                                                  \
    dst[2][0] = v[8];                                                                                                  \
    dst[2][1] = v[9];                                                                                                  \
    dst[2][2] = v[10];                                                                                                 \
    dst[2][3] = v[11];                                                                                                 \
    dst[3][0] = v[12];                                                                                                 \
    dst[3][1] = v[13];                                                                                                 \
    dst[3][2] = v[14];                                                                                                 \
    dst[3][3] = v[15];                                                                                                 \
}                                                                                                                      \
void lvn_mat4_##t##_zero(LvnMat4x4_##t dst) {                                                                          \
    dst[0][0] = (LvnType_##t)0;                                                                                        \
    dst[0][1] = (LvnType_##t)0;                                                                                        \
    dst[0][2] = (LvnType_##t)0;                                                                                        \
    dst[0][3] = (LvnType_##t)0;                                                                                        \
    dst[1][0] = (LvnType_##t)0;                                                                                        \
    dst[1][1] = (LvnType_##t)0;                                                                                        \
    dst[1][2] = (LvnType_##t)0;                                                                                        \
    dst[1][3] = (LvnType_##t)0;                                                                                        \
    dst[2][0] = (LvnType_##t)0;                                                                                        \
    dst[2][1] = (LvnType_##t)0;                                                                                        \
    dst[2][2] = (LvnType_##t)0;                                                                                        \
    dst[2][3] = (LvnType_##t)0;                                                                                        \
    dst[3][0] = (LvnType_##t)0;                                                                                        \
    dst[3][1] = (LvnType_##t)0;                                                                                        \
    dst[3][2] = (LvnType_##t)0;                                                                                        \
    dst[3][3] = (LvnType_##t)0;                                                                                        \
}                                                                                                                      \
void lvn_mat4_##t##_identity(LvnMat4x4_##t dst) {                                                                      \
    dst[0][0] = (LvnType_##t)1;                                                                                        \
    dst[0][1] = (LvnType_##t)0;                                                                                        \
    dst[0][2] = (LvnType_##t)0;                                                                                        \
    dst[0][3] = (LvnType_##t)0;                                                                                        \
    dst[1][0] = (LvnType_##t)0;                                                                                        \
    dst[1][1] = (LvnType_##t)1;                                                                                        \
    dst[1][2] = (LvnType_##t)0;                                                                                        \
    dst[1][3] = (LvnType_##t)0;                                                                                        \
    dst[2][0] = (LvnType_##t)0;                                                                                        \
    dst[2][1] = (LvnType_##t)0;                                                                                        \
    dst[2][2] = (LvnType_##t)1;                                                                                        \
    dst[2][3] = (LvnType_##t)0;                                                                                        \
    dst[3][0] = (LvnType_##t)0;                                                                                        \
    dst[3][1] = (LvnType_##t)0;                                                                                        \
    dst[3][2] = (LvnType_##t)0;                                                                                        \
    dst[3][3] = (LvnType_##t)1;                                                                                        \
}                                                                                                                      \
void lvn_mat4_##t##_copy(LvnMat4x4_##t src, LvnMat4x4_##t dst) {                                                       \
    dst[0][0] = src[0][0];                                                                                             \
    dst[0][1] = src[0][1];                                                                                             \
    dst[0][2] = src[0][2];                                                                                             \
    dst[0][3] = src[0][3];                                                                                             \
    dst[1][0] = src[1][0];                                                                                             \
    dst[1][1] = src[1][1];                                                                                             \
    dst[1][2] = src[1][2];                                                                                             \
    dst[1][3] = src[1][3];                                                                                             \
    dst[2][0] = src[2][0];                                                                                             \
    dst[2][1] = src[2][1];                                                                                             \
    dst[2][2] = src[2][2];                                                                                             \
    dst[2][3] = src[2][3];                                                                                             \
    dst[3][0] = src[3][0];                                                                                             \
    dst[3][1] = src[3][1];                                                                                             \
    dst[3][2] = src[3][2];                                                                                             \
    dst[3][3] = src[3][3];                                                                                             \
}                                                                                                                      \
LvnType_##t lvn_mat4_##t##_det(LvnMat4x4_##t m) {                                                                      \
    LvnType_##t sub00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];                                                         \
    LvnType_##t sub01 = m[2][1] * m[3][3] - m[3][1] * m[2][3];                                                         \
    LvnType_##t sub02 = m[2][1] * m[3][2] - m[3][1] * m[2][2];                                                         \
    LvnType_##t sub03 = m[2][0] * m[3][3] - m[3][0] * m[2][3];                                                         \
    LvnType_##t sub04 = m[2][0] * m[3][2] - m[3][0] * m[2][2];                                                         \
    LvnType_##t sub05 = m[2][0] * m[3][1] - m[3][0] * m[2][1];                                                         \
                                                                                                                       \
    LvnVec4_##t detCof = {                                                                                             \
        + (m[1][1] * sub00 - m[1][2] * sub01 + m[1][3] * sub02),                                                       \
        - (m[1][0] * sub00 - m[1][2] * sub03 + m[1][3] * sub04),                                                       \
        + (m[1][0] * sub01 - m[1][1] * sub03 + m[1][3] * sub05),                                                       \
        - (m[1][0] * sub02 - m[1][1] * sub04 + m[1][2] * sub05)};                                                      \
                                                                                                                       \
    return m[0][0] * detCof[0] + m[0][1] * detCof[1] +                                                                 \
           m[0][2] * detCof[2] + m[0][3] * detCof[3];                                                                  \
}                                                                                                                      \
void lvn_mat4_##t##_inv(LvnMat4x4_##t mat, LvnMat4x4_##t dst) {                                                        \
    float a = mat[0][0], b = mat[0][1], c = mat[0][2], d = mat[0][3],                                                  \
          e = mat[1][0], f = mat[1][1], g = mat[1][2], h = mat[1][3],                                                  \
          i = mat[2][0], j = mat[2][1], k = mat[2][2], l = mat[2][3],                                                  \
          m = mat[3][0], n = mat[3][1], o = mat[3][2], p = mat[3][3],                                                  \
          c1  = k * p - l * o,  c2  = c * h - d * g,  c3  = i * p - l * m,                                             \
          c4  = a * h - d * e,  c5  = j * p - l * n,  c6  = b * h - d * f,                                             \
          c7  = i * n - j * m,  c8  = a * f - b * e,  c9  = j * o - k * n,                                             \
          c10 = b * g - c * f,  c11 = i * o - k * m,  c12 = a * g - c * e,                                             \
          idt = 1.0f/(c8*c1+c4*c9+c10*c3+c2*c7-c12*c5-c6*c11), ndt = -idt;                                             \
    dst[0][0] = (f * c1  - g * c5  + h * c9)  * idt;                                                                   \
    dst[0][1] = (b * c1  - c * c5  + d * c9)  * ndt;                                                                   \
    dst[0][2] = (n * c2  - o * c6  + p * c10) * idt;                                                                   \
    dst[0][3] = (j * c2  - k * c6  + l * c10) * ndt;                                                                   \
    dst[1][0] = (e * c1  - g * c3  + h * c11) * ndt;                                                                   \
    dst[1][1] = (a * c1  - c * c3  + d * c11) * idt;                                                                   \
    dst[1][2] = (m * c2  - o * c4  + p * c12) * ndt;                                                                   \
    dst[1][3] = (i * c2  - k * c4  + l * c12) * idt;                                                                   \
    dst[2][0] = (e * c5  - f * c3  + h * c7)  * idt;                                                                   \
    dst[2][1] = (a * c5  - b * c3  + d * c7)  * ndt;                                                                   \
    dst[2][2] = (m * c6  - n * c4  + p * c8)  * idt;                                                                   \
    dst[2][3] = (i * c6  - j * c4  + l * c8)  * ndt;                                                                   \
    dst[3][0] = (e * c9  - f * c11 + g * c7)  * ndt;                                                                   \
    dst[3][1] = (a * c9  - b * c11 + c * c7)  * idt;                                                                   \
    dst[3][2] = (m * c10 - n * c12 + o * c8)  * ndt;                                                                   \
    dst[3][3] = (i * c10 - j * c12 + k * c8)  * idt;                                                                   \
}                                                                                                                      \
void lvn_mat4_##t##_transpose_to(LvnMat4x4_##t m, LvnMat4x4_##t dst) {                                                 \
    dst[0][0] = m[0][0];                                                                                               \
    dst[0][1] = m[1][0];                                                                                               \
    dst[0][2] = m[2][0];                                                                                               \
    dst[0][3] = m[3][0];                                                                                               \
    dst[1][0] = m[0][1];                                                                                               \
    dst[1][1] = m[1][1];                                                                                               \
    dst[1][2] = m[2][1];                                                                                               \
    dst[1][3] = m[3][1];                                                                                               \
    dst[2][0] = m[0][2];                                                                                               \
    dst[2][1] = m[1][2];                                                                                               \
    dst[2][2] = m[2][2];                                                                                               \
    dst[2][3] = m[3][2];                                                                                               \
    dst[3][0] = m[0][3];                                                                                               \
    dst[3][1] = m[1][3];                                                                                               \
    dst[3][2] = m[2][3];                                                                                               \
    dst[3][3] = m[3][3];                                                                                               \
}                                                                                                                      \
void lvn_mat4_##t##_transpose(LvnMat4x4_##t m) {                                                                       \
    LvnMat4x4_##t temp;                                                                                                \
    temp[0][0] = m[0][0];                                                                                              \
    temp[0][1] = m[1][0];                                                                                              \
    temp[0][2] = m[2][0];                                                                                              \
    temp[0][3] = m[3][0];                                                                                              \
    temp[1][0] = m[0][1];                                                                                              \
    temp[1][1] = m[1][1];                                                                                              \
    temp[1][2] = m[2][1];                                                                                              \
    temp[1][3] = m[3][1];                                                                                              \
    temp[2][0] = m[0][2];                                                                                              \
    temp[2][1] = m[1][2];                                                                                              \
    temp[2][2] = m[2][2];                                                                                              \
    temp[2][3] = m[3][2];                                                                                              \
    temp[3][0] = m[0][3];                                                                                              \
    temp[3][1] = m[1][3];                                                                                              \
    temp[3][2] = m[2][3];                                                                                              \
    temp[3][3] = m[3][3];                                                                                              \
    m[0][0] = temp[0][0];                                                                                              \
    m[0][1] = temp[0][1];                                                                                              \
    m[0][2] = temp[0][2];                                                                                              \
    m[0][3] = temp[0][3];                                                                                              \
    m[1][0] = temp[1][0];                                                                                              \
    m[1][1] = temp[1][1];                                                                                              \
    m[1][2] = temp[1][2];                                                                                              \
    m[1][3] = temp[1][3];                                                                                              \
    m[2][0] = temp[2][0];                                                                                              \
    m[2][1] = temp[2][1];                                                                                              \
    m[2][2] = temp[2][2];                                                                                              \
    m[2][3] = temp[2][3];                                                                                              \
    m[3][0] = temp[3][0];                                                                                              \
    m[3][1] = temp[3][1];                                                                                              \
    m[3][2] = temp[3][2];                                                                                              \
    m[3][3] = temp[3][3];                                                                                              \
}                                                                                                                      \
void lvn_mat4_##t##_negate_to(LvnMat4x4_##t m, LvnMat4x4_##t dst) {                                                    \
    dst[0][0] = -m[0][0];                                                                                              \
    dst[0][1] = -m[0][1];                                                                                              \
    dst[0][2] = -m[0][2];                                                                                              \
    dst[0][3] = -m[0][3];                                                                                              \
    dst[1][0] = -m[1][0];                                                                                              \
    dst[1][1] = -m[1][1];                                                                                              \
    dst[1][2] = -m[1][2];                                                                                              \
    dst[1][3] = -m[1][3];                                                                                              \
    dst[2][0] = -m[2][0];                                                                                              \
    dst[2][1] = -m[2][1];                                                                                              \
    dst[2][2] = -m[2][2];                                                                                              \
    dst[2][3] = -m[2][3];                                                                                              \
    dst[3][0] = -m[3][0];                                                                                              \
    dst[3][1] = -m[3][1];                                                                                              \
    dst[3][2] = -m[3][2];                                                                                              \
    dst[3][3] = -m[3][3];                                                                                              \
}                                                                                                                      \
void lvn_mat4_##t##_negate(LvnMat4x4_##t m) {                                                                          \
    m[0][0] = -m[0][0];                                                                                                \
    m[0][1] = -m[0][1];                                                                                                \
    m[0][2] = -m[0][2];                                                                                                \
    m[0][3] = -m[0][3];                                                                                                \
    m[1][0] = -m[1][0];                                                                                                \
    m[1][1] = -m[1][1];                                                                                                \
    m[1][2] = -m[1][2];                                                                                                \
    m[1][3] = -m[1][3];                                                                                                \
    m[2][0] = -m[2][0];                                                                                                \
    m[2][1] = -m[2][1];                                                                                                \
    m[2][2] = -m[2][2];                                                                                                \
    m[2][3] = -m[2][3];                                                                                                \
    m[3][0] = -m[3][0];                                                                                                \
    m[3][1] = -m[3][1];                                                                                                \
    m[3][2] = -m[3][2];                                                                                                \
    m[3][3] = -m[3][3];                                                                                                \
}                                                                                                                      \
void lvn_mat4_##t##_add(LvnMat4x4_##t m1, LvnMat4x4_##t m2, LvnMat4x4_##t dst) {                                       \
    dst[0][0] = m1[0][0] + m2[0][0];                                                                                   \
    dst[0][1] = m1[0][1] + m2[0][1];                                                                                   \
    dst[0][2] = m1[0][2] + m2[0][2];                                                                                   \
    dst[0][3] = m1[0][3] + m2[0][3];                                                                                   \
    dst[1][0] = m1[1][0] + m2[1][0];                                                                                   \
    dst[1][1] = m1[1][1] + m2[1][1];                                                                                   \
    dst[1][2] = m1[1][2] + m2[1][2];                                                                                   \
    dst[1][3] = m1[1][3] + m2[1][3];                                                                                   \
    dst[2][0] = m1[2][0] + m2[2][0];                                                                                   \
    dst[2][1] = m1[2][1] + m2[2][1];                                                                                   \
    dst[2][2] = m1[2][2] + m2[2][2];                                                                                   \
    dst[2][3] = m1[2][3] + m2[2][3];                                                                                   \
    dst[3][0] = m1[3][0] + m2[3][0];                                                                                   \
    dst[3][1] = m1[3][1] + m2[3][1];                                                                                   \
    dst[3][2] = m1[3][2] + m2[3][2];                                                                                   \
    dst[3][3] = m1[3][3] + m2[3][3];                                                                                   \
}                                                                                                                      \
void lvn_mat4_##t##_sub(LvnMat4x4_##t m1, LvnMat4x4_##t m2, LvnMat4x4_##t dst) {                                       \
    dst[0][0] = m1[0][0] - m2[0][0];                                                                                   \
    dst[0][1] = m1[0][1] - m2[0][1];                                                                                   \
    dst[0][2] = m1[0][2] - m2[0][2];                                                                                   \
    dst[0][3] = m1[0][3] - m2[0][3];                                                                                   \
    dst[1][0] = m1[1][0] - m2[1][0];                                                                                   \
    dst[1][1] = m1[1][1] - m2[1][1];                                                                                   \
    dst[1][2] = m1[1][2] - m2[1][2];                                                                                   \
    dst[1][3] = m1[1][3] - m2[1][3];                                                                                   \
    dst[2][0] = m1[2][0] - m2[2][0];                                                                                   \
    dst[2][1] = m1[2][1] - m2[2][1];                                                                                   \
    dst[2][2] = m1[2][2] - m2[2][2];                                                                                   \
    dst[2][3] = m1[2][3] - m2[2][3];                                                                                   \
    dst[3][0] = m1[3][0] - m2[3][0];                                                                                   \
    dst[3][1] = m1[3][1] - m2[3][1];                                                                                   \
    dst[3][2] = m1[3][2] - m2[3][2];                                                                                   \
    dst[3][3] = m1[3][3] - m2[3][3];                                                                                   \
}                                                                                                                      \
void lvn_mat4_##t##_mult(LvnMat4x4_##t m1, LvnMat4x4_##t m2, LvnMat4x4_##t dst) {                                      \
    dst[0][0] = m1[0][0] * m2[0][0] + m1[1][0] * m2[0][1] + m1[2][0] * m2[0][2] + m1[3][0] * m2[0][3];                 \
    dst[0][1] = m1[0][1] * m2[0][0] + m1[1][1] * m2[0][1] + m1[2][1] * m2[0][2] + m1[3][1] * m2[0][3];                 \
    dst[0][2] = m1[0][2] * m2[0][0] + m1[1][2] * m2[0][1] + m1[2][2] * m2[0][2] + m1[3][2] * m2[0][3];                 \
    dst[0][3] = m1[0][3] * m2[0][0] + m1[1][3] * m2[0][1] + m1[2][3] * m2[0][2] + m1[3][3] * m2[0][3];                 \
    dst[1][0] = m1[0][0] * m2[1][0] + m1[1][0] * m2[1][1] + m1[2][0] * m2[1][2] + m1[3][0] * m2[1][3];                 \
    dst[1][1] = m1[0][1] * m2[1][0] + m1[1][1] * m2[1][1] + m1[2][1] * m2[1][2] + m1[3][1] * m2[1][3];                 \
    dst[1][2] = m1[0][2] * m2[1][0] + m1[1][2] * m2[1][1] + m1[2][2] * m2[1][2] + m1[3][2] * m2[1][3];                 \
    dst[1][3] = m1[0][3] * m2[1][0] + m1[1][3] * m2[1][1] + m1[2][3] * m2[1][2] + m1[3][3] * m2[1][3];                 \
    dst[2][0] = m1[0][0] * m2[2][0] + m1[1][0] * m2[2][1] + m1[2][0] * m2[2][2] + m1[3][0] * m2[2][3];                 \
    dst[2][1] = m1[0][1] * m2[2][0] + m1[1][1] * m2[2][1] + m1[2][1] * m2[2][2] + m1[3][1] * m2[2][3];                 \
    dst[2][2] = m1[0][2] * m2[2][0] + m1[1][2] * m2[2][1] + m1[2][2] * m2[2][2] + m1[3][2] * m2[2][3];                 \
    dst[2][3] = m1[0][3] * m2[2][0] + m1[1][3] * m2[2][1] + m1[2][3] * m2[2][2] + m1[3][3] * m2[2][3];                 \
    dst[3][0] = m1[0][0] * m2[3][0] + m1[1][0] * m2[3][1] + m1[2][0] * m2[3][2] + m1[3][0] * m2[3][3];                 \
    dst[3][1] = m1[0][1] * m2[3][0] + m1[1][1] * m2[3][1] + m1[2][1] * m2[3][2] + m1[3][1] * m2[3][3];                 \
    dst[3][2] = m1[0][2] * m2[3][0] + m1[1][2] * m2[3][1] + m1[2][2] * m2[3][2] + m1[3][2] * m2[3][3];                 \
    dst[3][3] = m1[0][3] * m2[3][0] + m1[1][3] * m2[3][1] + m1[2][3] * m2[3][2] + m1[3][3] * m2[3][3];                 \
}                                                                                                                      \
void lvn_mat4_##t##_multmv(LvnMat4x4_##t m, LvnVec4_##t v, LvnVec4_##t dst) {                                          \
    dst[0] = m[0][0] * v[0] + m[1][0] * v[1] + m[2][0] * v[2] + m[3][0] * v[3];                                        \
    dst[1] = m[0][1] * v[0] + m[1][1] * v[1] + m[2][1] * v[2] + m[3][1] * v[3];                                        \
    dst[2] = m[0][2] * v[0] + m[1][2] * v[1] + m[2][2] * v[2] + m[3][2] * v[3];                                        \
    dst[3] = m[0][3] * v[0] + m[1][3] * v[1] + m[2][3] * v[2] + m[3][3] * v[3];                                        \
}                                                                                                                      \
void lvn_mat4_##t##_multvm(LvnVec4_##t v, LvnMat4x4_##t m, LvnVec4_##t dst) {                                          \
    dst[0] = v[0] * m[0][0] + v[1] * m[0][1] + v[2] * m[0][2] + v[3] * m[0][3];                                        \
    dst[1] = v[0] * m[1][0] + v[1] * m[1][1] + v[2] * m[1][2] + v[3] * m[1][3];                                        \
    dst[2] = v[0] * m[2][0] + v[1] * m[2][1] + v[2] * m[2][2] + v[3] * m[2][3];                                        \
    dst[3] = v[0] * m[3][0] + v[1] * m[3][1] + v[2] * m[3][2] + v[3] * m[3][3];                                        \
}                                                                                                                      \
void lvn_mat4_##t##_addms(LvnMat4x4_##t m, LvnType_##t s, LvnMat4x4_##t dst) {                                         \
    dst[0][0] = m[0][0] + s;                                                                                           \
    dst[0][1] = m[0][1] + s;                                                                                           \
    dst[0][2] = m[0][2] + s;                                                                                           \
    dst[0][3] = m[0][3] + s;                                                                                           \
    dst[1][0] = m[1][0] + s;                                                                                           \
    dst[1][1] = m[1][1] + s;                                                                                           \
    dst[1][2] = m[1][2] + s;                                                                                           \
    dst[1][3] = m[1][3] + s;                                                                                           \
    dst[2][0] = m[2][0] + s;                                                                                           \
    dst[2][1] = m[2][1] + s;                                                                                           \
    dst[2][2] = m[2][2] + s;                                                                                           \
    dst[2][3] = m[2][3] + s;                                                                                           \
    dst[3][0] = m[3][0] + s;                                                                                           \
    dst[3][1] = m[3][1] + s;                                                                                           \
    dst[3][2] = m[3][2] + s;                                                                                           \
    dst[3][3] = m[3][3] + s;                                                                                           \
}                                                                                                                      \
void lvn_mat4_##t##_subms(LvnMat4x4_##t m, LvnType_##t s, LvnMat4x4_##t dst) {                                         \
    dst[0][0] = m[0][0] - s;                                                                                           \
    dst[0][1] = m[0][1] - s;                                                                                           \
    dst[0][2] = m[0][2] - s;                                                                                           \
    dst[0][3] = m[0][3] - s;                                                                                           \
    dst[1][0] = m[1][0] - s;                                                                                           \
    dst[1][1] = m[1][1] - s;                                                                                           \
    dst[1][2] = m[1][2] - s;                                                                                           \
    dst[1][3] = m[1][3] - s;                                                                                           \
    dst[2][0] = m[2][0] - s;                                                                                           \
    dst[2][1] = m[2][1] - s;                                                                                           \
    dst[2][2] = m[2][2] - s;                                                                                           \
    dst[2][3] = m[2][3] - s;                                                                                           \
    dst[3][0] = m[3][0] - s;                                                                                           \
    dst[3][1] = m[3][1] - s;                                                                                           \
    dst[3][2] = m[3][2] - s;                                                                                           \
    dst[3][3] = m[3][3] - s;                                                                                           \
}                                                                                                                      \
void lvn_mat4_##t##_multms(LvnMat4x4_##t m, LvnType_##t s, LvnMat4x4_##t dst) {                                        \
    dst[0][0] = m[0][0] * s;                                                                                           \
    dst[0][1] = m[0][1] * s;                                                                                           \
    dst[0][2] = m[0][2] * s;                                                                                           \
    dst[0][3] = m[0][3] * s;                                                                                           \
    dst[1][0] = m[1][0] * s;                                                                                           \
    dst[1][1] = m[1][1] * s;                                                                                           \
    dst[1][2] = m[1][2] * s;                                                                                           \
    dst[1][3] = m[1][3] * s;                                                                                           \
    dst[2][0] = m[2][0] * s;                                                                                           \
    dst[2][1] = m[2][1] * s;                                                                                           \
    dst[2][2] = m[2][2] * s;                                                                                           \
    dst[2][3] = m[2][3] * s;                                                                                           \
    dst[3][0] = m[3][0] * s;                                                                                           \
    dst[3][1] = m[3][1] * s;                                                                                           \
    dst[3][2] = m[3][2] * s;                                                                                           \
    dst[3][3] = m[3][3] * s;                                                                                           \
}                                                                                                                      \
void lvn_mat4_##t##_divms(LvnMat4x4_##t m, LvnType_##t s, LvnMat4x4_##t dst) {                                         \
    dst[0][0] = m[0][0] / s;                                                                                           \
    dst[0][1] = m[0][1] / s;                                                                                           \
    dst[0][2] = m[0][2] / s;                                                                                           \
    dst[0][3] = m[0][3] / s;                                                                                           \
    dst[1][0] = m[1][0] / s;                                                                                           \
    dst[1][1] = m[1][1] / s;                                                                                           \
    dst[1][2] = m[1][2] / s;                                                                                           \
    dst[1][3] = m[1][3] / s;                                                                                           \
    dst[2][0] = m[2][0] / s;                                                                                           \
    dst[2][1] = m[2][1] / s;                                                                                           \
    dst[2][2] = m[2][2] / s;                                                                                           \
    dst[2][3] = m[2][3] / s;                                                                                           \
    dst[3][0] = m[3][0] / s;                                                                                           \
    dst[3][1] = m[3][1] / s;                                                                                           \
    dst[3][2] = m[3][2] / s;                                                                                           \
    dst[3][3] = m[3][3] / s;                                                                                           \
}                                                                                                                      \
void lvn_mat4_##t##_addsm(LvnType_##t s, LvnMat4x4_##t m, LvnMat4x4_##t dst) {                                         \
    dst[0][0] = s + m[0][0];                                                                                           \
    dst[0][1] = s + m[0][1];                                                                                           \
    dst[0][2] = s + m[0][2];                                                                                           \
    dst[0][3] = s + m[0][3];                                                                                           \
    dst[1][0] = s + m[1][0];                                                                                           \
    dst[1][1] = s + m[1][1];                                                                                           \
    dst[1][2] = s + m[1][2];                                                                                           \
    dst[1][3] = s + m[1][3];                                                                                           \
    dst[2][0] = s + m[2][0];                                                                                           \
    dst[2][1] = s + m[2][1];                                                                                           \
    dst[2][2] = s + m[2][2];                                                                                           \
    dst[2][3] = s + m[2][3];                                                                                           \
    dst[3][0] = s + m[3][0];                                                                                           \
    dst[3][1] = s + m[3][1];                                                                                           \
    dst[3][2] = s + m[3][2];                                                                                           \
    dst[3][3] = s + m[3][3];                                                                                           \
}                                                                                                                      \
void lvn_mat4_##t##_subsm(LvnType_##t s, LvnMat4x4_##t m, LvnMat4x4_##t dst) {                                         \
    dst[0][0] = s - m[0][0];                                                                                           \
    dst[0][1] = s - m[0][1];                                                                                           \
    dst[0][2] = s - m[0][2];                                                                                           \
    dst[0][3] = s - m[0][3];                                                                                           \
    dst[1][0] = s - m[1][0];                                                                                           \
    dst[1][1] = s - m[1][1];                                                                                           \
    dst[1][2] = s - m[1][2];                                                                                           \
    dst[1][3] = s - m[1][3];                                                                                           \
    dst[2][0] = s - m[2][0];                                                                                           \
    dst[2][1] = s - m[2][1];                                                                                           \
    dst[2][2] = s - m[2][2];                                                                                           \
    dst[2][3] = s - m[2][3];                                                                                           \
    dst[3][0] = s - m[3][0];                                                                                           \
    dst[3][1] = s - m[3][1];                                                                                           \
    dst[3][2] = s - m[3][2];                                                                                           \
    dst[3][3] = s - m[3][3];                                                                                           \
}                                                                                                                      \
void lvn_mat4_##t##_multsm(LvnType_##t s, LvnMat4x4_##t m, LvnMat4x4_##t dst) {                                        \
    dst[0][0] = s * m[0][0];                                                                                           \
    dst[0][1] = s * m[0][1];                                                                                           \
    dst[0][2] = s * m[0][2];                                                                                           \
    dst[0][3] = s * m[0][3];                                                                                           \
    dst[1][0] = s * m[1][0];                                                                                           \
    dst[1][1] = s * m[1][1];                                                                                           \
    dst[1][2] = s * m[1][2];                                                                                           \
    dst[1][3] = s * m[1][3];                                                                                           \
    dst[2][0] = s * m[2][0];                                                                                           \
    dst[2][1] = s * m[2][1];                                                                                           \
    dst[2][2] = s * m[2][2];                                                                                           \
    dst[2][3] = s * m[2][3];                                                                                           \
    dst[3][0] = s * m[3][0];                                                                                           \
    dst[3][1] = s * m[3][1];                                                                                           \
    dst[3][2] = s * m[3][2];                                                                                           \
    dst[3][3] = s * m[3][3];                                                                                           \
}                                                                                                                      \
void lvn_mat4_##t##_divsm(LvnType_##t s, LvnMat4x4_##t m, LvnMat4x4_##t dst) {                                         \
    dst[0][0] = s / m[0][0];                                                                                           \
    dst[0][1] = s / m[0][1];                                                                                           \
    dst[0][2] = s / m[0][2];                                                                                           \
    dst[0][3] = s / m[0][3];                                                                                           \
    dst[1][0] = s / m[1][0];                                                                                           \
    dst[1][1] = s / m[1][1];                                                                                           \
    dst[1][2] = s / m[1][2];                                                                                           \
    dst[1][3] = s / m[1][3];                                                                                           \
    dst[2][0] = s / m[2][0];                                                                                           \
    dst[2][1] = s / m[2][1];                                                                                           \
    dst[2][2] = s / m[2][2];                                                                                           \
    dst[2][3] = s / m[2][3];                                                                                           \
    dst[3][0] = s / m[3][0];                                                                                           \
    dst[3][1] = s / m[3][1];                                                                                           \
    dst[3][2] = s / m[3][2];                                                                                           \
    dst[3][3] = s / m[3][3];                                                                                           \
}


#define LVN_DEFINE_VEC_F_TYPE_MATH_IMPL(n)                                                                             \
void lvn_vec##n(float* v, LvnVec##n dst) { lvn_vec##n##_f(v, dst); }                                                   \
void lvn_vec##n##_s(float s, LvnVec##n dst) { lvn_vec##n##_f_s(s, dst); }                                              \
void lvn_vec##n##_zero(LvnVec##n dst) { lvn_vec##n##_f_zero(dst); }                                                    \
void lvn_vec##n##_one(LvnVec##n dst) { lvn_vec##n##_f_one(dst); }                                                      \
void lvn_vec##n##_copy(LvnVec##n src, LvnVec##n dst) { lvn_vec##n##_f_copy(src, dst); }                                \
void lvn_vec##n##_negate(LvnVec##n v) { lvn_vec##n##_f_negate(v); }                                                    \
void lvn_vec##n##_negate_to(LvnVec##n v, LvnVec##n dst) { lvn_vec##n##_f_negate_to(v, dst); }                          \
void lvn_vec##n##_add(LvnVec##n v1, LvnVec##n v2, LvnVec##n dst) { lvn_vec##n##_f_add(v1, v2, dst); }                  \
void lvn_vec##n##_sub(LvnVec##n v1, LvnVec##n v2, LvnVec##n dst) { lvn_vec##n##_f_sub(v1, v2, dst); }                  \
void lvn_vec##n##_mult(LvnVec##n v1, LvnVec##n v2, LvnVec##n dst) { lvn_vec##n##_f_mult(v1, v2, dst); }                \
void lvn_vec##n##_div(LvnVec##n v1, LvnVec##n v2, LvnVec##n dst) { lvn_vec##n##_f_div(v1, v2, dst); }                  \
void lvn_vec##n##_addvs(LvnVec##n v, float s, LvnVec##n dst) { lvn_vec##n##_f_addvs(v, s, dst); }                      \
void lvn_vec##n##_subvs(LvnVec##n v, float s, LvnVec##n dst) { lvn_vec##n##_f_subvs(v, s, dst); }                      \
void lvn_vec##n##_multvs(LvnVec##n v, float s, LvnVec##n dst) { lvn_vec##n##_f_multvs(v, s, dst); }                    \
void lvn_vec##n##_divvs(LvnVec##n v, float s, LvnVec##n dst) { lvn_vec##n##_f_divvs(v, s, dst); }                      \
void lvn_vec##n##_addsv(float s, LvnVec##n v, LvnVec##n dst) { lvn_vec##n##_f_addsv(s, v, dst); }                      \
void lvn_vec##n##_subsv(float s, LvnVec##n v, LvnVec##n dst) { lvn_vec##n##_f_subsv(s, v, dst); }                      \
void lvn_vec##n##_multsv(float s, LvnVec##n v, LvnVec##n dst) { lvn_vec##n##_f_multsv(s, v, dst); }                    \
void lvn_vec##n##_divsv(float s, LvnVec##n v, LvnVec##n dst) { lvn_vec##n##_f_divsv(s, v, dst); }                      \
void lvn_vec##n##_addpv(LvnVec##n* pv[], uint32_t count, LvnVec##n dst) { lvn_vec##n##_f_addpv(pv, count, dst); }      \
void lvn_vec##n##_subpv(LvnVec##n* pv[], uint32_t count, LvnVec##n dst) { lvn_vec##n##_f_subpv(pv, count, dst); }      \
void lvn_vec##n##_multpv(LvnVec##n* pv[], uint32_t count, LvnVec##n dst) { lvn_vec##n##_f_multpv(pv, count, dst); }    \
void lvn_vec##n##_divpv(LvnVec##n* pv[], uint32_t count, LvnVec##n dst) { lvn_vec##n##_f_divpv(pv, count, dst); }      \
float lvn_vec##n##_mag(LvnVec##n v) { return lvn_vec##n##_f_mag(v); }                                                  \
float lvn_vec##n##_mag2(LvnVec##n v) { return lvn_vec##n##_f_mag2(v); }                                                \
float lvn_vec##n##_dot(LvnVec##n v1, LvnVec##n v2) { return lvn_vec##n##_f_dot(v1, v2); }

#define LVN_DEFINE_VEC_I32_TYPE_MATH_IMPL(n)                                                                           \
void lvn_vec##n##i(int32_t* v, LvnVec##n##i dst) { lvn_vec##n##_i32(v, dst); }                                         \
void lvn_vec##n##i_s(int32_t s, LvnVec##n##i dst) { lvn_vec##n##_i32_s(s, dst); }                                      \
void lvn_vec##n##i_zero(LvnVec##n##i dst) { lvn_vec##n##_i32_zero(dst); }                                              \
void lvn_vec##n##i_one(LvnVec##n##i dst) { lvn_vec##n##_i32_one(dst); }                                                \
void lvn_vec##n##i_copy(LvnVec##n##i src, LvnVec##n##i dst) { lvn_vec##n##_i32_copy(src, dst); }                       \
void lvn_vec##n##i_negate(LvnVec##n##i v) { lvn_vec##n##_i32_negate(v); }                                              \
void lvn_vec##n##i_negate_to(LvnVec##n##i v, LvnVec##n##i dst) { lvn_vec##n##_i32_negate_to(v, dst); }                 \
void lvn_vec##n##i_add(LvnVec##n##i v1, LvnVec##n##i v2, LvnVec##n##i dst) { lvn_vec##n##_i32_add(v1, v2, dst); }      \
void lvn_vec##n##i_sub(LvnVec##n##i v1, LvnVec##n##i v2, LvnVec##n##i dst) { lvn_vec##n##_i32_sub(v1, v2, dst); }      \
void lvn_vec##n##i_mult(LvnVec##n##i v1, LvnVec##n##i v2, LvnVec##n##i dst) { lvn_vec##n##_i32_mult(v1, v2, dst); }    \
void lvn_vec##n##i_div(LvnVec##n##i v1, LvnVec##n##i v2, LvnVec##n##i dst) { lvn_vec##n##_i32_div(v1, v2, dst); }      \
void lvn_vec##n##i_addvs(LvnVec##n##i v, int32_t s, LvnVec##n##i dst) { lvn_vec##n##_i32_addvs(v, s, dst); }           \
void lvn_vec##n##i_subvs(LvnVec##n##i v, int32_t s, LvnVec##n##i dst) { lvn_vec##n##_i32_subvs(v, s, dst); }           \
void lvn_vec##n##i_multvs(LvnVec##n##i v, int32_t s, LvnVec##n##i dst) { lvn_vec##n##_i32_multvs(v, s, dst); }         \
void lvn_vec##n##i_divvs(LvnVec##n##i v, int32_t s, LvnVec##n##i dst) { lvn_vec##n##_i32_divvs(v, s, dst); }           \
void lvn_vec##n##i_addsv(int32_t s, LvnVec##n##i v, LvnVec##n##i dst) { lvn_vec##n##_i32_addsv(s, v, dst); }           \
void lvn_vec##n##i_subsv(int32_t s, LvnVec##n##i v, LvnVec##n##i dst) { lvn_vec##n##_i32_subsv(s, v, dst); }           \
void lvn_vec##n##i_multsv(int32_t s, LvnVec##n##i v, LvnVec##n##i dst) { lvn_vec##n##_i32_multsv(s, v, dst); }         \
void lvn_vec##n##i_divsv(int32_t s, LvnVec##n##i v, LvnVec##n##i dst) { lvn_vec##n##_i32_divsv(s, v, dst); }           \
void lvn_vec##n##i_addpv(LvnVec##n##i* pv[], uint32_t count, LvnVec##n##i dst) { lvn_vec##n##_i32_addpv(pv, count, dst); }\
void lvn_vec##n##i_subpv(LvnVec##n##i* pv[], uint32_t count, LvnVec##n##i dst) { lvn_vec##n##_i32_subpv(pv, count, dst); }\
void lvn_vec##n##i_multpv(LvnVec##n##i* pv[], uint32_t count, LvnVec##n##i dst) { lvn_vec##n##_i32_multpv(pv, count, dst); }\
void lvn_vec##n##i_divpv(LvnVec##n##i* pv[], uint32_t count, LvnVec##n##i dst) { lvn_vec##n##_i32_divpv(pv, count, dst); }\
int32_t lvn_vec##n##i_mag(LvnVec##n##i v) { return lvn_vec##n##_i32_mag(v); }                                          \
int32_t lvn_vec##n##i_mag2(LvnVec##n##i v) { return lvn_vec##n##_i32_mag2(v); }                                        \
int32_t lvn_vec##n##i_dot(LvnVec##n##i v1, LvnVec##n##i v2) { return lvn_vec##n##_i32_dot(v1, v2); }

#define LVN_DEFINE_VEC_UI32_TYPE_MATH_IMPL(n)                                                                          \
void lvn_vec##n##ui(uint32_t* v, LvnVec##n##ui dst) { lvn_vec##n##_ui32(v, dst); }                                     \
void lvn_vec##n##ui_s(uint32_t s, LvnVec##n##ui dst) { lvn_vec##n##_ui32_s(s, dst); }                                  \
void lvn_vec##n##ui_zero(LvnVec##n##ui dst) { lvn_vec##n##_ui32_zero(dst); }                                           \
void lvn_vec##n##ui_one(LvnVec##n##ui dst) { lvn_vec##n##_ui32_one(dst); }                                             \
void lvn_vec##n##ui_copy(LvnVec##n##ui src, LvnVec##n##ui dst) { lvn_vec##n##_ui32_copy(src, dst); }                   \
void lvn_vec##n##ui_negate(LvnVec##n##ui v) { lvn_vec##n##_ui32_negate(v); }                                           \
void lvn_vec##n##ui_negate_to(LvnVec##n##ui v, LvnVec##n##ui dst) { lvn_vec##n##_ui32_negate_to(v, dst); }             \
void lvn_vec##n##ui_add(LvnVec##n##ui v1, LvnVec##n##ui v2, LvnVec##n##ui dst) { lvn_vec##n##_ui32_add(v1, v2, dst); } \
void lvn_vec##n##ui_sub(LvnVec##n##ui v1, LvnVec##n##ui v2, LvnVec##n##ui dst) { lvn_vec##n##_ui32_sub(v1, v2, dst); } \
void lvn_vec##n##ui_mult(LvnVec##n##ui v1, LvnVec##n##ui v2, LvnVec##n##ui dst) { lvn_vec##n##_ui32_mult(v1, v2, dst); }\
void lvn_vec##n##ui_div(LvnVec##n##ui v1, LvnVec##n##ui v2, LvnVec##n##ui dst) { lvn_vec##n##_ui32_div(v1, v2, dst); } \
void lvn_vec##n##ui_addvs(LvnVec##n##ui v, uint32_t s, LvnVec##n##ui dst) { lvn_vec##n##_ui32_addvs(v, s, dst); }      \
void lvn_vec##n##ui_subvs(LvnVec##n##ui v, uint32_t s, LvnVec##n##ui dst) { lvn_vec##n##_ui32_subvs(v, s, dst); }      \
void lvn_vec##n##ui_multvs(LvnVec##n##ui v, uint32_t s, LvnVec##n##ui dst) { lvn_vec##n##_ui32_multvs(v, s, dst); }    \
void lvn_vec##n##ui_divvs(LvnVec##n##ui v, uint32_t s, LvnVec##n##ui dst) { lvn_vec##n##_ui32_divvs(v, s, dst); }      \
void lvn_vec##n##ui_addsv(uint32_t s, LvnVec##n##ui v, LvnVec##n##ui dst) { lvn_vec##n##_ui32_addsv(s, v, dst); }      \
void lvn_vec##n##ui_subsv(uint32_t s, LvnVec##n##ui v, LvnVec##n##ui dst) { lvn_vec##n##_ui32_subsv(s, v, dst); }      \
void lvn_vec##n##ui_multsv(uint32_t s, LvnVec##n##ui v, LvnVec##n##ui dst) { lvn_vec##n##_ui32_multsv(s, v, dst); }    \
void lvn_vec##n##ui_divsv(uint32_t s, LvnVec##n##ui v, LvnVec##n##ui dst) { lvn_vec##n##_ui32_divsv(s, v, dst); }      \
void lvn_vec##n##ui_addpv(LvnVec##n##ui* pv[], uint32_t count, LvnVec##n##ui dst) { lvn_vec##n##_ui32_addpv(pv, count, dst); }\
void lvn_vec##n##ui_subpv(LvnVec##n##ui* pv[], uint32_t count, LvnVec##n##ui dst) { lvn_vec##n##_ui32_subpv(pv, count, dst); }\
void lvn_vec##n##ui_multpv(LvnVec##n##ui* pv[], uint32_t count, LvnVec##n##ui dst) { lvn_vec##n##_ui32_multpv(pv, count, dst); }\
void lvn_vec##n##ui_divpv(LvnVec##n##ui* pv[], uint32_t count, LvnVec##n##ui dst) { lvn_vec##n##_ui32_divpv(pv, count, dst); }\
uint32_t lvn_vec##n##ui_mag(LvnVec##n##ui v) { return lvn_vec##n##_ui32_mag(v); }                                      \
uint32_t lvn_vec##n##ui_mag2(LvnVec##n##ui v) { return lvn_vec##n##_ui32_mag2(v); }                                    \
uint32_t lvn_vec##n##ui_dot(LvnVec##n##ui v1, LvnVec##n##ui v2) { return lvn_vec##n##_ui32_dot(v1, v2); }

#define LVN_DEFINE_MAT_F_TYPE_MATH_IMPL(n)                                                                             \
void lvn_mat##n(float* v, LvnMat##n##x##n dst) { lvn_mat##n##_f(v, dst); }                                             \
void lvn_mat##n##_zero(LvnMat##n##x##n dst) { lvn_mat##n##_f_zero(dst); }                                              \
void lvn_mat##n##_identity(LvnMat##n##x##n dst) { lvn_mat##n##_f_identity(dst); }                                      \
void lvn_mat##n##_copy(LvnMat##n##x##n src, LvnMat##n##x##n dst) { lvn_mat##n##_f_copy(src, dst); }                    \
void lvn_mat##n##_inv(LvnMat##n##x##n mat, LvnMat##n##x##n dst) { lvn_mat##n##_f_inv(mat, dst); }                      \
void lvn_mat##n##_transpose_to(LvnMat##n##x##n m, LvnMat##n##x##n dst) { lvn_mat##n##_f_transpose_to(m, dst); }        \
void lvn_mat##n##_transpose(LvnMat##n##x##n m) { lvn_mat##n##_f_transpose(m); }                                        \
void lvn_mat##n##_negate_to(LvnMat##n##x##n m, LvnMat##n##x##n dst) { lvn_mat##n##_f_negate_to(m, dst); }              \
void lvn_mat##n##_negate(LvnMat##n##x##n m) { lvn_mat##n##_f_negate(m); }                                              \
void lvn_mat##n##_add(LvnMat##n##x##n m1, LvnMat##n##x##n m2, LvnMat##n##x##n dst) { lvn_mat##n##_f_add(m1, m2, dst); }\
void lvn_mat##n##_sub(LvnMat##n##x##n m1, LvnMat##n##x##n m2, LvnMat##n##x##n dst) { lvn_mat##n##_f_sub(m1, m2, dst); }\
void lvn_mat##n##_mult(LvnMat##n##x##n m1, LvnMat##n##x##n m2, LvnMat##n##x##n dst) { lvn_mat##n##_f_mult(m1, m2, dst); }\
void lvn_mat##n##_multmv(LvnMat##n##x##n m, LvnVec##n v, LvnVec##n dst) { lvn_mat##n##_f_multmv(m, v, dst); }          \
void lvn_mat##n##_multvm(LvnVec##n v, LvnMat##n##x##n m, LvnVec##n dst) { lvn_mat##n##_f_multvm(v, m, dst); }          \
void lvn_mat##n##_addms(LvnMat##n##x##n m, float s, LvnMat##n##x##n dst) { lvn_mat##n##_f_addms(m, s, dst); }          \
void lvn_mat##n##_subms(LvnMat##n##x##n m, float s, LvnMat##n##x##n dst) { lvn_mat##n##_f_subms(m, s, dst); }          \
void lvn_mat##n##_multms(LvnMat##n##x##n m, float s, LvnMat##n##x##n dst) { lvn_mat##n##_f_multms(m, s, dst); }        \
void lvn_mat##n##_divms(LvnMat##n##x##n m, float s, LvnMat##n##x##n dst) { lvn_mat##n##_f_divms(m, s, dst); }          \
void lvn_mat##n##_addsm(float s, LvnMat##n##x##n m, LvnMat##n##x##n dst) { lvn_mat##n##_f_addsm(s, m, dst); }          \
void lvn_mat##n##_subsm(float s, LvnMat##n##x##n m, LvnMat##n##x##n dst) { lvn_mat##n##_f_subsm(s, m, dst); }          \
void lvn_mat##n##_multsm(float s, LvnMat##n##x##n m, LvnMat##n##x##n dst) { lvn_mat##n##_f_multsm(s, m, dst); }        \
void lvn_mat##n##_divsm(float s, LvnMat##n##x##n m, LvnMat##n##x##n dst) { lvn_mat##n##_f_divsm(s, m, dst); }

#define LVN_DEFINE_MAT_I32_TYPE_MATH_IMPL(n)                                                                           \
void lvn_mat##n##i(int32_t* v, LvnMat##n##x##n##i dst) { lvn_mat##n##_i32(v, dst); }                                   \
void lvn_mat##n##i_zero(LvnMat##n##x##n##i dst) { lvn_mat##n##_i32_zero(dst); }                                        \
void lvn_mat##n##i_identity(LvnMat##n##x##n##i dst) { lvn_mat##n##_i32_identity(dst); }                                \
void lvn_mat##n##i_copy(LvnMat##n##x##n##i src, LvnMat##n##x##n##i dst) { lvn_mat##n##_i32_copy(src, dst); }           \
void lvn_mat##n##i_inv(LvnMat##n##x##n##i mat, LvnMat##n##x##n##i dst) { lvn_mat##n##_i32_inv(mat, dst); }             \
void lvn_mat##n##i_transpose_to(LvnMat##n##x##n##i m, LvnMat##n##x##n##i dst) { lvn_mat##n##_i32_transpose_to(m, dst); }\
void lvn_mat##n##i_transpose(LvnMat##n##x##n##i m) { lvn_mat##n##_i32_transpose(m); }                                  \
void lvn_mat##n##i_negate_to(LvnMat##n##x##n##i m, LvnMat##n##x##n##i dst) { lvn_mat##n##_i32_negate_to(m, dst); }     \
void lvn_mat##n##i_negate(LvnMat##n##x##n##i m) { lvn_mat##n##_i32_negate(m); }                                        \
void lvn_mat##n##i_add(LvnMat##n##x##n##i m1, LvnMat##n##x##n##i m2, LvnMat##n##x##n##i dst) { lvn_mat##n##_i32_add(m1, m2, dst); }\
void lvn_mat##n##i_sub(LvnMat##n##x##n##i m1, LvnMat##n##x##n##i m2, LvnMat##n##x##n##i dst) { lvn_mat##n##_i32_sub(m1, m2, dst); }\
void lvn_mat##n##i_mult(LvnMat##n##x##n##i m1, LvnMat##n##x##n##i m2, LvnMat##n##x##n##i dst) { lvn_mat##n##_i32_mult(m1, m2, dst); }\
void lvn_mat##n##i_multmv(LvnMat##n##x##n##i m, LvnVec##n##i v, LvnVec##n##i dst) { lvn_mat##n##_i32_multmv(m, v, dst); }\
void lvn_mat##n##i_multvm(LvnVec##n##i v, LvnMat##n##x##n##i m, LvnVec##n##i dst) { lvn_mat##n##_i32_multvm(v, m, dst); }\
void lvn_mat##n##i_addms(LvnMat##n##x##n##i m, int32_t s, LvnMat##n##x##n##i dst) { lvn_mat##n##_i32_addms(m, s, dst); }\
void lvn_mat##n##i_subms(LvnMat##n##x##n##i m, int32_t s, LvnMat##n##x##n##i dst) { lvn_mat##n##_i32_subms(m, s, dst); }\
void lvn_mat##n##i_multms(LvnMat##n##x##n##i m, int32_t s, LvnMat##n##x##n##i dst) { lvn_mat##n##_i32_multms(m, s, dst); }\
void lvn_mat##n##i_divms(LvnMat##n##x##n##i m, int32_t s, LvnMat##n##x##n##i dst) { lvn_mat##n##_i32_divms(m, s, dst); }\
void lvn_mat##n##i_addsm(int32_t s, LvnMat##n##x##n##i m, LvnMat##n##x##n##i dst) { lvn_mat##n##_i32_addsm(s, m, dst); }\
void lvn_mat##n##i_subsm(int32_t s, LvnMat##n##x##n##i m, LvnMat##n##x##n##i dst) { lvn_mat##n##_i32_subsm(s, m, dst); }\
void lvn_mat##n##i_multsm(int32_t s, LvnMat##n##x##n##i m, LvnMat##n##x##n##i dst) { lvn_mat##n##_i32_multsm(s, m, dst); }\
void lvn_mat##n##i_divsm(int32_t s, LvnMat##n##x##n##i m, LvnMat##n##x##n##i dst) { lvn_mat##n##_i32_divsm(s, m, dst); }

#define LVN_DEFINE_MAT_UI32_TYPE_MATH_IMPL(n)                                                                          \
void lvn_mat##n##ui(uint32_t* v, LvnMat##n##x##n##ui dst) { lvn_mat##n##_ui32(v, dst); }                               \
void lvn_mat##n##ui_zero(LvnMat##n##x##n##ui dst) { lvn_mat##n##_ui32_zero(dst); }                                     \
void lvn_mat##n##ui_identity(LvnMat##n##x##n##ui dst) { lvn_mat##n##_ui32_identity(dst); }                             \
void lvn_mat##n##ui_copy(LvnMat##n##x##n##ui src, LvnMat##n##x##n##ui dst) { lvn_mat##n##_ui32_copy(src, src); }       \
void lvn_mat##n##ui_inv(LvnMat##n##x##n##ui mat, LvnMat##n##x##n##ui dst) { lvn_mat##n##_ui32_inv(mat, dst); }         \
void lvn_mat##n##ui_transpose_to(LvnMat##n##x##n##ui m, LvnMat##n##x##n##ui dst) { lvn_mat##n##_ui32_transpose_to(m, dst); }\
void lvn_mat##n##ui_transpose(LvnMat##n##x##n##ui m) { lvn_mat##n##_ui32_transpose(m); }                               \
void lvn_mat##n##ui_negate_to(LvnMat##n##x##n##ui m, LvnMat##n##x##n##ui dst) { lvn_mat##n##_ui32_negate_to(m, dst); } \
void lvn_mat##n##ui_negate(LvnMat##n##x##n##ui m) { lvn_mat##n##_ui32_negate(m); }                                     \
void lvn_mat##n##ui_add(LvnMat##n##x##n##ui m1, LvnMat##n##x##n##ui m2, LvnMat##n##x##n##ui dst) { lvn_mat##n##_ui32_add(m1, m2, dst); }\
void lvn_mat##n##ui_sub(LvnMat##n##x##n##ui m1, LvnMat##n##x##n##ui m2, LvnMat##n##x##n##ui dst) { lvn_mat##n##_ui32_sub(m1, m2, dst); }\
void lvn_mat##n##ui_mult(LvnMat##n##x##n##ui m1, LvnMat##n##x##n##ui m2, LvnMat##n##x##n##ui dst) { lvn_mat##n##_ui32_mult(m1, m2, dst); }\
void lvn_mat##n##ui_multmv(LvnMat##n##x##n##ui m, LvnVec##n##ui v, LvnVec##n##ui dst) { lvn_mat##n##_ui32_multmv(m, v, dst); }\
void lvn_mat##n##ui_multvm(LvnVec##n##ui v, LvnMat##n##x##n##ui m, LvnVec##n##ui dst) { lvn_mat##n##_ui32_multvm(v, m, dst); }\
void lvn_mat##n##ui_addms(LvnMat##n##x##n##ui m, uint32_t s, LvnMat##n##x##n##ui dst) { lvn_mat##n##_ui32_addms(m, s, dst); }\
void lvn_mat##n##ui_subms(LvnMat##n##x##n##ui m, uint32_t s, LvnMat##n##x##n##ui dst) { lvn_mat##n##_ui32_subms(m, s, dst); }\
void lvn_mat##n##ui_multms(LvnMat##n##x##n##ui m, uint32_t s, LvnMat##n##x##n##ui dst) { lvn_mat##n##_ui32_multms(m, s, dst); }\
void lvn_mat##n##ui_divms(LvnMat##n##x##n##ui m, uint32_t s, LvnMat##n##x##n##ui dst) { lvn_mat##n##_ui32_divms(m, s, dst); }\
void lvn_mat##n##ui_addsm(uint32_t s, LvnMat##n##x##n##ui m, LvnMat##n##x##n##ui dst) { lvn_mat##n##_ui32_addsm(s, m, dst); }\
void lvn_mat##n##ui_subsm(uint32_t s, LvnMat##n##x##n##ui m, LvnMat##n##x##n##ui dst) { lvn_mat##n##_ui32_subsm(s, m, dst); }\
void lvn_mat##n##ui_multsm(uint32_t s, LvnMat##n##x##n##ui m, LvnMat##n##x##n##ui dst) { lvn_mat##n##_ui32_multsm(s, m, dst); }\
void lvn_mat##n##ui_divsm(uint32_t s, LvnMat##n##x##n##ui m, LvnMat##n##x##n##ui dst) { lvn_mat##n##_ui32_divsm(s, m, dst); }

#ifdef LVN_GMATH_INCLUDE_VEC2_F
LVN_DEFINE_VEC2_TYPE_MATH_DECL(f)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC2_D
LVN_DEFINE_VEC2_TYPE_MATH_DECL(d)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC2_I8
LVN_DEFINE_VEC2_TYPE_MATH_DECL(i8)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC2_UI8
LVN_DEFINE_VEC2_TYPE_MATH_DECL(ui8)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC2_I16
LVN_DEFINE_VEC2_TYPE_MATH_DECL(i16)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC2_UI16
LVN_DEFINE_VEC2_TYPE_MATH_DECL(ui16)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC2_I32
LVN_DEFINE_VEC2_TYPE_MATH_DECL(i32)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC2_UI32
LVN_DEFINE_VEC2_TYPE_MATH_DECL(ui32)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC2_I64
LVN_DEFINE_VEC2_TYPE_MATH_DECL(i64)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC2_UI64
LVN_DEFINE_VEC2_TYPE_MATH_DECL(ui64)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC3_F
LVN_DEFINE_VEC3_TYPE_MATH_DECL(f)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC3_D
LVN_DEFINE_VEC3_TYPE_MATH_DECL(d)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC3_I8
LVN_DEFINE_VEC3_TYPE_MATH_DECL(i8)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC3_UI8
LVN_DEFINE_VEC3_TYPE_MATH_DECL(ui8)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC3_I16
LVN_DEFINE_VEC3_TYPE_MATH_DECL(i16)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC3_UI16
LVN_DEFINE_VEC3_TYPE_MATH_DECL(ui16)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC3_I32
LVN_DEFINE_VEC3_TYPE_MATH_DECL(i32)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC3_UI32
LVN_DEFINE_VEC3_TYPE_MATH_DECL(ui32)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC3_I64
LVN_DEFINE_VEC3_TYPE_MATH_DECL(i64)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC3_UI64
LVN_DEFINE_VEC3_TYPE_MATH_DECL(ui64)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC4_F
LVN_DEFINE_VEC4_TYPE_MATH_DECL(f)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC4_D
LVN_DEFINE_VEC4_TYPE_MATH_DECL(d)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC4_I8
LVN_DEFINE_VEC4_TYPE_MATH_DECL(i8)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC4_UI8
LVN_DEFINE_VEC4_TYPE_MATH_DECL(ui8)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC4_I16
LVN_DEFINE_VEC4_TYPE_MATH_DECL(i16)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC4_UI16
LVN_DEFINE_VEC4_TYPE_MATH_DECL(ui16)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC4_I32
LVN_DEFINE_VEC4_TYPE_MATH_DECL(i32)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC4_UI32
LVN_DEFINE_VEC4_TYPE_MATH_DECL(ui32)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC4_I64
LVN_DEFINE_VEC4_TYPE_MATH_DECL(i64)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC4_UI64
LVN_DEFINE_VEC4_TYPE_MATH_DECL(ui64)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT2_F
LVN_DEFINE_MAT2_TYPE_MATH_DECL(f)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT2_D
LVN_DEFINE_MAT2_TYPE_MATH_DECL(d)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT2_I8
LVN_DEFINE_MAT2_TYPE_MATH_DECL(i8)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT2_UI8
LVN_DEFINE_MAT2_TYPE_MATH_DECL(ui8)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT2_I16
LVN_DEFINE_MAT2_TYPE_MATH_DECL(i16)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT2_UI16
LVN_DEFINE_MAT2_TYPE_MATH_DECL(ui16)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT2_I32
LVN_DEFINE_MAT2_TYPE_MATH_DECL(i32)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT2_UI32
LVN_DEFINE_MAT2_TYPE_MATH_DECL(ui32)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT2_I64
LVN_DEFINE_MAT2_TYPE_MATH_DECL(i64)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT2_UI64
LVN_DEFINE_MAT2_TYPE_MATH_DECL(ui64)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT3_F
LVN_DEFINE_MAT3_TYPE_MATH_DECL(f)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT3_D
LVN_DEFINE_MAT3_TYPE_MATH_DECL(d)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT3_I8
LVN_DEFINE_MAT3_TYPE_MATH_DECL(i8)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT3_UI8
LVN_DEFINE_MAT3_TYPE_MATH_DECL(ui8)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT3_I16
LVN_DEFINE_MAT3_TYPE_MATH_DECL(i16)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT3_UI16
LVN_DEFINE_MAT3_TYPE_MATH_DECL(ui16)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT3_I32
LVN_DEFINE_MAT3_TYPE_MATH_DECL(i32)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT3_UI32
LVN_DEFINE_MAT3_TYPE_MATH_DECL(ui32)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT3_I64
LVN_DEFINE_MAT3_TYPE_MATH_DECL(i64)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT3_UI64
LVN_DEFINE_MAT3_TYPE_MATH_DECL(ui64)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT4_F
LVN_DEFINE_MAT4_TYPE_MATH_DECL(f)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT4_D
LVN_DEFINE_MAT4_TYPE_MATH_DECL(d)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT4_I8
LVN_DEFINE_MAT4_TYPE_MATH_DECL(i8)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT4_UI8
LVN_DEFINE_MAT4_TYPE_MATH_DECL(ui8)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT4_I16
LVN_DEFINE_MAT4_TYPE_MATH_DECL(i16)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT4_UI16
LVN_DEFINE_MAT4_TYPE_MATH_DECL(ui16)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT4_I32
LVN_DEFINE_MAT4_TYPE_MATH_DECL(i32)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT4_UI32
LVN_DEFINE_MAT4_TYPE_MATH_DECL(ui32)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT4_I64
LVN_DEFINE_MAT4_TYPE_MATH_DECL(i64)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT4_UI64
LVN_DEFINE_MAT4_TYPE_MATH_DECL(ui64)
#endif


#ifdef LVN_GMATH_INCLUDE_VEC2_F
LVN_DEFINE_VEC_F_TYPE_MATH_DECL(2)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC2_I32
LVN_DEFINE_VEC_I32_TYPE_MATH_DECL(2)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC2_UI32
LVN_DEFINE_VEC_UI32_TYPE_MATH_DECL(2)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC3_F
LVN_DEFINE_VEC_F_TYPE_MATH_DECL(3)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC3_I32
LVN_DEFINE_VEC_I32_TYPE_MATH_DECL(3)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC3_UI32
LVN_DEFINE_VEC_UI32_TYPE_MATH_DECL(3)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC4_F
LVN_DEFINE_VEC_F_TYPE_MATH_DECL(4)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC4_I32
LVN_DEFINE_VEC_I32_TYPE_MATH_DECL(4)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC4_UI32
LVN_DEFINE_VEC_UI32_TYPE_MATH_DECL(4)
#endif

#ifdef LVN_GMATH_INCLUDE_MAT2_F
LVN_DEFINE_MAT_F_TYPE_MATH_DECL(2)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT2_I32
LVN_DEFINE_MAT_I32_TYPE_MATH_DECL(2)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT2_UI32
LVN_DEFINE_MAT_UI32_TYPE_MATH_DECL(2)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT3_F
LVN_DEFINE_MAT_F_TYPE_MATH_DECL(3)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT3_I32
LVN_DEFINE_MAT_I32_TYPE_MATH_DECL(3)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT3_UI32
LVN_DEFINE_MAT_UI32_TYPE_MATH_DECL(3)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT4_F
LVN_DEFINE_MAT_F_TYPE_MATH_DECL(4)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT4_I32
LVN_DEFINE_MAT_I32_TYPE_MATH_DECL(4)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT4_UI32
LVN_DEFINE_MAT_UI32_TYPE_MATH_DECL(4)
#endif

// TODO: remove this later
#define LVN_GMATH_IMPL


#ifdef LVN_GMATH_IMPL

#include <math.h>

#ifdef LVN_GMATH_INCLUDE_VEC2_F
LVN_DEFINE_VEC2_TYPE_MATH_IMPL(f)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC2_D
LVN_DEFINE_VEC2_TYPE_MATH_IMPL(d)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC2_I8
LVN_DEFINE_VEC2_TYPE_MATH_IMPL(i8)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC2_UI8
LVN_DEFINE_VEC2_TYPE_MATH_IMPL(ui8)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC2_I16
LVN_DEFINE_VEC2_TYPE_MATH_IMPL(i16)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC2_UI16
LVN_DEFINE_VEC2_TYPE_MATH_IMPL(ui16)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC2_I32
LVN_DEFINE_VEC2_TYPE_MATH_IMPL(i32)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC2_UI32
LVN_DEFINE_VEC2_TYPE_MATH_IMPL(ui32)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC2_I64
LVN_DEFINE_VEC2_TYPE_MATH_IMPL(i64)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC2_UI64
LVN_DEFINE_VEC2_TYPE_MATH_IMPL(ui64)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC3_F
LVN_DEFINE_VEC3_TYPE_MATH_IMPL(f)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC3_D
LVN_DEFINE_VEC3_TYPE_MATH_IMPL(d)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC3_I8
LVN_DEFINE_VEC3_TYPE_MATH_IMPL(i8)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC3_UI8
LVN_DEFINE_VEC3_TYPE_MATH_IMPL(ui8)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC3_I16
LVN_DEFINE_VEC3_TYPE_MATH_IMPL(i16)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC3_UI16
LVN_DEFINE_VEC3_TYPE_MATH_IMPL(ui16)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC3_I32
LVN_DEFINE_VEC3_TYPE_MATH_IMPL(i32)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC3_UI32
LVN_DEFINE_VEC3_TYPE_MATH_IMPL(ui32)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC3_I64
LVN_DEFINE_VEC3_TYPE_MATH_IMPL(i64)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC3_UI64
LVN_DEFINE_VEC3_TYPE_MATH_IMPL(ui64)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC4_F
LVN_DEFINE_VEC4_TYPE_MATH_IMPL(f)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC4_D
LVN_DEFINE_VEC4_TYPE_MATH_IMPL(d)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC4_I8
LVN_DEFINE_VEC4_TYPE_MATH_IMPL(i8)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC4_UI8
LVN_DEFINE_VEC4_TYPE_MATH_IMPL(ui8)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC4_I16
LVN_DEFINE_VEC4_TYPE_MATH_IMPL(i16)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC4_UI16
LVN_DEFINE_VEC4_TYPE_MATH_IMPL(ui16)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC4_I32
LVN_DEFINE_VEC4_TYPE_MATH_IMPL(i32)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC4_UI32
LVN_DEFINE_VEC4_TYPE_MATH_IMPL(ui32)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC4_I64
LVN_DEFINE_VEC4_TYPE_MATH_IMPL(i64)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC4_UI64
LVN_DEFINE_VEC4_TYPE_MATH_IMPL(ui64)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT2_F
LVN_DEFINE_MAT2_TYPE_MATH_IMPL(f)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT2_D
LVN_DEFINE_MAT2_TYPE_MATH_IMPL(d)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT2_I8
LVN_DEFINE_MAT2_TYPE_MATH_IMPL(i8)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT2_UI8
LVN_DEFINE_MAT2_TYPE_MATH_IMPL(ui8)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT2_I16
LVN_DEFINE_MAT2_TYPE_MATH_IMPL(i16)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT2_UI16
LVN_DEFINE_MAT2_TYPE_MATH_IMPL(ui16)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT2_I32
LVN_DEFINE_MAT2_TYPE_MATH_IMPL(i32)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT2_UI32
LVN_DEFINE_MAT2_TYPE_MATH_IMPL(ui32)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT2_I64
LVN_DEFINE_MAT2_TYPE_MATH_IMPL(i64)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT2_UI64
LVN_DEFINE_MAT2_TYPE_MATH_IMPL(ui64)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT3_F
LVN_DEFINE_MAT3_TYPE_MATH_IMPL(f)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT3_D
LVN_DEFINE_MAT3_TYPE_MATH_IMPL(d)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT3_I8
LVN_DEFINE_MAT3_TYPE_MATH_IMPL(i8)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT3_UI8
LVN_DEFINE_MAT3_TYPE_MATH_IMPL(ui8)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT3_I16
LVN_DEFINE_MAT3_TYPE_MATH_IMPL(i16)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT3_UI16
LVN_DEFINE_MAT3_TYPE_MATH_IMPL(ui16)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT3_I32
LVN_DEFINE_MAT3_TYPE_MATH_IMPL(i32)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT3_UI32
LVN_DEFINE_MAT3_TYPE_MATH_IMPL(ui32)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT3_I64
LVN_DEFINE_MAT3_TYPE_MATH_IMPL(i64)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT3_UI64
LVN_DEFINE_MAT3_TYPE_MATH_IMPL(ui64)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT4_F
LVN_DEFINE_MAT4_TYPE_MATH_IMPL(f)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT4_D
LVN_DEFINE_MAT4_TYPE_MATH_IMPL(d)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT4_I8
LVN_DEFINE_MAT4_TYPE_MATH_IMPL(i8)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT4_UI8
LVN_DEFINE_MAT4_TYPE_MATH_IMPL(ui8)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT4_I16
LVN_DEFINE_MAT4_TYPE_MATH_IMPL(i16)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT4_UI16
LVN_DEFINE_MAT4_TYPE_MATH_IMPL(ui16)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT4_I32
LVN_DEFINE_MAT4_TYPE_MATH_IMPL(i32)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT4_UI32
LVN_DEFINE_MAT4_TYPE_MATH_IMPL(ui32)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT4_I64
LVN_DEFINE_MAT4_TYPE_MATH_IMPL(i64)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT4_UI64
LVN_DEFINE_MAT4_TYPE_MATH_IMPL(ui64)
#endif

#ifdef LVN_GMATH_INCLUDE_VEC2_F
LVN_DEFINE_VEC_F_TYPE_MATH_IMPL(2)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC2_I32
LVN_DEFINE_VEC_I32_TYPE_MATH_IMPL(2)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC2_UI32
LVN_DEFINE_VEC_UI32_TYPE_MATH_IMPL(2)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC3_F
LVN_DEFINE_VEC_F_TYPE_MATH_IMPL(3)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC3_I32
LVN_DEFINE_VEC_I32_TYPE_MATH_IMPL(3)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC3_UI32
LVN_DEFINE_VEC_UI32_TYPE_MATH_IMPL(3)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC4_F
LVN_DEFINE_VEC_F_TYPE_MATH_IMPL(4)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC4_I32
LVN_DEFINE_VEC_I32_TYPE_MATH_IMPL(4)
#endif
#ifdef LVN_GMATH_INCLUDE_VEC4_UI32
LVN_DEFINE_VEC_UI32_TYPE_MATH_IMPL(4)
#endif

#ifdef LVN_GMATH_INCLUDE_MAT2_F
LVN_DEFINE_MAT_F_TYPE_MATH_IMPL(2)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT2_I32
LVN_DEFINE_MAT_I32_TYPE_MATH_IMPL(2)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT2_UI32
LVN_DEFINE_MAT_UI32_TYPE_MATH_IMPL(2)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT3_F
LVN_DEFINE_MAT_F_TYPE_MATH_IMPL(3)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT3_I32
LVN_DEFINE_MAT_I32_TYPE_MATH_IMPL(3)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT3_UI32
LVN_DEFINE_MAT_UI32_TYPE_MATH_IMPL(3)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT4_F
LVN_DEFINE_MAT_F_TYPE_MATH_IMPL(4)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT4_I32
LVN_DEFINE_MAT_I32_TYPE_MATH_IMPL(4)
#endif
#ifdef LVN_GMATH_INCLUDE_MAT4_UI32
LVN_DEFINE_MAT_UI32_TYPE_MATH_IMPL(4)
#endif

#endif // LVN_GMATH_IMPL


#endif // !HG_LVN_GMATH_H
