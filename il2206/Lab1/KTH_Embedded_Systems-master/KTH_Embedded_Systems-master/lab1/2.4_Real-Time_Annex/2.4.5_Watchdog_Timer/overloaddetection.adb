pragma Task_Dispatching_Policy(FIFO_Within_Priorities);

with Ada.Text_IO; use Ada.Text_IO;
with Ada.Real_Time; use Ada.Real_Time;

procedure Overloaddetection is

   Start : Time;
   HyperPeriod : Time_Span := Milliseconds(36*103);

   package Duration_IO is new Ada.Text_IO.Fixed_IO(Duration);
   package Int_IO is new Ada.Text_IO.Integer_IO(Integer);

   task type T(Id: Integer; Priority : Integer; Period : Integer; e:Integer) is
      pragma Priority(Priority);
   end;

   task type Watchdog is
      pragma Priority(10);
      entry OK;
   end Watchdog;

   task body Watchdog is
      ErrorTime : Time;
      begin
      ErrorTime := Clock;
         loop
            accept OK do
               ErrorTime := Clock;
               --Put_Line("No Overload");
            end OK;

            if Clock > (ErrorTime + HyperPeriod) then
               Put_Line("Overload Detected!!!!");
            end if;
         end loop;   
      end;


   task type OK is
      pragma Priority(1);
   end OK;

   Dog : Watchdog;
   SysOK : OK;

   task body OK is
      NextHyperPeriod : Time;
      begin
         NextHyperPeriod := Clock;
         loop
            NextHyperPeriod := NextHyperPeriod + HyperPeriod;
            Dog.OK;
         end loop;
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
         Dummy := F(e); --10 = 103ms, 20 = 203ms
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

   -- Example Task

   T1 : T(1, 5, 3*103, 10);
   T2 : T(2, 4, 4*103, 10);
   T3 : T(3, 3, 6*103, 10);
   T4 : T(4, 2, 9*103, 20);

   
   
begin
   Start := Clock;
   null;
end Overloaddetection;
