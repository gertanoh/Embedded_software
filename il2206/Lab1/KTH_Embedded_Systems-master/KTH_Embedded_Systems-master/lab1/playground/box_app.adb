with Ada.Text_IO; use Ada.Text_IO;
with Shared; use Shared;

procedure Box_App is
	Box : Shared_Variable(2);
	N : Integer := 0;
	task Put_Money_Task;
	task body Put_Money_Task is
	begin
		--put_line("test");
		for I in 1..5 loop
			Box.Add(1);
		end loop;
	end Put_Money_Task;

begin -- Box_App
	delay 1.0;
	N := Box.Read;
	Put("N = " & Integer'Image(N));
end Box_App;