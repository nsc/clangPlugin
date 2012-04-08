set(SYMBOL_FILE DetectTrailingWhiteSpace.exports)

set (CLANG_LIBS
	clangFrontend
	clangAST
	clangAnalysis
	clangBasic
	clangCodeGen
	clangDriver
	clangFrontendTool
	clangIndex
	clangLex
	clangParse
	clangRewrite
	clangSema
	clangSerialization
	clangStaticAnalyzerCheckers
	clangStaticAnalyzerCore
	clangStaticAnalyzerFrontend
)

set (USER_LIBS
	pthread
)

add_clang_plugin(DetectTrailingWhiteSpace DetectTrailingWhiteSpace.cpp)

set_target_properties(DetectTrailingWhiteSpace PROPERTIES
	LINKER_LANGUAGE CXX
	PREFIX "")

