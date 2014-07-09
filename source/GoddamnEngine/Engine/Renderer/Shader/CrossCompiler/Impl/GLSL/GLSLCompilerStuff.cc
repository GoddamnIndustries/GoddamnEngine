/// ==========================================================================================
/// GLSLCompiler.cc: Some GLSLang suff.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 14.06.2014 - Created by James Jhuighuy.
/// ==========================================================================================

// We have Warning 4 level, but not glslang.
#if (defined(_MSC_VER))
#	pragma warning(push, 0)
#endif	// if (defined(_MSC_VER))

//
//Copyright (C) 2002-2005 3Dlabs Inc. Ltd.
//Copyright (C) 2012-2013 LunarG, Inc.
//
//All rights reserved.
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions
//are met:
//
// Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following
// disclaimer in the documentation and/or other materials provided
// with the distribution.
//
// Neither the name of 3Dlabs Inc. Ltd. nor the names of its
// contributors may be used to endorse or promote products derived
// from this software without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
//FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
//COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
//INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
//BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
//CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
//LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
//ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//POSSIBILITY OF SUCH DAMAGE.
//

/// ~~~~~~~~~~~~~~~
/// CodeGen.cpp
/// ~~~~~~~~~~~~~~~

#include <Include/Common.h>
#include <Include/ShHandle.h>
#include <MachineIndependent/Versions.h>

class TGenericCompiler : public TCompiler {
public:
	TGenericCompiler(EShLanguage l, int dOptions) : TCompiler(l, infoSink), debugOptions(dOptions) { }
	virtual bool compile(TIntermNode* root, int version = 0, EProfile profile = ENoProfile);
	TInfoSink infoSink;
	int debugOptions;
};

//
// This function must be provided to create the actual
// compile object used by higher level code.  It returns
// a subclass of TCompiler.
//
TCompiler* ConstructCompiler(EShLanguage language, int debugOptions)
{
	return new TGenericCompiler(language, debugOptions);
}

//
// Delete the compiler made by ConstructCompiler
//
void DeleteCompiler(TCompiler* compiler)
{
	delete compiler;
}

//
//  Generate code from the given parse tree
//
bool TGenericCompiler::compile(TIntermNode *root, int version, EProfile profile)
{
	haveValidObjectCode = true;

	return haveValidObjectCode;
}

/// ~~~~~~~~~~~~~~~
/// Link.cpp
/// ~~~~~~~~~~~~~~~

#include <Include/Common.h>
#include <Include/ShHandle.h>

//
// Actual link object, derived from the shader handle base classes.
//
class TGenericLinker : public TLinker {
public:
	TGenericLinker(EShExecutable e, int dOptions) : TLinker(e, infoSink), debugOptions(dOptions) { }
	bool link(TCompilerList&, TUniformMap*) { return true; }
	void getAttributeBindings(ShBindingTable const **t) const { }
	TInfoSink infoSink;
	int debugOptions;
};

//
// The internal view of a uniform/float object exchanged with the driver.
//
class TUniformLinkedMap : public TUniformMap {
public:
	TUniformLinkedMap() { }
	virtual int getLocation(const char* name) { return 0; }
};

TShHandleBase* ConstructLinker(EShExecutable executable, int debugOptions)
{
	return new TGenericLinker(executable, debugOptions);
}

void DeleteLinker(TShHandleBase* linker)
{
	delete linker;
}

TUniformMap* ConstructUniformMap()
{
	return new TUniformLinkedMap();
}

void DeleteUniformMap(TUniformMap* map)
{
	delete map;
}

TShHandleBase* ConstructBindings()
{
	return 0;
}

void DeleteBindingList(TShHandleBase* bindingList)
{
	delete bindingList;
}

#if (defined(_MSC_VER))
#	pragma warning(pop)
#endif	// if (defined(_MSC_VER))
