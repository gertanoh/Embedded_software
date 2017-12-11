-- Package: Semaphores

package body Semaphores is
   protected body CountingSemaphore is
      entry Wait when Count > 0 is -- To be completed
      	begin
      		count := count - 1;
      	end;
      	
      entry Signal when count < MaxCount is -- To be completed
      	begin
      		count := count + 1;
      	end;
   end CountingSemaphore;
end Semaphores;

