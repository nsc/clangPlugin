#include "clang/StaticAnalyzer/Core/Checker.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/CheckerContext.h"
#include "clang/StaticAnalyzer/Core/CheckerRegistry.h"
#include "clang/StaticAnalyzer/Core/BugReporter/BugType.h"

using namespace clang;
using namespace ento;

namespace {
class TestChecker : public Checker < check::PostStmt<CastExpr> > {
  mutable OwningPtr<BugType> BT;

public:
  void checkPostStmt(const CastExpr *CE, CheckerContext &C) const;

};
} // end anonymous namespace

void TestChecker::checkPostStmt(const CastExpr *CE, CheckerContext &C) const {
//    const ProgramStateRef state = C.getState();
//    const LocationContext *LC = C.getLocationContext();
    if (CE->getCastKind() == CK_PointerToIntegral) {
        
        const ExplicitCastExpr *explicitCast = dyn_cast<const ExplicitCastExpr>(CE);
        if (explicitCast) {
            QualType type = explicitCast->getTypeAsWritten();
            if (type.getAsString() == "BOOL") {
                ExplodedNode *N = C.addTransition();
                if (!N) {
                    return;
                }
                
                if (!BT)
                    BT.reset(new BugType("Cast of pointer to BOOL may give misleading results. Maybe you should check if the pointer is nil instead.", "example analyzer plugin"));
                
                BugReport *report = new BugReport(*BT, BT->getName(), N);
                report->addRange(CE->getSourceRange());
                C.emitReport(report);
            }
        }
    }
}

// Register plugin!
extern "C"
void clang_registerCheckers (CheckerRegistry &registry) {
  registry.addChecker<TestChecker>("example.TestChecker", "Checks for pointers cast to BOOL");
}

extern "C"
const char clang_analyzerAPIVersionString[] = CLANG_ANALYZER_API_VERSION_STRING;
