set(SYMBOL_FILE TestChecker.exports)

set (CLANG_LIBS
	clangFrontend
	clangAST
	clangAnalysis
	clangBasic
	clangCodeGen
	clangDriver
	clangEdit
	clangFrontendTool
	clangLex
	clangParse
	clangSema
	clangSerialization
	clangStaticAnalyzerCheckers
	clangStaticAnalyzerCore
	clangStaticAnalyzerFrontend
)

set (USER_LIBS
	pthread
)

add_clang_plugin(TestChecker TestChecker.cpp)

set_target_properties(TestChecker PROPERTIES
	LINKER_LANGUAGE CXX
	PREFIX "")
