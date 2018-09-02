model uncontrollable
  parameter Real a = 1;
  parameter Real b = 1;
  Real x;
  Real u;
  Real v;
equation
  0 = x + u - v;
  0 = x + der(x) + a;
  0 = x + b;
end uncontrollable;