#include <gtest/gtest.h>
#include "../helpers/test_helpers.h"

TEST(EnumDependOnTest, BasicEnumDeclaration) {
    std::string notalCode = R"(
PROGRAM EnumExample

KAMUS
    type Day: (monday, tuesday, wednesday, thursday, friday, saturday, sunday)
    type Grade: (A, B, C, D, E, F)
    
    today: Day
    myGrade: Grade
    
ALGORITMA
    today <- monday
    myGrade <- A
    output('Today is: ', today)
    output('My grade is: ', myGrade)
)";
    std::string expected_pascal_code = R"(program EnumExample;

type
  Day = (monday, tuesday, wednesday, thursday, friday, saturday, sunday);
  Grade = (A, B, C, D, E, F);

var
  today: Day;
  myGrade: Grade;

begin
  today := monday;
  myGrade := A;
  writeln('Today is: ', today);
  writeln('My grade is: ', myGrade);
end.
)";
    std::string generated_code = transpile(notalCode);
    ASSERT_EQ(normalizeCode(generated_code), normalizeCode(expected_pascal_code));
}

TEST(EnumDependOnTest, BasicDependOnStatement) {
    std::string notalCode = R"(
PROGRAM DependOnExample

KAMUS
    op: character

ALGORITMA
    op <- '+'
    depend on (op)
        '+': output('Operasi Penjumlahan')
        '-': output('Operasi Pengurangan')
        '*', '/': output('Operasi Perkalian atau Pembagian')
        otherwise: output('Operator tidak dikenal')
)";
    std::string expected_pascal_code = R"(program DependOnExample;

var
  op: char;

begin
  op := '+';
  case op of
    '+': writeln('Operasi Penjumlahan');
    '-': writeln('Operasi Pengurangan');
    '*', '/': writeln('Operasi Perkalian atau Pembagian');
  else
    writeln('Operator tidak dikenal');
  end;
end.
)";
    std::string generated_code = transpile(notalCode);
    ASSERT_EQ(normalizeCode(generated_code), normalizeCode(expected_pascal_code));
}

TEST(EnumDependOnTest, ConditionalDependOn) {
    std::string notalCode = R"(
PROGRAM ConditionalDependOnExample

KAMUS
    nilai: integer
    status: string

ALGORITMA
    nilai <- 85
    depend on (true)
        nilai >= 90: status <- 'Sangat Baik'
        nilai >= 75: status <- 'Baik'
        otherwise: status <- 'Perlu Perbaikan'
    output('Status: ', status)
)";
    std::string expected_pascal_code = R"(program ConditionalDependOnExample;

var
  nilai: integer;
  status: string;

begin
  nilai := 85;
  if (nilai >= 90) then
    status := 'Sangat Baik'
  else if (nilai >= 75) then
    status := 'Baik'
  else
    status := 'Perlu Perbaikan';
  writeln('Status: ', status);
end.
)";
    std::string generated_code = transpile(notalCode);
    ASSERT_EQ(normalizeCode(generated_code), normalizeCode(expected_pascal_code));
}

TEST(EnumDependOnTest, EnumWithDependOn) {
    std::string notalCode = R"(
PROGRAM EnumDependOnExample

KAMUS
    type Status: (active, inactive, pending)
    currentStatus: Status
    message: string

ALGORITMA
    currentStatus <- active
    depend on (currentStatus)
        active: message <- 'System is running'
        inactive: message <- 'System is stopped'
        pending: message <- 'System is waiting'
        otherwise: message <- 'Unknown status'
    output(message)
)";
    std::string expected_pascal_code = R"(program EnumDependOnExample;

type
  Status = (active, inactive, pending);

var
  currentStatus: Status;
  message: string;

begin
  currentStatus := active;
  case currentStatus of
    active: message := 'System is running';
    inactive: message := 'System is stopped';
    pending: message := 'System is waiting';
  else
    message := 'Unknown status';
  end;
  writeln(message);
end.
)";
    std::string generated_code = transpile(notalCode);
    ASSERT_EQ(normalizeCode(generated_code), normalizeCode(expected_pascal_code));
}