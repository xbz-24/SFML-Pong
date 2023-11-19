{ pkgs ? import <nixpkgs> {}, lib ? pkgs.lib }:

pkgs.mkShell {
  buildInputs = [
    pkgs.cmake
    pkgs.sfml
    pkgs.box2d
    pkgs.pkg-config
    pkgs.gdb
  ];
}
