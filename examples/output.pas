program EnumExample;

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
