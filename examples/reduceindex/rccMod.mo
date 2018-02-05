model rcc_total
  parameter Real c1=1;
  parameter Real c2=2;
  Real i1(start=1);
  Real u1(start=1);
  Real u2(start=1);
  Real i0(start=1);
  Real i2(start=1);
  parameter Real R=1;
  parameter Real u0=1;
equation
  c1*der(u1) = i1;
  c2*der(u2) = i2;
  u1 = u2;
  i0+i1+i2 = 0;
  u0 = R*i0+u1;
  der(u1,time) = der(u2,time);
end rlc_total;
