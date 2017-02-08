// ==========================================================================================
// Copyright (C) Goddamn Industries 2016. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*!
 * @file GoddamnEngine/Core/Templates/VaryingArgsMacro.h
 * Varying args macros.
 */
#pragma once

#include <GoddamnEngine/Include.h>

/*! Used to count arguments in the macros with varying arguments. */
#define GD_VA_ARGS_LENGTH_(N100 \
		, N99, N98, N97, N96, N95, N94, N93, N92, N91, N90, N89, N88, N87, N86, N85, N84, N83, N82 \
		, N81, N80, N79, N78, N77, N76, N75, N74, N73, N72, N71, N70, N69, N68, N67, N66, N65, N64 \
		, N63, N62, N61, N60, N59, N58, N57, N56, N55, N54, N53, N52, N51, N50, N49, N48, N47, N46 \
		, N45, N44, N43, N42, N41, N40, N39, N38, N37, N36, N35, N34, N33, N32, N31, N30, N29, N28 \
		, N27, N26, N25, N24, N23, N22, N21, N20, N19, N18, N17, N16, N15, N14, N13, N12, N11, N10 \
		, N9, N8, N7, N6, N5, N4, N3, N2, N1, N, ...) N
#define GD_VA_ARGS_LENGTH(...) GD_PASS(GD_VA_ARGS_LENGTH_(__VA_ARGS__, 100 \
		, 99, 98, 97, 96, 95, 94, 93, 92, 91, 90, 89, 88, 87, 86, 85, 84, 83, 82, 81, 80, 79, 78 \
		, 77, 76, 75, 74, 73, 72, 71, 70, 69, 68, 67, 66, 65, 64, 63, 62, 61, 60, 59, 58, 57, 56 \
		, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34 \
		, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12 \
		, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1))

/*! Used for iterations for recursive code generation in the macros with varying arguments. */
#define GD_VA_ARGS_ITER_IMPL1(Macro, Data, Arg) \
		Macro(Data, 0, Arg)

#define GD_VA_ARGS_ITER_IMPL2(Macro, Data, Arg, ...) \
		Macro(Data, 1, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL1(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL3(Macro, Data, Arg, ...) \
		Macro(Data, 2, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL2(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL4(Macro, Data, Arg, ...) \
		Macro(Data, 3, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL3(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL5(Macro, Data, Arg, ...) \
		Macro(Data, 4, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL4(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL6(Macro, Data, Arg, ...) \
		Macro(Data, 5, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL5(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL7(Macro, Data, Arg, ...) \
		Macro(Data, 6, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL6(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL8(Macro, Data, Arg, ...) \
		Macro(Data, 7, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL7(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL9(Macro, Data, Arg, ...) \
		Macro(Data, 8, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL8(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL10(Macro, Data, Arg, ...) \
		Macro(Data, 9, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL9(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL11(Macro, Data, Arg, ...) \
		Macro(Data, 10, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL10(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL12(Macro, Data, Arg, ...) \
		Macro(Data, 11, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL11(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL13(Macro, Data, Arg, ...) \
		Macro(Data, 12, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL12(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL14(Macro, Data, Arg, ...) \
		Macro(Data, 13, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL13(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL15(Macro, Data, Arg, ...) \
		Macro(Data, 14, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL14(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL16(Macro, Data, Arg, ...) \
		Macro(Data, 15, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL15(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL17(Macro, Data, Arg, ...) \
		Macro(Data, 16, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL16(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL18(Macro, Data, Arg, ...) \
		Macro(Data, 17, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL17(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL19(Macro, Data, Arg, ...) \
		Macro(Data, 18, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL18(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL20(Macro, Data, Arg, ...) \
		Macro(Data, 19, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL19(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL21(Macro, Data, Arg, ...) \
		Macro(Data, 20, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL20(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL22(Macro, Data, Arg, ...) \
		Macro(Data, 21, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL21(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL23(Macro, Data, Arg, ...) \
		Macro(Data, 22, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL22(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL24(Macro, Data, Arg, ...) \
		Macro(Data, 23, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL23(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL25(Macro, Data, Arg, ...) \
		Macro(Data, 24, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL24(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL26(Macro, Data, Arg, ...) \
		Macro(Data, 25, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL25(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL27(Macro, Data, Arg, ...) \
		Macro(Data, 26, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL26(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL28(Macro, Data, Arg, ...) \
		Macro(Data, 27, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL27(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL29(Macro, Data, Arg, ...) \
		Macro(Data, 28, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL28(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL30(Macro, Data, Arg, ...) \
		Macro(Data, 29, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL29(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL31(Macro, Data, Arg, ...) \
		Macro(Data, 30, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL30(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL32(Macro, Data, Arg, ...) \
		Macro(Data, 31, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL31(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL33(Macro, Data, Arg, ...) \
		Macro(Data, 32, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL32(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL34(Macro, Data, Arg, ...) \
		Macro(Data, 33, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL33(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL35(Macro, Data, Arg, ...) \
		Macro(Data, 34, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL34(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL36(Macro, Data, Arg, ...) \
		Macro(Data, 35, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL35(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL37(Macro, Data, Arg, ...) \
		Macro(Data, 36, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL36(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL38(Macro, Data, Arg, ...) \
		Macro(Data, 37, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL37(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL39(Macro, Data, Arg, ...) \
		Macro(Data, 38, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL38(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL40(Macro, Data, Arg, ...) \
		Macro(Data, 39, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL39(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL41(Macro, Data, Arg, ...) \
		Macro(Data, 40, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL40(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL42(Macro, Data, Arg, ...) \
		Macro(Data, 41, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL41(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL43(Macro, Data, Arg, ...) \
		Macro(Data, 42, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL42(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL44(Macro, Data, Arg, ...) \
		Macro(Data, 43, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL43(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL45(Macro, Data, Arg, ...) \
		Macro(Data, 44, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL44(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL46(Macro, Data, Arg, ...) \
		Macro(Data, 45, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL45(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL47(Macro, Data, Arg, ...) \
		Macro(Data, 46, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL46(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL48(Macro, Data, Arg, ...) \
		Macro(Data, 47, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL47(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL49(Macro, Data, Arg, ...) \
		Macro(Data, 48, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL48(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL50(Macro, Data, Arg, ...) \
		Macro(Data, 49, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL49(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL51(Macro, Data, Arg, ...) \
		Macro(Data, 50, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL50(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL52(Macro, Data, Arg, ...) \
		Macro(Data, 51, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL51(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL53(Macro, Data, Arg, ...) \
		Macro(Data, 52, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL52(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL54(Macro, Data, Arg, ...) \
		Macro(Data, 53, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL53(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL55(Macro, Data, Arg, ...) \
		Macro(Data, 54, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL54(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL56(Macro, Data, Arg, ...) \
		Macro(Data, 55, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL55(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL57(Macro, Data, Arg, ...) \
		Macro(Data, 56, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL56(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL58(Macro, Data, Arg, ...) \
		Macro(Data, 57, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL57(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL59(Macro, Data, Arg, ...) \
		Macro(Data, 58, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL58(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL60(Macro, Data, Arg, ...) \
		Macro(Data, 59, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL59(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL61(Macro, Data, Arg, ...) \
		Macro(Data, 60, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL60(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL62(Macro, Data, Arg, ...) \
		Macro(Data, 61, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL61(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL63(Macro, Data, Arg, ...) \
		Macro(Data, 62, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL62(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL64(Macro, Data, Arg, ...) \
		Macro(Data, 63, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL63(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL65(Macro, Data, Arg, ...) \
		Macro(Data, 64, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL64(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL66(Macro, Data, Arg, ...) \
		Macro(Data, 65, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL65(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL67(Macro, Data, Arg, ...) \
		Macro(Data, 66, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL66(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL68(Macro, Data, Arg, ...) \
		Macro(Data, 67, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL67(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL69(Macro, Data, Arg, ...) \
		Macro(Data, 68, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL68(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL70(Macro, Data, Arg, ...) \
		Macro(Data, 69, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL69(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL71(Macro, Data, Arg, ...) \
		Macro(Data, 70, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL70(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL72(Macro, Data, Arg, ...) \
		Macro(Data, 71, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL71(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL73(Macro, Data, Arg, ...) \
		Macro(Data, 72, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL72(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL74(Macro, Data, Arg, ...) \
		Macro(Data, 73, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL73(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL75(Macro, Data, Arg, ...) \
		Macro(Data, 74, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL74(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL76(Macro, Data, Arg, ...) \
		Macro(Data, 75, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL75(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL77(Macro, Data, Arg, ...) \
		Macro(Data, 76, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL76(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL78(Macro, Data, Arg, ...) \
		Macro(Data, 77, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL77(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL79(Macro, Data, Arg, ...) \
		Macro(Data, 78, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL78(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL80(Macro, Data, Arg, ...) \
		Macro(Data, 79, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL79(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL81(Macro, Data, Arg, ...) \
		Macro(Data, 80, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL80(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL82(Macro, Data, Arg, ...) \
		Macro(Data, 81, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL81(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL83(Macro, Data, Arg, ...) \
		Macro(Data, 82, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL82(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL84(Macro, Data, Arg, ...) \
		Macro(Data, 83, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL83(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL85(Macro, Data, Arg, ...) \
		Macro(Data, 84, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL84(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL86(Macro, Data, Arg, ...) \
		Macro(Data, 85, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL85(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL87(Macro, Data, Arg, ...) \
		Macro(Data, 86, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL86(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL88(Macro, Data, Arg, ...) \
		Macro(Data, 87, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL87(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL89(Macro, Data, Arg, ...) \
		Macro(Data, 88, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL88(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL90(Macro, Data, Arg, ...) \
		Macro(Data, 89, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL89(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL91(Macro, Data, Arg, ...) \
		Macro(Data, 90, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL90(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL92(Macro, Data, Arg, ...) \
		Macro(Data, 91, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL91(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL93(Macro, Data, Arg, ...) \
		Macro(Data, 92, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL92(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL94(Macro, Data, Arg, ...) \
		Macro(Data, 93, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL93(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL95(Macro, Data, Arg, ...) \
		Macro(Data, 94, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL94(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL96(Macro, Data, Arg, ...) \
		Macro(Data, 95, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL95(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL97(Macro, Data, Arg, ...) \
		Macro(Data, 96, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL96(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL98(Macro, Data, Arg, ...) \
		Macro(Data, 97, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL97(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL99(Macro, Data, Arg, ...) \
		Macro(Data, 98, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL98(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL100(Macro, Data, Arg, ...) \
		Macro(Data, 99, Arg) \
		GD_PASS(GD_VA_ARGS_ITER_IMPL99(Macro, Data, __VA_ARGS__))

#define GD_VA_ARGS_ITER_IMPL_APPLY(Macro, ...) GD_PASS(Macro(__VA_ARGS__))
#define GD_VA_ARGS_ITER_IMPL_SELECT(Count) GD_VA_ARGS_ITER_IMPL ## Count

/*!
 * Use this low-level macro to generate recursive sequences for macros with varying arguments.
 *
 * @param Macro Name of the macro to substitute.
 * @param Data Additional data to pass.
 */
#define GD_VA_ARGS_ITER(Macro, Data, ...) \
		GD_PASS( \
			GD_VA_ARGS_ITER_IMPL_APPLY( \
				GD_VA_ARGS_ITER_IMPL_SELECT, \
				GD_VA_ARGS_LENGTH(__VA_ARGS__) \
				) \
			(Macro, Data, __VA_ARGS__) \
			)
