/* include/gate/diagnostics/DiagnosticEngine.h */

#ifndef GATE_DIAGNOSTICS_ENGINE_H
#define GATE_DIAGNOSTICS_ENGINE_H

#include "Diagnostic.h"
#include <functional>
#include <map>

namespace gate::diagnostics {

/* Callback for diagnostic handling */
using DiagnosticHandler = std::function<void(const Diagnostic&)>;

class DiagnosticEngine {
private:
    std::vector<Diagnostic> diagnostics_;
    std::string sourceCode_;
    std::string filename_;
    bool treatWarningsAsErrors_;
    size_t errorCount_;
    size_t warningCount_;
    DiagnosticHandler customHandler_;

    /* Error message templates */
    static const std::map<std::string, std::string> ERROR_TEMPLATES;

public:
    DiagnosticEngine(const std::string& source, const std::string& filename);

    /* Report a diagnostic */
    void report(Diagnostic diagnostic);

    /* Convenience methods for common errors */
    void reportSyntaxError(const SourceLocation& location,
                          const std::string& message);
    void reportTypeError(const SourceLocation& location,
                        const std::string& expected,
                        const std::string& actual);
    void reportUndefinedVariable(const SourceLocation& location,
                                const std::string& variableName);

    /* Generate formatted error output */
    std::string formatDiagnostic(const Diagnostic& diagnostic) const;
    std::string generateReport() const;

    /* Extract source context for error display */
    std::string extractSourceContext(const SourceLocation& location,
                                   const char* levelColor,
                                   const std::string& message) const;

    /* Error state queries */
    bool hasErrors() const { return errorCount_ > 0; }
    bool hasWarnings() const { return warningCount_ > 0; }
    size_t getErrorCount() const { return errorCount_; }
    size_t getWarningCount() const { return warningCount_; }

    /* Configuration */
    void setTreatWarningsAsErrors(bool treat) { treatWarningsAsErrors_ = treat; }
    void setDiagnosticHandler(DiagnosticHandler handler) { customHandler_ = handler; }

    /* Clear all diagnostics */
    void clear();
};

} // namespace gate::diagnostics

#endif // GATE_DIAGNOSTICS_ENGINE_H
