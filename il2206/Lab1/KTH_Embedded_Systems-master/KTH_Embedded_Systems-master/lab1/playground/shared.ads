package Shared is
	protected type Shared_Variable(Initial_Value : Integer)
	is
		procedure Add(Number : Integer);
		procedure Subtract(Number : Integer);
		procedure Write(Number : Integer);
		function Read return Integer;
	private
		X : Integer := Initial_Value;
	end Shared_Variable;
end Shared;	