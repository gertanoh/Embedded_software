pragma Task_Dispatching_Policy(FIFO_Within_Priorities);

with Ada.Text_IO; use Ada.Text_IO;
with Ada.Real_Time; use Ada.Real_Time;

procedure OverloadDetection is
   package Duration_IO is new Ada.Text_IO.Fixed_IO(Duration);
   package Int_IO is new Ada.Text_IO.Integer_IO(Integer);

   Start: Time := Clock;
   HyperPeriod : Time_Span := Milliseconds(1000);

   System_OK : Boolean := False;

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

   task type OkIdleTask is
      pragma Priority(1);
   end OkIdleTask;

   okTask : OkIdleTask;

   task type Watchdog is
      pragma Priority(10);
      entry System_OK;
   end Watchdog;

   Dog : Watchdog;

   task body OkIdleTask is
      NextHyperPeriod: Time;
   begin
      NextHyperPeriod := Start;
      loop
         NextHyperPeriod := NextHyperPeriod + HyperPeriod;
         delay until NextHyperPeriod - Milliseconds(100);
         Dog.System_OK;
      end loop;
   end OkIdleTask;


   task body Watchdog is
      NextHyperPeriod: Time;
   begin
      NextHyperPeriod := Start;
      loop
         NextHyperPeriod := NextHyperPeriod + HyperPeriod;
         delay until NextHyperPeriod - Milliseconds(100);
         select
            accept System_OK;
         or
            delay 0.2;
            Put_Line("System Overload!");
         end select;
      end loop;
   end Watchdog;

   task type T(Task_Id: Integer; Priority: Integer; Period: Integer; ExecTime: Integer) is
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
         Dummy := F(ExecTime);
         -- Log duration and task ID.
         Duration_IO.Put(To_Duration(Clock - Start), 3, 3);
         Put(" : ");
         Int_IO.Put(Task_Id, 2);
         Put_Line("");
         delay until Next;
      end loop;
   end T;

   T1 : T(1, 5, 3*1000/12, 8);
   T2 : T(2, 4, 4*1000/12, 8);
   T3 : T(3, 3, 6*1000/12, 8);
   T4 : T(4, 2, 9*1000/12, 16);

begin -- main task
   null;
end OverloadDetection;