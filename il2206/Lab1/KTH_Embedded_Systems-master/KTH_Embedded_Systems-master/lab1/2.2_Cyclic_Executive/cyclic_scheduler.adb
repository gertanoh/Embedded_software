with Ada.Text_IO;
use Ada.Text_IO;

with Ada.Real_Time;
use Ada.Real_Time;

with System_Function_Package;
use System_Function_Package;

procedure Cyclic_Scheduler is
   X : Integer := 0; -- Input
   Y : Integer := 0; -- Input

   task Source_X;
   task Source_Y;
   task Scheduler;

   task body Source_X is -- Generates Data for Input X
      Start : Time;
      Next_Release : Time;
      Release : Time_Span := Milliseconds(0);
      Period : Time_Span := Milliseconds(1000);
   begin
      Start := Clock;
      Next_Release := Start + Release;
      loop
         delay until Next_Release;
         Next_Release:= Next_Release + Period;
         X := X + 1;
      end loop;
   end Source_X;

   task body Source_Y is -- Generated Data for Input Y
      Start : Time;
      Next_Release : Time;
      Release: Time_Span := Milliseconds(500);
      Period : Time_Span := Milliseconds(1000);
   begin
      Start := Clock;
      Next_Release := Start + Release;
      loop
         delay until Next_Release;
         Next_Release:= Next_Release + Period;
         Y := Y + 1;
      end loop;
   end Source_Y;

   task body Scheduler is
      Z : Integer; -- Output
      X_tmp : Integer;
      Y_tmp : Integer;
      Start : Time;
      Y_Release: Time_Span := Milliseconds(500);
      Period : Time_Span := Milliseconds(1000);
      A_e : Time_Span := Milliseconds(100);
      B_e : Time_Span := Milliseconds(200);
      C_e : Time_Span := Milliseconds(200);
   begin
      Start := Clock;
      loop
         delay until Start + Milliseconds(50);
         X_tmp := System_A(X);
         Put(Duration'Image(To_Duration(A_e + Milliseconds(50))));
         Put_Line(": A executed");
         delay until (Start + Y_Release);
         Y_tmp := System_B(Y);
         Put(Duration'Image(To_Duration(Y_Release + B_e)));
         Put_Line(": B executed");
         delay until  (Start + Y_Release + B_e);
         Z := System_C(X_tmp,Y_tmp);
         Put(Duration'Image(To_Duration(Y_Release + B_e + C_e)));
         Put_Line(": C executed");
         Put_Line("X = " & Integer'Image(X));
         Put_Line("X_tmp = " & Integer'Image(X_tmp));
         Put_Line("Y = " & Integer'Image(Y));
         Put_Line("Y_tmp = " & Integer'Image(Y_tmp));
         Put_Line("Z = " & Integer'Image(Z));
         Start := Start + Period;
         delay until Start;

      end loop;
   end Scheduler;

begin
   null;
end Cyclic_Scheduler;

