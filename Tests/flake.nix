{
inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.05";
    flake-utils = {
        url = "github:numtide/flake-utils";
        inputs.system.follows = "systems";
    };
    custom_libtorch = {
        url = "github:pytorch/pytorch";
        flake = false;
    };
};

outputs = { self, nixpkgs, flake-utils, ... } @inputs:
    flake-utils.lib.eachDefaultSystem
    (system:
    let
        pkgs = import nixpkgs {
            inherit system;
            config.allowUnfree = true;
        };
        libs = [
            pkgs.libcxx
            pkgs.libtorch-bin
        ];
        libtorch = pkgs.callPackage ./. {
            inherit (inputs) custom_libtorch;
        };
    in
        with pkgs;
            {
            devShells.default = mkShell {
        # default = libtorch;
                nativeBuildInputs = with pkgs; [
                    libtorch
                    bashInteractive
                    libcxx
                    pkg-config
                    gcc
                    cmake
                ];
                buildInputs = with pkgs; [
                    (python311.withPackages(ps: with ps;[
                        # (opencv4.override {enableGtk2 = true;})
                        numpy
                        scipy
                        matplotlib
                        pytorch-bin
                    ]))
                    cudatoolkit
                    # (libtorch-bin.override ({
                    # }))
                    (opencv.override ({
                        enableGtk3 = true;
                        enableCuda = false;
                        enableFfmpeg = true;
                        enableUnfree = true;
                    }))
                ];
                shellHook = ''
                    echo $(inputs.custom_libtorch)
                '';
                LD_LIBRARY_PATH = "${pkgs.lib.makeLibraryPath libs}";
            };
        }
    );
}
