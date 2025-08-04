#include <gtest/gtest.h>
#include "diagnostics/DiagnosticEngine.h"
#include "diagnostics/Diagnostic.h"
#include <string>

TEST(DiagnosticEngineTest, BasicErrorReporting) {
    std::string source = "PROGRAM Test\nKAMUS\n    x: integer\nALGORITMA\n    x <- 42";
    gate::diagnostics::DiagnosticEngine engine(source, "test.notal");
    
    gate::diagnostics::SourceLocation location("test.notal", 2, 5, 10);
    engine.reportSyntaxError(location, "Expected semicolon");
    
    EXPECT_TRUE(engine.hasErrors());
    EXPECT_EQ(engine.getErrorCount(), 1);
    EXPECT_EQ(engine.getWarningCount(), 0);
}

TEST(DiagnosticEngineTest, TypeErrorReporting) {
    std::string source = "PROGRAM Test\nKAMUS\n    x: integer\nALGORITMA\n    x <- 'hello'";
    gate::diagnostics::DiagnosticEngine engine(source, "test.notal");
    
    gate::diagnostics::SourceLocation location("test.notal", 4, 10, 25);
    engine.reportTypeError(location, "integer", "string");
    
    EXPECT_TRUE(engine.hasErrors());
    EXPECT_EQ(engine.getErrorCount(), 1);
    
    std::string report = engine.generateReport();
    EXPECT_TRUE(report.find("Type error") != std::string::npos);
    EXPECT_TRUE(report.find("integer") != std::string::npos);
    EXPECT_TRUE(report.find("string") != std::string::npos);
}

TEST(DiagnosticEngineTest, UndefinedVariableError) {
    std::string source = "PROGRAM Test\nKAMUS\nALGORITMA\n    output(undefinedVar)";
    gate::diagnostics::DiagnosticEngine engine(source, "test.notal");
    
    gate::diagnostics::SourceLocation location("test.notal", 3, 12, 30);
    engine.reportUndefinedVariable(location, "undefinedVar");
    
    EXPECT_TRUE(engine.hasErrors());
    EXPECT_EQ(engine.getErrorCount(), 1);
    
    std::string report = engine.generateReport();
    EXPECT_TRUE(report.find("undefinedVar") != std::string::npos);
}

TEST(DiagnosticEngineTest, MultipleErrors) {
    std::string source = "PROGRAM Test\nKAMUS\n    x: integer\nALGORITMA\n    x <- 'hello'\n    output(y)";
    gate::diagnostics::DiagnosticEngine engine(source, "test.notal");
    
    gate::diagnostics::SourceLocation location1("test.notal", 4, 10, 25);
    gate::diagnostics::SourceLocation location2("test.notal", 5, 12, 40);
    
    engine.reportTypeError(location1, "integer", "string");
    engine.reportUndefinedVariable(location2, "y");
    
    EXPECT_TRUE(engine.hasErrors());
    EXPECT_EQ(engine.getErrorCount(), 2);
    
    std::string report = engine.generateReport();
    EXPECT_TRUE(report.find("Type error") != std::string::npos);
    EXPECT_TRUE(report.find("Undefined variable") != std::string::npos);
}

TEST(DiagnosticEngineTest, WarningReporting) {
    std::string source = "PROGRAM Test\nKAMUS\n    x: integer\nALGORITMA\n    x <- 42";
    gate::diagnostics::DiagnosticEngine engine(source, "test.notal");
    
    gate::diagnostics::SourceLocation location("test.notal", 3, 5, 15);
    gate::diagnostics::Diagnostic warning{
        gate::diagnostics::DiagnosticLevel::WARNING,
        gate::diagnostics::DiagnosticCategory::SEMANTIC_ERROR,
        "W001",
        "Variable 'x' is assigned but never used",
        location,
        "",
        {},
        {}
    };
    
    engine.report(warning);
    
    EXPECT_FALSE(engine.hasErrors());
    EXPECT_EQ(engine.getErrorCount(), 0);
    EXPECT_EQ(engine.getWarningCount(), 1);
}

TEST(DiagnosticEngineTest, SourceContextExtraction) {
    std::string source = "PROGRAM Test\nKAMUS\n    x: integer\nALGORITMA\n    x <- 42";
    gate::diagnostics::DiagnosticEngine engine(source, "test.notal");
    
    gate::diagnostics::SourceLocation location("test.notal", 3, 5, 20);
    std::string context = engine.extractSourceContext(location, "\033[31m", "Test error");
    
    EXPECT_TRUE(context.find("x: integer") != std::string::npos);
    EXPECT_TRUE(context.find("3") != std::string::npos); // Line number
}

TEST(DiagnosticEngineTest, DiagnosticFormatting) {
    std::string source = "PROGRAM Test\nKAMUS\n    x: integer\nALGORITMA\n    x <- 42";
    gate::diagnostics::DiagnosticEngine engine(source, "test.notal");
    
    gate::diagnostics::SourceLocation location("test.notal", 3, 5, 20);
    gate::diagnostics::Diagnostic diagnostic{
        gate::diagnostics::DiagnosticLevel::ERROR,
        gate::diagnostics::DiagnosticCategory::SYNTAX_ERROR,
        "E001",
        "Test error message",
        location,
        "",
        {},
        {}
    };
    
    std::string formatted = engine.formatDiagnostic(diagnostic);
    
    EXPECT_TRUE(formatted.find("error") != std::string::npos);
    EXPECT_TRUE(formatted.find("Test error message") != std::string::npos);
    EXPECT_TRUE(formatted.find("test.notal") != std::string::npos);
}