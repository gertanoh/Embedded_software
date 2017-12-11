pragma Priority_Specific_Dispatching(FIFO_Within_Priorities, 2, 30);
pragma Priority_Specific_Dispatching(Round_Robin_Within_Priorities, 1, 1);

with Ada.Text_IO; use Ada.Text_IO;
with Ada.Real_Time; use Ada.Real_Time;

procedure Mixedscheduling is

   Start : Time;

   package Duration_IO is new Ada.Text_IO.Fixed_IO(Duration);
   package Int_IO is new Ada.Text_IO.Integer_IO(Integer);

   task type T(Id: Integer; Priority : Integer; Period : Integer) is
      pragma Priority(Priority);
   end;

   function F(N : Integer) return Integer;

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

   task body T is
      Next : Time;
      --X : Integer;
      Dummy : Integer;
   begin
      Next := Start;
      loop
         Next := Next + Milliseconds(Period);
         Dummy := F(10); --103ms
         --X := 0;
         --for Index in 1..5000000 loop
            --X := X + Index;
         --end loop;
         Duration_IO.Put(To_Duration(Clock - Start), 3, 3);
         Put(" : ");
         Int_IO.Put(Id, 2);
         Put_Line("");
         delay until Next;
      end loop;
   end T;

   -- High Priority Tasks

   T1 : T(1, 3, 3*103);
   T2 : T(2, 2, 4*103);
   T3 : T(3, 1, 6*103);

   -- Low Priority Tasks

   T4 : T(4,1,0);
   T5 : T(5,1,0);
   T6 : T(6,1,0);
   
   
begin
   Start := Clock;
   null;
end Mixedscheduling;
