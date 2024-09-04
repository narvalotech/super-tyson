{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = [
    # C++ compiler (GCC or Clang)
    pkgs.gcc
    pkgs.clang

    # CMake
    pkgs.cmake

    # Other useful tools
    pkgs.gdb
    pkgs.valgrind
    pkgs.clang-tools
    pkgs.ccache
  ];

  # Shell hook to set up environment
  shellHook = ''
    echo "Entered C++ development environment"
  '';
}
