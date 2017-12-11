with Ada.Text_IO; use Ada.Text_IO;

procedure Simpletasks is
	N : Integer := 0;
	task type Simple;
	T1, T2 : Simple;

	task body Simple is
	begin
		for I in 1..20 loop
			N := N+1;
		end loop;
	end Simple;
begin -- Simpletasks
	delay 3.0;
	Put("N = " & Integer'Image(N));
end Simpletasks;