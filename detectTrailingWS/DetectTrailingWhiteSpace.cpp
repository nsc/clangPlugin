#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/AST.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/FileManager.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;

namespace {

class DetectTrailingWhiteSpaceConsumer : public ASTConsumer {
public:
virtual void HandleTranslationUnit (ASTContext &context)
{
    // register our diagnostics
    DiagnosticsEngine& diagnostics = context.getDiagnostics();
    unsigned int whiteSpaceAtEndOfLineWarning = diagnostics.getDiagnosticIDs()->getCustomDiagID(DiagnosticIDs::Warning, "white space at end of line");

    SourceManager& sourceManager = context.getSourceManager();
    FileID fileID = sourceManager.getMainFileID();
    //const FileEntry *fileEntry = sourceManager.getFileEntryForID(fileID);
    //const char *fileName = fileEntry->getName();

    llvm::StringRef s = sourceManager.getBuffer(fileID)->getBuffer();
    SourceLocation start = sourceManager.getLocForStartOfFile(fileID);

    const char* code = s.data();
    size_t length = s.size();
    for (size_t i=0; i < length; ++i)
    {
        if (code[i] == '\n')
        {
            int64_t j=i-1;
            bool spaceAtTheEnd = false;
            bool nonWhiteSpaceUnderCursor = false;
            while (j >= 0)
            {
                switch (code[j])
                {
                    case ' ':
                    case '\t':
                    case '\v':
                    {
                        spaceAtTheEnd = true;
                        break;
                    }
                    default:
                        nonWhiteSpaceUnderCursor = true;
                        break;
                }
                if (nonWhiteSpaceUnderCursor)
                    break;
                --j;
            }

            if (spaceAtTheEnd)
            {
				SourceLocation wsStart = start.getLocWithOffset(j+1);
				SourceLocation wsEnd = start.getLocWithOffset(i);
				CharSourceRange range;
				range.setBegin(wsStart);
				range.setEnd(wsEnd);
				FixItHint hint = FixItHint::CreateRemoval(range);

                FullSourceLoc l(wsStart, sourceManager);
                DiagnosticBuilder b = diagnostics.Report(l, whiteSpaceAtEndOfLineWarning);
				b.AddSourceRange(range);
				b.AddString("remove trailing whitespace");
				b.AddFixItHint(hint);
            }
        }
    }
}

};

class DetectTrailingWhiteSpaceAction : public PluginASTAction {
public:
  DetectTrailingWhiteSpaceAction() {
      llvm::outs() << "DetectTrailingWhiteSpace\n";
  }
protected:
  ASTConsumer *CreateASTConsumer(CompilerInstance &CI, llvm::StringRef) {
    return new DetectTrailingWhiteSpaceConsumer();
  }

  bool ParseArgs(const CompilerInstance &CI,
                 const std::vector<std::string>& args) {
    for (unsigned i = 0, e = args.size(); i != e; ++i) {
      llvm::errs() << "DetectTrailingWhiteSpace arg = " << args[i] << "\n";

      // Example error handling.
      if (args[i] == "-an-error") {
        DiagnosticsEngine &D = CI.getDiagnostics();
        unsigned DiagID = D.getCustomDiagID(
          DiagnosticsEngine::Error, "invalid argument '" + args[i] + "'");
        D.Report(DiagID);
        return false;
      }
    }
    if (args.size() && args[0] == "help")
      PrintHelp(llvm::errs());

    return true;
  }
  void PrintHelp(llvm::raw_ostream& ros) {
    ros << "Help for DetectTrailingWhiteSpace plugin goes here\n";
  }

};

}

static FrontendPluginRegistry::Add<DetectTrailingWhiteSpaceAction> X("detect-trailing-ws", "detect trailing white space");

/*
__attribute__((constructor)) void f()
{
    llvm::outs() << "Hello\n";
    FrontendPluginRegistry::Add<DetectTrailingWhiteSpaceAction> X("detect-trailing-ws", "detect trailing white space");
}
*/
