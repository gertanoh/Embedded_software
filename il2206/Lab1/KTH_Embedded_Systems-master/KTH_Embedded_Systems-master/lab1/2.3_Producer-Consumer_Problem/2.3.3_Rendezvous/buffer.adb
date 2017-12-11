package body Buffer is
   protected body CircularBuffer is
      entry write_one(X: in Integer) when Count < N is
         begin
            A(In_Ptr) := X;
            Count := Count + 1;
            In_Ptr := In_Ptr + 1;
         end;

      entry read_one(X: out Integer) when Count > 0 is
         begin
            X := A(Out_Ptr);
            Count := Count - 1;
            Out_Ptr := Out_Ptr + 1;
         end;
   end CircularBuffer;
end Buffer;