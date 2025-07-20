program AllFeaturesTest;

type
  Employee = record
    id: integer;
    name: string;
    salary: real;
    department: string;
  end;

  Address = record
    street: string;
    city: string;
    zipCode: integer;
  end;

  Department = (HR, IT, Finance, Marketing, Sales);

  Priority = (Low, Medium, High, Critical);

  Status = (Active, Inactive, Pending, Suspended);


var
  emp1: Employee;
  empDept: Department;
  taskPriority: Priority;
  userStatus: Status;
  homeAddress: Address;
  counter: integer;
  message: string;
  employeeAge: integer;
  salary: real;
  workHours: integer;
  rating: real;

procedure SetemployeeAge(var employeeAge: integer; value: integer);
begin
  Assert((((employeeAge >= 18) and (employeeAge <= 70))), 'Error: employeeAge constraint violation!');
  employeeAge := value;
end;

procedure Setsalary(var salary: real; value: real);
begin
  Assert((((salary >= 1000) and (salary <= 100000))), 'Error: salary constraint violation!');
  salary := value;
end;

procedure SetworkHours(var workHours: integer; value: integer);
begin
  Assert((((workHours >= 1) and (workHours <= 60))), 'Error: workHours constraint violation!');
  workHours := value;
end;

procedure Setrating(var rating: real; value: real);
begin
  Assert((((rating >= 0) and (rating <= 10))), 'Error: rating constraint violation!');
  rating := value;
end;

begin
  counter := 1;
  message := 'All features implemented successfully!';
  employeeAge := 30;
  salary := 50000;
  workHours := 40;
  rating := 8.5;
  writeln('Test completed: ', message);
  writeln('Counter: ', counter);
  writeln('Employee age: ', employeeAge);
  writeln('Salary: ', salary);
  writeln('Work hours: ', workHours);
  writeln('Rating: ', rating);
end.
