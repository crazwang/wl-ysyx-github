module top(
	input a,
	input b,
	output [2:0] f
);
	assign f = {a , b , a ^ b};
	
endmodule
