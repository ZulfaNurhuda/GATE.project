program RecordExample;

type
  Student = record
    name: string;
    age: integer;
    gpa: real;
  end;


var
  student1: Student;

begin
  student1.name := 'John Doe';
  student1.age := 20;
  student1.gpa := 3.75;
  writeln('Student: ', student1.name, ', Age: ', student1.age);
end.
