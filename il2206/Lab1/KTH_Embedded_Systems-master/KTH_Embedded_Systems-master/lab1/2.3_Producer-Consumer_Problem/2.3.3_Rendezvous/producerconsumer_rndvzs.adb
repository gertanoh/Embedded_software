with Ada.Text_IO;
use Ada.Text_IO;

with Ada.Real_Time;
use Ada.Real_Time;

with Buffer;
use Buffer;

with Ada.Numerics.Discrete_Random;

procedure ProducerConsumer_rndvzs is
   
   N : constant Integer := 40; -- Number of produced and comsumed variables

   --pragma Volatile(X); -- For a volatile object all reads and updates of
                       -- the object as a whole are performed directly
                       -- to memory (Ada Reference Manual, C.6)

   -- Random Delays
   subtype Delay_Interval is Integer range 50..250;
   package Random_Delay is new Ada.Numerics.Discrete_Random (Delay_Interval);
   use Random_Delay;
   G : Generator;

   task type BufferServer is 
      entry write_one(X: in Integer);
      entry read_one(X: out Integer);
   end BufferServer;

   task body BufferServer is
      Buf : CircularBuffer; -- Protected Object from buffer.adb
      begin
         loop
            accept write_one(X: in Integer) do
               Buf.Write_One(X);
            end write_one;

            accept read_one(X: out Integer) do
               Buf.Read_One(X);
            end read_one;
         end loop;
   end BufferServer;

   BufServ : BufferServer;


   task Producer;

   task Consumer;

   task body Producer is
      Next : Time;
   begin
      Next := Clock;
      for I in 1..N loop
         -- Write to Buffer
         Bc.Write_One(I);
         -- Next 'Release' in 50..250ms
         Next := Next + Milliseconds(Random(G));
         delay until Next;
      end loop;
   end;

   task body Consumer is
      Next : Time;
      X : Integer;
   begin
      Next := Clock;
      for I in 1..N loop
         -- Read from Buffer
         BufServ.Read_One(X);
         Put_Line(Integer'Image(X));
         Next := Next + Milliseconds(Random(G));
         delay until Next;
      end loop;
   end;

begin -- main task
   null;
end ProducerConsumer_rndvzs;


