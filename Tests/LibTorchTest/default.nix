{
  lib,
  stdenv,
  custom_libtorch,
  ...
}:
stdenv.mkDerivation {
  name = "libtorch";
  src = custom_libtorch;
  # the rest of your derivation goes here
}
