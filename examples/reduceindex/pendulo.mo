model pendulo
  parameter Real m = 1;
  parameter Real g = 2;
  Real q1;
  Real q2;
  Real v1;
  Real v2;
  Real lambda;
  parameter Real l = 1;
equation
  der(q1) = v1;
  der(q2) = v2;
  m * der(v1) = -(q1 * lambda);
  m * der(v2) = m * g - q2 * lambda;
  0 = q1 * q1 + q2 * q2 - l * l;
end pendulo;
