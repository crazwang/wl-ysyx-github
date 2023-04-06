module mux41 (x[3:0],y,f);
    input [1:0] x [3:0];
    input [1:0] y;
    output[1:0] f;

    MuxKeyWithDefault #(4,2,2) m1 (f,y,2'b00,{
        2'b00,x[0],
        2'b01,x[1],
        2'b10,x[2],
        2'b11,x[3]
    });

endmodule
