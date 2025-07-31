unit StringToIntegerUnit;

{$mode objfpc}{$H+}

interface

uses
  SysUtils;

function StringToInteger(const inStr: string; var outInt: Int64): Boolean;
function StringToInteger(const inStr: string; var outInt: Integer): Boolean;

implementation

function StringToInteger(const inStr: string; var outInt: Int64): Boolean;
var
  code: Integer;
  trimmedS: string;
begin
  trimmedS := Trim(inStr);
  if trimmedS = '' then
  begin
    outInt := 0;
    StringToInteger := False;
    Exit;
  end;
  Val(trimmedS, outInt, code);
  StringToInteger := (code = 0);
end;

function StringToInteger(const inStr: string; var outInt: Integer): Boolean;
var
  tempInt64: Int64;
begin
  if StringToInteger(inStr, tempInt64) then
  begin
    if (tempInt64 >= Low(Integer)) and (tempInt64 <= High(Integer)) then
    begin
      outInt := Integer(tempInt64);
      StringToInteger := True;
    end
    else
    begin
      outInt := 0;
      StringToInteger := False; // Out of range for Integer
    end;
  end
  else
  begin
    outInt := 0;
    StringToInteger := False;
  end;
end;

end.
