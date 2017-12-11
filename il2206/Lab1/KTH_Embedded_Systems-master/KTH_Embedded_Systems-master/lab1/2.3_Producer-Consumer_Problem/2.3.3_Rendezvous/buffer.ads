package Buffer is
   N: constant Integer := 4;
   subtype Item is Integer;
   type Index is mod N;
   type Item_Array is array(Index) of Item;
   status: Integer := 0; -- 0 = Write, 1 = Read

   protected type CircularBuffer is
      -- To be completed
      entry write_one(X: in Integer);
      entry read_one(X: out Integer);
   private
      A: Item_Array;
      In_Ptr, Out_Ptr: Index := 0;
      Count: Integer range 0..N := 0;
   end CircularBuffer;
end Buffer;

