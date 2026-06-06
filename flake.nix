{
  description = "Teehee";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs =
    {
      self,
      nixpkgs,
      flake-utils,
      ...
    }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in
      rec {
        defaultPackage = pkgs.stdenv.mkDerivation rec {
          LD_LIBRARY_PATH = "${pkgs.lib.makeLibraryPath buildInputs}";
          name = "teehee";
          src = ./.;
          nativeBuildInputs = with pkgs; [
            gcc
            pkg-config
            wayland-scanner
            wayland-protocols
            wlr-protocols
            wlroots_0_20
            libxkbcommon
            pixman
          ];
          buildInputs = with pkgs; [
            wayland
          ];
          makeFlags = [ "PREFIX=$(out)" ];
        };

        devShell = pkgs.mkShell {
          shellHook = "";
          buildInputs =
            defaultPackage.nativeBuildInputs
            ++ defaultPackage.buildInputs
            ++ (with pkgs; [
              gnumake
              gdb
            ]);
        };
      }
    );
}
