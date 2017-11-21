model rlc_total
  Real ground1_p_i;
  parameter Real resistor1_R(start=1)=1;
  parameter Real resistor1_T_ref=300.15;
  parameter Real resistor1_alpha=0;
  parameter Real resistor1_useHeatPort=0;
  parameter Real resistor1_T=resistor1_T_ref;
  Real resistor1_LossPower;
  parameter Real capacitor1_C(start=1)=1;
  Real capacitor1_v(start=1);
  Real capacitor1_n_i;
  parameter Real capacitor2_C(start=1)=1;
  Real capacitor2_v(start=1);
  Real capacitor2_n_i;
equation
  resistor1_LossPower = capacitor2_v*capacitor2_v;
  (-capacitor1_n_i) = der(capacitor1_v);
  capacitor1_v = capacitor2_v;
  (-capacitor2_n_i) = der(capacitor2_v);
  (-capacitor2_n_i)+capacitor2_v+(-capacitor1_n_i) = 0;
  (-capacitor2_v)+ground1_p_i+capacitor2_n_i+capacitor1_n_i = 0;
end rlc_total;
