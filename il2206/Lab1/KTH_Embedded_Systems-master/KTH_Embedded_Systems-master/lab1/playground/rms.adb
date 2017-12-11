pragma Task_Dispatching_Policy(FIFO_Within_Priorities);

with Ada.Text_IO; use Ada.Text_IO;
with Ada.Real_Time; use Ada.Real_Time;

procedure RMS is
   package Duration_IO is new Ada.Text_IO.Fixed_IO(Duration);
   package Int_IO is new Ada.Text_IO.Integer_IO(Integer);

   Start: Time := Clock;

   function F(N : Integer) return Integer is
      X : Integer := 0;
   begin
      for Index in 1..N loop
         for I in 1..5000000 loop
            X := I;
         end loop;
      end loop;
      return X;
   end F;

   task type T(Task_Id: Integer; Priority: Integer; Period : Integer) is
      pragma Priority(Priority);
   end;

   task body T is
      Next : Time;
      Dummy : Integer;
   begin
      Next := Start;
      loop
         Next := Next + Milliseconds(Period);
         -- Some dummy function
         Dummy := F(4);
         -- Log duration and task ID.
         Duration_IO.Put(To_Duration(Clock - Start), 3, 3);
         Put(" : ");
         Int_IO.Put(Task_Id, 2);
         Put_Line("");
         delay until Next;
      end loop;
   end T;

   T1 : T(1, 3, 3*1000/12);
   T2 : T(2, 2, 4*1000/12);
   T3 : T(3, 1, 6*1000/12);

begin -- main task
   null;
end RMS;
