/// ==========================================================================================
/// AssemblyInfo.cs - OpenGL utility source.
/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.
/// 
/// History:
///		* 28.07.2014 - Created by James Jhuighuy
/// ==========================================================================================

using System;
using System.IO;

namespace GoddamnEngine.OpenGLUtility
{
    internal abstract class OpenGLHeader
    {
        public readonly static string SDKLocation = Environment.GetEnvironmentVariable("GODDAMN_SDK");
        public readonly static string RendererLocation = Path.Combine(SDKLocation, "source", "GoddamnEngine", "Engine", "Renderer", "Impl", "OpenGL");
        public readonly static string ESAPILocation = Path.Combine(RendererLocation, "OpenGL_API");
        
        protected readonly string HeaderLocation;
        protected OpenGLHeader(string HeaderFileName)
        {
            this.HeaderLocation = Path.Combine(ESAPILocation, HeaderFileName);
        }

        public virtual void Parse(StreamWriter GeneratedMethodsWriter, StreamWriter GeneratedDefinesWriter)
        {
            using (StreamReader HeaderReader = new StreamReader(this.HeaderLocation)) {
                GeneratedDefinesWriter.WriteLine();
                GeneratedDefinesWriter.WriteLine(string.Format("// Defininitions coming from {0}.h", Path.GetFileNameWithoutExtension(this.HeaderLocation)));
                for (string Line = null; (Line = HeaderReader.ReadLine()) != null; ) {
                    if (Line.Trim().StartsWith("#define GL")) {
                        // Macro definition here.
                        GeneratedDefinesWriter.WriteLine(Line);
                    } else if (Line.Trim().StartsWith("GL")) {
                        // API function definition here.
                        Line = Line.Replace(" * ", " *");
                        int ParamsBegin = Line.IndexOf('(');
                        string MethodDefinition = Line.Substring(0, ParamsBegin - 1);
                        string[] MethodDefinitionLexems = MethodDefinition.Split(' ', '\t');
                        string MethodDefinitionTypeName = null;
                        string MethodDefinitionType = MethodDefinitionLexems[1];
                        if (MethodDefinitionType == "const") {
                            MethodDefinitionType = MethodDefinitionLexems[2] + " const*";
                            MethodDefinitionTypeName = MethodDefinitionLexems[4];
                        } else {
                            MethodDefinitionTypeName = MethodDefinitionLexems[3];
                        }

                        string ParamsDefinition = Line.Substring(ParamsBegin + 1, Line.Length - ParamsBegin - ");".Length - 1);
                        string[] ParamsDefinitionParams = ParamsDefinition.Split(',');

                        GeneratedMethodsWriter.WriteLine();
                        GeneratedMethodsWriter.WriteLine(string.Format("// {0} {1}({2});", MethodDefinitionType, MethodDefinitionTypeName, ParamsDefinition));
                        if(ParamsDefinitionParams.Length <= 10) {
                            if (ParamsDefinitionParams[0] != "void") {
                                GeneratedMethodsWriter.WriteLine(string.Format("GD_DEFINE_OGL_METHOD_{0}(", ParamsDefinitionParams.Length));
                                GeneratedMethodsWriter.WriteLine(string.Format("\t{0}, {1}", MethodDefinitionType, MethodDefinitionTypeName.Substring("gl".Length)));
                                foreach (var ParamDefinition in ParamsDefinitionParams) {
                                    int ParamDefinitionLastSpace = ParamDefinition.LastIndexOf(' ');
                                    string ParamDefinitionType = ParamDefinition.Substring(0, ParamDefinitionLastSpace).Trim();
                                    string ParamDefinitionName = ParamDefinition.Substring(ParamDefinitionLastSpace).Trim();

                                    // Pointer argument type.
                                    int ParamDefinitionNamePointerIndex = ParamDefinitionName.LastIndexOf('*');
                                    if (ParamDefinitionNamePointerIndex != -1) {
                                        ParamDefinitionType += ParamDefinitionName.Substring(0, ParamDefinitionNamePointerIndex + 1);
                                        ParamDefinitionName  = ParamDefinitionName.Substring(ParamDefinitionNamePointerIndex + 1);
                                    }
                                    GeneratedMethodsWriter.WriteLine(string.Format("\t, {0} const, {1}", ParamDefinitionType, ParamDefinitionName));
                                }
                            } else {
                                GeneratedMethodsWriter.WriteLine(string.Format("GD_DEFINE_OGL_METHOD_0(", ParamsDefinitionParams.Length));
                                GeneratedMethodsWriter.WriteLine(string.Format("\t{0}, {1}", MethodDefinitionType, MethodDefinitionTypeName.Substring("gl".Length)));
                                GeneratedMethodsWriter.WriteLine("\t/**** NO ARGUMENTS ****/");
                            }
                            GeneratedMethodsWriter.WriteLine("\t)");
                        } else {
                            GeneratedMethodsWriter.WriteLine("// ***** SKIPPED DUE ARGUMENTS COUNT LIMITATION *****");
                        }
                    }
                }
            }
        }
    }   // class OpenGLHeader

    internal sealed class OpenGLESCoreHeader : OpenGLHeader
    {
        public const string CoreHeaderFileName = "OpenGL_ES_3.0.Non-Source-C-Header";
        public OpenGLESCoreHeader()
            : base(CoreHeaderFileName)
        {
        }
    }   // class OpenGLESCoreHeader

    internal sealed class OpenGLESExtensionsHeader : OpenGLHeader
    {
        public const string ExtensionsHeaderFileName = "OpenGL_ES_2.0.Extensions.Non-Source-C-Header";
        public OpenGLESExtensionsHeader()
            : base(ExtensionsHeaderFileName)
        {
        }
    }   // class OpenGLESExtensionsHeader

    internal sealed class OpenGL430Header : OpenGLHeader
    {
        public const string _430FileName = "OpenGL_Desktop_4.3.Non-Source-C-Header";
        public OpenGL430Header()
            : base(_430FileName)
        {
        }
    }   // class OpenGLESExtensionsHeader

    internal static class OpenGLUtility
    {
        private static void WriteCopyrightMessage(StreamWriter SomeGeneratedHeader)
        {
            SomeGeneratedHeader.WriteLine("/// ==========================================================================================");
            SomeGeneratedHeader.WriteLine(string.Format("/// {0} - Generated header.", Path.GetFileName((SomeGeneratedHeader.BaseStream as FileStream).Name)));
            SomeGeneratedHeader.WriteLine("/// Copyright (C) $(GODDAMN_DEV) 2011 - Present. All Rights Reserved.");
            SomeGeneratedHeader.WriteLine("/// ");
            SomeGeneratedHeader.WriteLine("/// History:");
            SomeGeneratedHeader.WriteLine("/// \t* Generated by GoddamnOpenGLUtility.");
            SomeGeneratedHeader.WriteLine("/// ==========================================================================================");
        }

        #region Methods header code
        private const string MethodsBegin = @"
#if (!defined(GD_ENGINE_RENDERER_IMPL_OGL_RENDERER))
#	error 'This header should be included inside ""OGLRenderer.h""'
#endif	// if (!defined(GD_ENGINE_RENDERER_IMPL_OGL_RENDERER))

#define GD_DEFINE_OGL_METHOD_GLUE_ARGS(...) \
	__VA_ARGS__

#define GD_DEFINE_OGL_METHOD_DEFINITION(ReturnType, MethodName, ArgumentsDeclarations, ArgumentsPassing) \
	private: \
		ReturnType (* _##MethodName)(ArgumentsDeclarations) = nullptr; \
	public: \
		GDINL ReturnType MethodName(ArgumentsDeclarations) const { \
			return this->_##MethodName(ArgumentsPassing); \
		}

#define GD_DEFINE_OGL_METHOD_LINKED(ReturnType, MethodName, ArgumentsDeclarations, ArgumentsPassing) \
	public: \
		GDINL static ReturnType MethodName(ArgumentsDeclarations) { \
			return GL.##MethodName(ArgumentsPassing); \
		}

#if (!defined(GD_DEFINE_OGL_METHOD))
#	if (!defined(GD_HRI_OGL_ES)) 
#		define GD_DEFINE_OGL_METHOD(ReturnType, MethodName, ArgumentsDeclarations, ArgumentsPassing) GD_DEFINE_OGL_METHOD_DEFINITION(ReturnType, MethodName, ArgumentsDeclarations, ArgumentsPassing)
#	else	// if (!defined(IE_HRI_OGL_ES))
#		define GD_DEFINE_OGL_METHOD(ReturnType, MethodName, ArgumentsDeclarations, ArgumentsPassing) GD_DEFINE_OGL_METHOD_LINKED(ReturnType, MethodName, ArgumentsDeclarations, ArgumentsPassing)
#	endif	// if (!defined(IE_HRI_OGL_ES))
#endif	// if (!defined(GD_HRI_OGL_METHOD_DECL))

#define GD_DEFINE_OGL_METHOD_0(ReturnType, MethodName) \
	GD_DEFINE_OGL_METHOD(ReturnType, MethodName, \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(), \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS() \
		)

#define GD_DEFINE_OGL_METHOD_1(ReturnType, MethodName, T0, A0) \
	GD_DEFINE_OGL_METHOD(ReturnType, MethodName, \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(T0 A0), \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(A0) \
		)

#define GD_DEFINE_OGL_METHOD_2(ReturnType, MethodName, T0, A0, T1, A1) \
	GD_DEFINE_OGL_METHOD(ReturnType, MethodName, \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(T0 A0, T1 A1), \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(A0, A1) \
		)

#define GD_DEFINE_OGL_METHOD_3(ReturnType, MethodName, T0, A0, T1, A1, T2, A2) \
	GD_DEFINE_OGL_METHOD(ReturnType, MethodName, \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(T0 A0, T1 A1, T2 A2), \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(A0, A1, A2) \
		)

#define GD_DEFINE_OGL_METHOD_4(ReturnType, MethodName, T0, A0, T1, A1, T2, A2, T3, A3) \
	GD_DEFINE_OGL_METHOD(ReturnType, MethodName, \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(T0 A0, T1 A1, T2 A2, T3 A3), \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(A0, A1, A2, A3) \
		)

#define GD_DEFINE_OGL_METHOD_5(ReturnType, MethodName, T0, A0, T1, A1, T2, A2, T3, A3, T4, A4) \
	GD_DEFINE_OGL_METHOD(ReturnType, MethodName, \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(T0 A0, T1 A1, T2 A2, T3 A3, T4 A4), \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(A0, A1, A2, A3, A4) \
		)

#define GD_DEFINE_OGL_METHOD_6(ReturnType, MethodName, T0, A0, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5) \
	GD_DEFINE_OGL_METHOD(ReturnType, MethodName, \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(T0 A0, T1 A1, T2 A2, T3 A3, T4 A4, T5 A5), \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(A0, A1, A2, A3, A4, A5) \
		)

#define GD_DEFINE_OGL_METHOD_7(ReturnType, MethodName, T0, A0, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6) \
	GD_DEFINE_OGL_METHOD(ReturnType, MethodName, \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(T0 A0, T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6), \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(A0, A1, A2, A3, A4, A5, A6) \
		)

#define GD_DEFINE_OGL_METHOD_8(ReturnType, MethodName, T0, A0, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, T7, A7) \
	GD_DEFINE_OGL_METHOD(ReturnType, MethodName, \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(T0 A0, T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7), \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(A0, A1, A2, A3, A4, A5, A6, A7) \
		)

#define GD_DEFINE_OGL_METHOD_9(ReturnType, MethodName, T0, A0, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, T7, A7, T8, A8) \
	GD_DEFINE_OGL_METHOD(ReturnType, MethodName, \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(T0 A0, T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8), \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(A0, A1, A2, A3, A4, A5, A6, A7, A8) \
		)

#define GD_DEFINE_OGL_METHOD_10(ReturnType, MethodName, T0, A0, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, T7, A7, T8, A8, T9, A9) \
	GD_DEFINE_OGL_METHOD(ReturnType, MethodName, \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(T0 A0, T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8, T9 A9), \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9) \
		)
		
#define GD_DEFINE_OGL_METHOD_11(ReturnType, MethodName, T0, A0, T1, A1, T2, A2, T3, A3, T4, A4, T5, A5, T6, A6, T7, A7, T8, A8, T9, A9, T10, A10) \
	GD_DEFINE_OGL_METHOD(ReturnType, MethodName, \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(T0 A0, T1 A1, T2 A2, T3 A3, T4 A4, T5 A5, T6 A6, T7 A7, T8 A8, T9 A9, T10 A10), \
		GD_DEFINE_OGL_METHOD_GLUE_ARGS(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) \
		)

// WinAPI has macros for everything..
#if (defined(GD_PLATFORM_API_WINAPI))
#	pragma push_macro(""MemoryBarrier"")
#	undef MemoryBarrier
#	pragma push_macro(""near"")
#	undef near
#	pragma push_macro(""far"")
#	undef far
#endif	// if (defined(GD_PLATFORM_API_WINAPI))

/// ==========================================================================================
/// OpenGL <-> Functionality
/// ==========================================================================================
";

        private const string MethodsEnd = @"
#if (defined(GD_PLATFORM_API_WINAPI))
#	pragma pop_macro(""MemoryBarrier"")
#	pragma pop_macro(""near"")
#	pragma pop_macro(""far"")
#endif	// if (defined(GD_PLATFORM_API_WINAPI))

#undef GD_DEFINE_OGL_METHOD
#undef GD_DEFINE_OGL_METHOD_0
#undef GD_DEFINE_OGL_METHOD_1
#undef GD_DEFINE_OGL_METHOD_2
#undef GD_DEFINE_OGL_METHOD_3
#undef GD_DEFINE_OGL_METHOD_4
#undef GD_DEFINE_OGL_METHOD_5
#undef GD_DEFINE_OGL_METHOD_6
#undef GD_DEFINE_OGL_METHOD_7
#undef GD_DEFINE_OGL_METHOD_8
#undef GD_DEFINE_OGL_METHOD_9
#undef GD_DEFINE_OGL_METHOD_10
#undef GD_DEFINE_OGL_METHOD_11
#undef GD_DEFINE_OGL_METHOD_GLUE_ARGS
";
        #endregion  // Methods header code

        private static void Main(string[] Args)
        {
            try {
                string GeneratedOpenGLMethodsHeaderLocation = Path.Combine(OpenGLHeader.RendererLocation, "OGLRendererMethods.h");
                string GeneratedOpenGLDefinesHeaderLocation = Path.Combine(OpenGLHeader.RendererLocation, "OGLRendererDefines.h");
                using (StreamWriter GeneratedOpenGLMethodsHeader = new StreamWriter(GeneratedOpenGLMethodsHeaderLocation),
                                    GeneratedOpenGLDefinesHeader = new StreamWriter(GeneratedOpenGLDefinesHeaderLocation)) {
                    // Writing Copyright.
                    WriteCopyrightMessage(GeneratedOpenGLMethodsHeader);
                    WriteCopyrightMessage(GeneratedOpenGLDefinesHeader);
                    
                    // Writing beginning code.
                    GeneratedOpenGLMethodsHeader.Write(MethodsBegin);
                    GeneratedOpenGLMethodsHeader.WriteLine("#if (defined(GD_HRI_OGL_ES))");
                    GeneratedOpenGLDefinesHeader.WriteLine("#if (defined(GD_HRI_OGL_ES))");

                    // Writing OpenGL(ES) code.
                    OpenGLESCoreHeader CoreHeader = new OpenGLESCoreHeader();
                    OpenGLESExtensionsHeader ExtensionsHeader = new OpenGLESExtensionsHeader();
                    CoreHeader.Parse(GeneratedOpenGLMethodsHeader, GeneratedOpenGLDefinesHeader);
                    ExtensionsHeader.Parse(GeneratedOpenGLMethodsHeader, GeneratedOpenGLDefinesHeader);

                    // Writing conditional code.
                    GeneratedOpenGLDefinesHeader.WriteLine("#else\t// if (defined(GD_HRI_OGL_ES))");
                    GeneratedOpenGLMethodsHeader.WriteLine("#else\t// if (defined(GD_HRI_OGL_ES))");

                    OpenGL430Header _430Header = new OpenGL430Header();
                    _430Header.Parse(GeneratedOpenGLMethodsHeader, GeneratedOpenGLDefinesHeader);

                    // Writing ending code.
                    GeneratedOpenGLDefinesHeader.WriteLine("#endif\t// if (defined(GD_HRI_OGL_ES))");
                    GeneratedOpenGLMethodsHeader.WriteLine("#endif\t// if (defined(GD_HRI_OGL_ES))");
                    GeneratedOpenGLMethodsHeader.Write(MethodsEnd);
                }
            } catch(IOException/*IOE*/) {
                Environment.Exit(1);
            }
        }
    }   // static sealed class OpenGLUtility
}   // namespace GoddamnEngine.OpenGLUtility
