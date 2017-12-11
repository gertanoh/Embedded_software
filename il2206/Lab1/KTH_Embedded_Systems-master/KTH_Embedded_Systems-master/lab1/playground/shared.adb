package body Shared is
	protected body Shared_Variable is
		procedure Add (Number : Integer) is
		begin
			X := X + Number;
		end Add;

		procedure Subtract(Number : Integer) is
		begin
			X := X - Number;
		end Subtract;

		procedure Write(Number : Integer) is
		begin
			X := Number;
		end;
		
		function Read return Integer is
		begin
			return X;
		end Read;
	end Shared_Variable;
end Shared;	