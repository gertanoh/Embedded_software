with Ada.Text_IO; use Ada.Text_IO;

procedure Concurrency1 is
	N : Integer := 0;
	task T1;
	task body T1 is
	begin
		for I in 1..2 loop
			N := N * 2;
		end loop;
	end T1;

	task T2;
	task body T2 is
	begin
		for I in 1..2 loop
			N := N + 2;
		end loop;
	end T2;
	
begin -- Concurrency1
	delay 1.0;
	Put("N = " & Integer'Image(N));
end Concurrency1;


-- For some reason the task 2 always runs firs...