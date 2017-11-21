model rlc_total
  Real ground1_p_i;
  parameter Real resistor1_R(start=1)=1;
  parameter Real resistor1_T_ref=300.15;
  parameter Real resistor1_alpha=0;
  Real resistor1_p_v;
  parameter Real resistor1_useHeatPort=0;
  parameter Real resistor1_T=resistor1_T_ref;
  Real resistor1_LossPower;
  parameter Real capacitor1_C(start=1)=1;
  Real capacitor1_v(start=1);
  parameter Real inductor1_L(start=1)=1;
  Real inductor1_v;
  Real inductor1_i(start=0);
equation
  resistor1_LossPower = resistor1_p_v*resistor1_p_v;
  (-resistor1_p_v) = der(capacitor1_v);
  der(inductor1_i) = inductor1_v;
  inductor1_v = resistor1_p_v-capacitor1_v;
  inductor1_i = (-resistor1_p_v);
  resistor1_p_v+ground1_p_i+(-resistor1_p_v) = 0;
end rlc_total;
