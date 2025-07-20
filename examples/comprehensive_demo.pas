program ComprehensiveDemo;

type
  Student = record
    id: integer;
    name: string;
    gpa: real;
    year: integer;
  end;

  Course = record
    code: string;
    title: string;
    credits: integer;
    instructor: string;
  end;

  Grade = (A, B, C, D, F);

  Semester = (Fall, Spring, Summer);

  Status = (Enrolled, Graduated, Withdrawn, OnHold);


var
  student1: Student;
  course1: Course;
  finalGrade: Grade;
  currentSem: Semester;
  studentStatus: Status;
  counter: integer;
  message: string;
  isValid: boolean;
  studentAge: integer;
  gpaScore: real;
  creditHours: integer;
  attendance: real;
  examScore: integer;

procedure SetstudentAge(var studentAge: integer; value: integer);
begin
  Assert((((studentAge >= 16) and (studentAge <= 65))), 'Error: studentAge constraint violation!');
  studentAge := value;
end;

procedure SetgpaScore(var gpaScore: real; value: real);
begin
  Assert((((gpaScore >= 0) and (gpaScore <= 4))), 'Error: gpaScore constraint violation!');
  gpaScore := value;
end;

procedure SetcreditHours(var creditHours: integer; value: integer);
begin
  Assert((((creditHours >= 1) and (creditHours <= 6))), 'Error: creditHours constraint violation!');
  creditHours := value;
end;

procedure Setattendance(var attendance: real; value: real);
begin
  Assert((((attendance >= 0) and (attendance <= 100))), 'Error: attendance constraint violation!');
  attendance := value;
end;

procedure SetexamScore(var examScore: integer; value: integer);
begin
  Assert((((examScore >= 0) and (examScore <= 100))), 'Error: examScore constraint violation!');
  examScore := value;
end;

begin
  counter := 1;
  message := 'NOTAL Transpiler with Records, Enums, and Constraints!';
  isValid := true;
  studentAge := 20;
  gpaScore := 3.75;
  creditHours := 4;
  attendance := 95;
  examScore := 87;
  writeln('=== NOTAL TRANSPILER DEMO ===');
  writeln('Message: ', message);
  writeln('Counter: ', counter);
  writeln('Valid: ', isValid);
  writeln('');
  writeln('=== CONSTRAINED VARIABLES ===');
  writeln('Student Age: ', studentAge);
  writeln('GPA Score: ', gpaScore);
  writeln('Credit Hours: ', creditHours);
  writeln('Attendance: ', attendance, '%');
  writeln('Exam Score: ', examScore);
  writeln('');
  writeln('All features implemented successfully!');
end.
