{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = [
    # C++ compilers
    pkgs.gcc
    pkgs.clang

    # Build tools
    pkgs.cmake
    pkgs.ninja

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
