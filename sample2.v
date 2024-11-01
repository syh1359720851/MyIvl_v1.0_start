module sample2(clk, rst,y,z,a,b,c,d,e);
input clk,rst;
output y;
output z;
input  a;
input  b;
input  c;
input  d;
input  e;

wire  a;
wire  b;
wire  c;
wire  d;
wire  e;
wire  y;
wire  z;
wire  g1;
wire  g2;
wire  g3;

assign g1 =  a & b ;
assign g2 =  b & c ;
assign g3 =  d & e ;
assign z =  g1  |  g2 ;
assign y =  g2  |  g3 ;

endmodule
